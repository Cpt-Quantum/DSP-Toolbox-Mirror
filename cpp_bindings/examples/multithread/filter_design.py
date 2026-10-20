import sys
import numpy as np
import scipy.signal as signal

# Add the top level to path to make the wrapper class file accessible
sys.path.append("../../../")
from scripts.filter_design_wrapper import filter_wrapper, filter_e


def ripple_to_deviation(gpass, gstop):
    """Converts passband ripple and stopband attenuation in dB into the linear
    deviations used by an equiripple FIR design."""
    delta_p = (10 ** (gpass / 20) - 1) / (10 ** (gpass / 20) + 1)
    delta_s = 10 ** (-gstop / 20)
    return delta_p, delta_s


def measure_response(b, a, f_s, f_pb, f_sb):
    """Returns the passband ripple and stopband attenuation (both in dB) of a
    lowpass filter."""
    w, h = signal.freqz(b, a, worN=8192, fs=f_s)
    mag_db = 20 * np.log10(np.maximum(np.abs(h), 1e-12))
    passband = mag_db[w <= f_pb]
    stopband = mag_db[w >= f_sb]
    return passband.max() - passband.min(), -stopband.max()


if __name__ == "__main__":
    # Setup the filter constants, the FIR and IIR are both designed to meet
    # this lowpass specification
    f_s = 16000.0  # Sampling frequency
    f_pb = 2000.0  # Passband frequency
    f_sb = 3000.0  # Stopband frequency
    gpass = 0.5  # Maximum passband ripple (dB)
    gstop = 60.0  # Minimum stopband attenuation (dB)

    # Output filename
    filename = "./filters"

    # Initialize the exporter
    designer = filter_wrapper(f_s=f_s)

    # Find the smallest equiripple FIR that meets the specification, weighting
    # the stopband so both bands hit their deviation limits together
    delta_p, delta_s = ripple_to_deviation(gpass, gstop)
    weight = [1, delta_p / delta_s]
    for fir_taps in range(8, 1024):
        b = signal.remez(fir_taps, [0, f_pb, f_sb, f_s / 2], [1, 0],
                         weight=weight, fs=f_s)
        ripple, atten = measure_response(b, 1, f_s, f_pb, f_sb)
        if ripple <= gpass and atten >= gstop:
            break
    designer.add_fir(
        name="fir",
        ftype=filter_e.LOWPASS,
        f_pb=f_pb,
        f_sb=f_sb,
        numtaps=fir_taps,
        weight=weight
    )
    print(f"FIR: {fir_taps} taps, {ripple:.3f} dB ripple, {atten:.1f} dB attenuation")

    # Design the IIR (elliptic) to the same specification, both as second order
    # sections and as a single direct form polynomial
    designer.add_iir_sos(
        name="iir_sos",
        ftype=filter_e.LOWPASS,
        f_pb=f_pb,
        f_sb=f_sb,
        gpass=gpass,
        gstop=gstop
    )
    sos = np.array(designer.filter_data["iir_sos"]["sections"])
    b, a = signal.sos2tf(sos)
    ripple, atten = measure_response(b, a, f_s, f_pb, f_sb)
    print(f"IIR (SOS): {len(sos)} sections, {ripple:.3f} dB ripple, {atten:.1f} dB attenuation")

    designer.add_iir_ba(
        name="iir_ba",
        ftype=filter_e.LOWPASS,
        f_pb=f_pb,
        f_sb=f_sb,
        gpass=gpass,
        gstop=gstop
    )
    # Direct form is sensitive to coefficient rounding, so check the poles and
    # response once the coefficients are rounded to float32 as the C code uses
    b = np.array(designer.filter_data["iir_ba"]["b"], dtype=np.float32).astype(float)
    a = np.array(designer.filter_data["iir_ba"]["a"], dtype=np.float32).astype(float)
    pole_radius = np.abs(np.roots(a)).max()
    ripple, atten = measure_response(b, a, f_s, f_pb, f_sb)
    print(f"IIR (direct form): order {len(a) - 1}, {ripple:.3f} dB ripple, "
          f"{atten:.1f} dB attenuation, max float32 pole radius {pole_radius:.4f}")
    if pole_radius >= 1:
        raise SystemExit("Direct form IIR is unstable with float32 coefficients")

    # Write to a JSON file
    designer.export_to_json(filename)
