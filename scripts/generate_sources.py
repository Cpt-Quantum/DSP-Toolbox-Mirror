import argparse
import math
import os
import struct
import sys
from jinja2 import Environment, FileSystemLoader

# Configuration for different types
types_config = [
    {
        "name": "float",
        "suffix": "float",
        "math_suffix": "f",
        "math_prefix": "(float)",
        "literal_suffix": "f",
        "math_type": "float",
        "fixed_point": False,
        "acc_type": "float",
        "limits": None,
    },
    {
        "name": "double",
        "suffix": "double",
        "math_suffix": "",
        "math_prefix": "(double)",
        "literal_suffix": "",
        "math_type": "double",
        "fixed_point": False,
        "acc_type": "double",
        "limits": None,
    },
    {
        "name": "int32_t",
        "suffix": "int32",
        "math_suffix": "f",  # Use float math for generation
        "math_prefix": "(int32_t)",  # Cast result back to int32_t
        "literal_suffix": "f",
        "math_type": "float",
        # The integer filters use Q format coefficients, accumulating in a
        # wider type and saturating the output back to the sample type
        "fixed_point": True,
        "acc_type": "int64_t",
        "limits": ("INT32_MIN", "INT32_MAX"),
    },
    {
        "name": "int64_t",
        "suffix": "int64",
        "math_suffix": "",  # Use double math for generation
        "math_prefix": "(int64_t)",  # Cast result back to int64_t
        "literal_suffix": "",
        "math_type": "double",
        # No wider type is portable, so the accumulator can't exceed the
        # output range and doesn't need saturating
        "fixed_point": True,
        "acc_type": "int64_t",
        "limits": None,
    },
]

# Templates and their output filename patterns
inc_templates = [
    ("complex.h.j2", "complex_{type}.h"),
    ("complex.c.j2", "complex_{type}.c"),
    ("waveform_gen.h.j2", "waveform_gen_{type}.h"),
    ("waveform_gen.c.j2", "waveform_gen_{type}.c"),
    ("spectrum_gen.h.j2", "spectrum_gen_{type}.h"),
    ("spectrum_gen.c.j2", "spectrum_gen_{type}.c"),
    ("nco.h.j2", "nco_{type}.h"),
    ("nco.c.j2", "nco_{type}.c"),
]
filter_templates = [
    ("fir.h.j2", "fir_{type}.h"),
    ("fir.c.j2", "fir_{type}.c"),
    ("iir.h.j2", "iir_{type}.h"),
    ("iir.c.j2", "iir_{type}.c"),
    ("json_to_filt.h.j2", "json_to_filt_{type}.h"),
    ("json_to_filt.c.j2", "json_to_filt_{type}.c"),
]
avg_templates = [
    ("avg.h.j2", "avg_{type}.h"),
    ("avg.c.j2", "avg_{type}.c"),
]
fft_templates = [
    ("fft.h.j2", "fft_{type}.h"),
    ("fft.c.j2", "fft_{type}.c"),
]
# The FFT is only generated for the floating point types
FFT_TYPES = ["float", "double"]
# The C++ wrapper traits, which map each type to its C structs and functions.
# Each header covers every type of its module, with the module's types (None
# for all of them)
cpp_traits_templates = [
    ("cpp_traits_average.h.j2", "cpp_traits_average.h", None),
    ("cpp_traits_complex.h.j2", "cpp_traits_complex.h", None),
    ("cpp_traits_fft.h.j2", "cpp_traits_fft.h", FFT_TYPES),
    ("cpp_traits_filter.h.j2", "cpp_traits_filter.h", None),
    ("cpp_traits_waveform.h.j2", "cpp_traits_waveform.h", None),
]


# Functions used by the filters to copy into, and shift, their history buffers.
# None selects a plain copy loop written into the generated code instead
STDLIB_COPY = {"memcpy": "memcpy", "memmove": "memmove", "includes": ["<string.h>"]}


