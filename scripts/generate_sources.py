import os
from jinja2 import Environment, FileSystemLoader

# Configuration for different types
types_config = [
    {
        "name": "float",
        "suffix": "float",
        "math_suffix": "f",
        "math_prefix": "(float)",
    },
    {
        "name": "double",
        "suffix": "double",
        "math_suffix": "",
        "math_prefix": "(double)",
    },
    {
        "name": "int32_t",
        "suffix": "int32",
        "math_suffix": "f",  # Use float math for generation
        "math_prefix": "(int32_t)",  # Cast result back to int32_t
    },
    {
        "name": "int64_t",
        "suffix": "int64",
        "math_suffix": "",  # Use double math for generation
        "math_prefix": "(int64_t)",  # Cast result back to int64_t
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
    ("constants.h.j2", "constants_{type}.h"),
    ("constants.c.j2", "constants_{type}.c"),
]
filter_templates = [
    ("fir.h.j2", "fir_{type}.h"),
    ("fir.c.j2", "fir_{type}.c"),
    ("iir.h.j2", "iir_{type}.h"),
    ("iir.c.j2", "iir_{type}.c"),
    ("json_to_filt.h.j2", "json_to_filt_{type}.h"),
    ("json_to_filt.c.j2", "json_to_filt_{type}.c"),
]


def generate(output_dir: str, templates: list):
    # Templates are in the 'templates' directory
    env = Environment(loader=FileSystemLoader("templates"))

    # Ensure we are in the project root (assuming script is run from there)
    # or handle paths relative to this script

    for config in types_config:
        for template_name, output_pattern in templates:
            try:
                template = env.get_template(template_name)
                output_name = output_pattern.format(type=config["suffix"])
                output_path = os.path.join(output_dir, output_name)

                print(f"Generating {output_path}...")
                content = template.render(type=config)
                with open(output_path, "w") as f:
                    f.write(content)
            except Exception as e:
                print(f"Error generating {output_name}: {e}")


if __name__ == "__main__":
    generate("inc", inc_templates)
    generate("filter", filter_templates)
