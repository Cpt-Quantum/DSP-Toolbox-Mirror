import sys

# Add the top level to path to make the wrapper class file accessible
sys.path.append("../")
from scripts.filter_design_wrapper import filter_wrapper, filter_e

if __name__ == "__main__":
    # Setup the filter constants
    f_s = 16000.0  # Sampling frequency
    f_pb = 2000.0  # Passband frequency
    f_sb = 4000.0  # Stopband frequency
    filt_gain = 40  # Sets the filter amplitude reduction at the stopband (used for IIR)
    fir_taps = 100  # Sets the number of coefficients to design for in the FIR design

    # Output filename
    filename = "results/filters"

    # Initialize the exporter
    designer = filter_wrapper(f_s=f_s)

    # Each filter is designed twice, as floating point coefficients for the
    # float filters and as Q format coefficients for the int32 filters. The
    # fixed point format is (total bits, fractional bits), with the
    # fractional bits exported to the JSON alongside the coefficients
    formats = {
        "": None,
        "_int32": {
            # The FIR coefficients are all below 1, so Q0.31
            "fir": (32, 31),
            # The IIR coefficients reach about 3.2 (and a[0] must be 1.0), so
            # Q2.29
            "iir": (32, 29),
        },
    }
    for suffix, fmt in formats.items():
        # Design the base IIR (a, b)
        designer.add_iir_ba(
            name="iir_base" + suffix,
            ftype=filter_e.LOWPASS,
            f_pb=f_pb,
            f_sb=f_sb,
            gpass=1,
            gstop=filt_gain,
            fixed_point_format=fmt and fmt["iir"],
        )

        # Design the SoS IIR
        designer.add_iir_sos(
            name="iir_sos" + suffix,
            ftype=filter_e.LOWPASS,
            f_pb=f_pb,
            f_sb=f_sb,
            gpass=1,
            gstop=filt_gain,
            gain=1.0,
            fixed_point_format=fmt and fmt["iir"],
        )

        # Design the FIR
        designer.add_fir(
            name="fir" + suffix,
            ftype=filter_e.LOWPASS,
            f_pb=f_pb,
            f_sb=f_sb,
            numtaps=fir_taps,
            fixed_point_format=fmt and fmt["fir"],
        )

    # Write to a JSON file
    designer.export_to_json(filename)