def copy_options(args) -> dict:
    """Resolve the copy functions for the filter history buffers from the
    command line arguments, falling back to the standard library (or the
    built in copy loops with --no-stdlib) for any that aren't specified"""
    stdlib = not args.no_stdlib
    memcpy = args.memcpy or ("memcpy" if stdlib else None)
    memmove = args.memmove or ("memmove" if stdlib else None)
    includes = []
    if "memcpy" in (memcpy, memmove) or "memmove" in (memcpy, memmove):
        includes.append("<string.h>")
    if args.copy_header:
        header = args.copy_header
        includes.append(header if header.startswith("<") else f'"{header}"')
    return {"memcpy": memcpy, "memmove": memmove, "includes": includes}


# Math used to calculate the FFT twiddle factors at plan initialisation
TWIDDLE_MATH = {
    "double": {"math_type": "double", "math_suffix": ""},
    "float": {"math_type": "float", "math_suffix": "f"},
}


def to_float32(value: float) -> float:
    """Rounds a double to the nearest float"""
    return struct.unpack("f", struct.pack("f", value))[0]


def c_literal(value: float, single: bool) -> str:
    """Shortest C literal that reads back as exactly value, as a float when
    single is set"""
    if single:
        for digits in range(6, 10):
            text = f"{value:.{digits}g}"
            if to_float32(float(text)) == value:
                break
    else:
        text = repr(value)
    if "." not in text and "e" not in text:
        text += ".0"
    return text + ("f" if single else "")


def twiddle_table(max_n: int, single: bool) -> list:
    """The max_n point twiddle factors e^(-j 2 pi k / max_n) for k < max_n / 2,
    as C initialisers. They are calculated and reflected in the same way as
    fft_plan_init_*() with double twiddles, so the tables match bit for bit"""
    rnd = to_float32 if single else float
    half, quarter = max_n // 2, max_n // 4
    twiddle = []
    for k in range(half):
        if k <= max_n // 8:
            angle = 2 * math.pi * k / max_n
            twiddle.append((rnd(math.cos(angle)), rnd(-math.sin(angle))))
        elif k <= quarter:
            real, imag = twiddle[quarter - k]
            twiddle.append((-imag, -real))
        else:
            real, imag = twiddle[half - k]
            twiddle.append((-real, imag))
    return [
        f"{{{c_literal(real, single)}, {c_literal(imag, single)}}}"
        for real, imag in twiddle
    ]


def fft_options(
    twiddle_precision: str = "double", table_size: int = None, table_only: bool = False
) -> dict:
    """Per type settings for the FFT templates. twiddle_precision only applies
    to the float FFT, the double FFT always calculates its twiddles in double.
    table_size generates a const twiddle table for transforms up to that size,
    and table_only removes the twiddle calculation (and the maths library)"""
    options = {}
    for suffix, precision in (("float", twiddle_precision), ("double", "double")):
        table = None
        if table_size is not None:
            table = {
                "max_n": table_size,
                "entries": twiddle_table(table_size, suffix == "float"),
            }
        options[suffix] = {
            "twiddle": TWIDDLE_MATH[precision],
            "table": table,
            "table_only": table_only,
        }
    return options


# Floating point types the waveforms can be generated in
WAVE_MATH = {
    "float": {"type": "float", "math_suffix": "f", "literal_suffix": "f"},
    "double": {"type": "double", "math_suffix": "", "literal_suffix": ""},
}
# Default generation precision for the integer waveforms
INT_WAVE_DEFAULT = {"int32": "float", "int64": "double"}


# Range and default of the NCO sine table size, as log2 of the quarter wave
# entries
NCO_TABLE_BITS_RANGE = (4, 16)
NCO_TABLE_BITS_DEFAULT = 10


