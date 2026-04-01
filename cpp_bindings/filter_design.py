import sys

# Add the top level to path to make the wrapper class file accessible
sys.path.append("../")
from scripts.filter_design_wrapper import filter_wrapper, filter_e


if __name__ == "__main__":
    # Setup the filter constants
    f_s = 16000.0  # Sampling frequency
    f_pb = 2000.0  # Passband frequency
    f_sb = 4000.0  # Stopband frequency
    fir_taps = 32  # Sets the number of coefficients to design for in the FIR design

    # Output filename
    filename = "./fir"

    # Initialize the exporter
    designer = filter_wrapper(f_s=f_s)

    # Design the FIR
    designer.add_fir(
        name="fir",
        ftype=filter_e.LOWPASS,
        f_pb=f_pb,
        f_sb=f_sb,
        numtaps=fir_taps
    )

    # Write to a JSON file
    designer.export_to_json(filename)