def nco_options(
    wave: dict, sine: str = "table", table_bits: int = None, int_math: str = "float"
) -> dict:
    """Per type settings for the NCO templates. sine is "table" for a quarter
    wave table of 2^table_bits entries with linear interpolation, or "libm"
    for sin/cos from the maths library. The table is in each type's waveform
    maths type, or for int_math "fixed" the integer types use a Q30 table and
    integer maths for every sample"""
    table_bits = table_bits or NCO_TABLE_BITS_DEFAULT
    size = 2**table_bits
    # Largest linear interpolation error of the table, (h / 2)^2 / 2 for a
    # step h in angle
    error = (math.pi / 2 / size) ** 2 / 8
    options = {}
    for suffix, math_type in wave.items():
        single = math_type["type"] == "float"
        fixed = int_math == "fixed" and suffix in INT_WAVE_DEFAULT
        option = {"sine": sine, "fixed": fixed}
        if fixed:
            values = [math.sin(math.pi / 2 * i / size) for i in range(size)] + [1.0]
            values.append(values[size - 1])
            # Bits of the position within a table step used for the
            # interpolation fraction, so the product with a table step fits
            # int64_t
            frac_bits = min(32, 62 - table_bits)
            option.update(
                table_bits=table_bits,
                table_size=size,
                table_entries=[str(math.floor(v * 2**30 + 0.5)) for v in values],
                table_error=f"{error:.0e}",
                frac_bits=frac_bits,
                frac_shift=62 - table_bits - frac_bits,
                frac_mask=f"UINT64_C(0x{2**frac_bits - 1:X})",
                frac_half=f"INT64_C({2 ** (frac_bits - 1)})",
            )
        elif sine == "table":
            values = [math.sin(math.pi / 2 * i / size) for i in range(size)] + [1.0]
            # Padding past the peak, only read with an interpolation fraction
            # of 0
            values.append(values[size - 1])
            if single:
                values = [to_float32(v) for v in values]
            # Bits of the position within a table step used for the
            # interpolation fraction, limited to what fits exactly in the type
            frac_bits = min(24 if single else 52, 62 - table_bits)
            option.update(
                table_bits=table_bits,
                table_size=size,
                table_entries=[c_literal(v, single) for v in values],
                table_error=f"{error:.0e}",
                frac_shift=62 - table_bits - frac_bits,
                frac_mask=f"UINT64_C(0x{2**frac_bits - 1:X})",
                frac_scale=c_literal(2.0**-frac_bits, single),
            )
        options[suffix] = option
    return options


def wave_options(int_precision: str = None) -> dict:
    """Per type floating point type that the waveforms are generated in. The
    floating point types use themselves, and the integer types use
    int_precision, or INT_WAVE_DEFAULT when it is None"""
    options = {"float": WAVE_MATH["float"], "double": WAVE_MATH["double"]}
    for suffix, default in INT_WAVE_DEFAULT.items():
        options[suffix] = WAVE_MATH[int_precision or default]
    return options


def generate(
    output_dir: str,
    templates: list,
    copy: dict = STDLIB_COPY,
    types: list = None,
    fft: dict = None,
    wave: dict = None,
    nco: dict = None,
) -> tuple:
    """Render the templates into output_dir for each type, returning the paths
    written and the number of templates that failed"""
    written = []
    failures = 0
    for config in type_configs(types):
        for template_name, output_pattern in templates:
            output_name = output_pattern.format(type=config["suffix"])
            ok = render(
                template_name,
                os.path.join(output_dir, output_name),
                type=config,
                copy=copy,
                fft=(fft or {}).get(config["suffix"]),
                wave=(wave or {}).get(config["suffix"]),
                nco=(nco or {}).get(config["suffix"]),
            )
            if ok:
                written.append(os.path.join(output_dir, output_name))
            else:
                failures += 1
    return written, failures


def generate_cpp_traits(
    output_dir: str, templates: list, fft: dict = None, wave: dict = None, nco: dict = None
) -> tuple:
    """Render the C++ traits templates into output_dir, each covering every
    type of its module, returning the paths written and the number of
    templates that failed. The options are per type, keyed by suffix"""
    written = []
    failures = 0
    for template_name, output_name, types in templates:
        output_path = os.path.join(output_dir, output_name)
        if render(
            template_name,
            output_path,
            types=type_configs(types),
            fft=fft or {},
            wave=wave or {},
            nco=nco or {},
        ):
            written.append(output_path)
        else:
            failures += 1
    return written, failures


def type_configs(types: list = None) -> list:
    """The configs of the types with their suffix in types (all of them for
    None), with the values derived from them added"""
    configs = []
    for config in types_config:
        if types is not None and config["suffix"] not in types:
            continue
        # Cast to widen the filter products into the accumulator, empty when
        # they are the same type
        acc_cast = (
            f"({config['acc_type']})" if config["acc_type"] != config["name"] else ""
        )
        configs.append({**config, "acc_cast": acc_cast})
    return configs


# Templates are in the 'templates' directory, relative to the project root the
# script is run from
env = Environment(loader=FileSystemLoader("templates"))


def render(template_name: str, output_path: str, **context) -> bool:
    """Render a template to output_path, returning False if it failed"""
    try:
        print(f"Generating {output_path}...")
        content = env.get_template(template_name).render(**context)
        # Unchanged files are left alone, so the build only recompiles what
        # changed
        if read_file(output_path) != content:
            with open(output_path, "w") as f:
                f.write(content)
        return True
    except Exception as e:
        print(f"Error generating {output_path}: {e}")
        return False


def read_file(path: str):
    """The contents of path, or None if it doesn't exist"""
    try:
        with open(path) as f:
            return f.read()
    except FileNotFoundError:
        return None


def write_manifest(path: str, sources: list):
    """Write the generated .c files as a makefile fragment, included by the
    top level Makefile so its source list always matches what was generated"""
    os.makedirs(os.path.dirname(path) or ".", exist_ok=True)
    with open(path, "w") as f:
        f.write("# Generated by scripts/generate_sources.py, do not edit\n")
        for source in sources:
            if source.endswith(".c"):
                f.write(f"GENERATED_SRC += {source}\n")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Generate the typed C sources from the jinja2 templates. "
        "Run from the root of the project."
    )
    copy_group = parser.add_argument_group(
        "filter copy functions",
        "The FIR and IIR filters copy new samples into, and shift, their "
        "history buffers. By default memcpy/memmove from the C standard "
        "library are used.",
    )
    copy_group.add_argument(
        "--no-stdlib",
        action="store_true",
        help="use plain copy loops instead of the C standard library, e.g. "
        "for MCUs without one",
    )
    copy_group.add_argument(
        "--memcpy",
        metavar="FUNC",
        help="function with the same signature as memcpy, used to copy new "
        "samples into the history buffers",
    )
    copy_group.add_argument(
        "--memmove",
        metavar="FUNC",
        help="function with the same signature as memmove, used to shift the "
        "history buffers (the regions overlap, with the destination first)",
    )
    copy_group.add_argument(
        "--copy-header",
        metavar="HEADER",
        help="header declaring the --memcpy/--memmove functions",
    )
    wave_group = parser.add_argument_group("waveform generation")
    wave_group.add_argument(
        "--int-waveform-precision",
        choices=["float", "double"],
        help="floating point type the int32/int64 waveforms are generated in, "
        "with only the output samples rounded to integers. The time buffer and "
        "wave properties use it too (default: float for int32, double for "
        "int64). double keeps int32 waveforms exact to the LSB at full scale, "
        "float is faster on targets with only a single precision FPU",
    )
    nco_group = parser.add_argument_group(
        "NCO",
        "The numerically controlled oscillators (nco_*) for streaming "
        "waveform generation",
    )
    nco_group.add_argument(
        "--nco-sine",
        choices=["table", "libm"],
        default="table",
        help="how the NCO calculates sine and cosine (default: %(default)s). "
        "table interpolates a const quarter wave table, without the maths "
        "library, e.g. for MCUs. libm calls sin/cos for every sample, for "
        "full precision",
    )
    nco_group.add_argument(
        "--nco-table-bits",
        type=int,
        metavar="BITS",
        help="size of the NCO sine table, 2^BITS entries for a quarter wave "
        f"(default: {NCO_TABLE_BITS_DEFAULT}, from {NCO_TABLE_BITS_RANGE[0]} "
        f"to {NCO_TABLE_BITS_RANGE[1]}). The interpolation error is about "
        "0.3 / 4^BITS of the amplitude, so 10 is about 3e-7, near float "
        "precision, and 16 about 7e-11",
    )
    nco_group.add_argument(
        "--nco-int-math",
        choices=["float", "fixed"],
        default="float",
        help="how the int32/int64 NCOs calculate each sample (default: "
        "%(default)s). float uses the waveform maths type, as for the other "
        "waveforms (see --int-waveform-precision). fixed uses integer maths "
        "only, from a Q30 sine table, for MCUs without an FPU. Needs --nco-sine "
        "table",
    )
    fft_group = parser.add_argument_group("FFT")
    fft_group.add_argument(
        "--fft-twiddle-precision",
        choices=["double", "float"],
        default="double",
        help="precision used to calculate the float FFT's twiddle factors at "
        "plan initialisation (default: %(default)s). float avoids double "
        "arithmetic entirely, for targets without a double FPU, at a small "
        "cost in accuracy. The double FFT always uses double",
    )
    fft_group.add_argument(
        "--fft-twiddle-table",
        metavar="MAX_N",
        type=int,
        help="generate a const table of twiddle factors for transforms up to "
        "MAX_N points (a power of two), with fft_plan_init_table_*() and "
        "rfft_plan_init_table_*() plans using it, so no workspace or plan "
        "initialisation maths is needed, e.g. for MCUs. The table takes "
        "MAX_N / 2 complex values of flash",
    )
    fft_group.add_argument(
        "--fft-table-only",
        action="store_true",
        help="with --fft-twiddle-table, leave out the workspace plans and "
        "dft_*(), so the FFT doesn't use the maths library",
    )
    parser.add_argument(
        "--manifest",
        metavar="PATH",
        help="also write the generated .c files to PATH as a makefile "
        "fragment, used by the top level Makefile",
    )
    args = parser.parse_args()
    if args.copy_header and not (args.memcpy or args.memmove):
        parser.error("--copy-header needs --memcpy and/or --memmove")
    table_size = args.fft_twiddle_table
    if table_size is not None and (table_size < 2 or table_size & (table_size - 1)):
        parser.error("--fft-twiddle-table needs a power of two of at least 2")
    if table_size is not None and table_size > 2**31:
        parser.error("--fft-twiddle-table can be at most 2^31")
    if args.fft_table_only and table_size is None:
        parser.error("--fft-table-only needs --fft-twiddle-table")
    if args.nco_int_math == "fixed" and args.nco_sine != "table":
        parser.error("--nco-int-math fixed needs --nco-sine table")
    if args.nco_table_bits is not None:
        if args.nco_sine != "table":
            parser.error("--nco-table-bits needs --nco-sine table")
        low, high = NCO_TABLE_BITS_RANGE
        if not low <= args.nco_table_bits <= high:
            parser.error(f"--nco-table-bits must be from {low} to {high}")

    copy = copy_options(args)
    print(
        "Filter copies: memcpy -> {}, memmove -> {}".format(
            copy["memcpy"] or "built in loop", copy["memmove"] or "built in loop"
        )
    )
    wave = wave_options(args.int_waveform_precision)
    print(
        "Integer waveforms generated in: int32 -> {}, int64 -> {}".format(
            wave["int32"]["type"], wave["int64"]["type"]
        )
    )
    nco = nco_options(wave, args.nco_sine, args.nco_table_bits, args.nco_int_math)
    if args.nco_sine == "table":
        print(
            "NCO sine: {} entry quarter wave table".format(nco["float"]["table_size"])
        )
    else:
        print("NCO sine: maths library")
    print(f"Integer NCO samples: {args.nco_int_math}")
    results = [
        generate("inc", inc_templates, wave=wave, nco=nco),
        generate("filter", filter_templates, copy),
        generate("average", avg_templates),
    ]
    if args.fft_table_only:
        print(f"FFT twiddles: table up to {table_size} points only")
    else:
        print(f"Float FFT twiddles: {args.fft_twiddle_precision}")
        if table_size is not None:
            print(f"FFT twiddle table: up to {table_size} points")
    fft = fft_options(args.fft_twiddle_precision, table_size, args.fft_table_only)
    results.append(generate("fft", fft_templates, types=FFT_TYPES, fft=fft))
    results.append(
        generate_cpp_traits(
            "cpp_bindings", cpp_traits_templates, fft=fft, wave=wave, nco=nco
        )
    )

    failures = sum(failed for _, failed in results)
    if failures:
        print(f"{failures} templates failed to generate")
        sys.exit(1)
    if args.manifest:
        write_manifest(args.manifest, [path for written, _ in results for path in written])
