import json
import os
import numpy as np
import scipy.signal as signal
from enum import Enum
from typing import Union, List, Tuple

class filter_e(Enum):
    LOWPASS = "lowpass"
    HIGHPASS = "highpass"
    BANDPASS = "bandpass"
    BANDSTOP = "bandstop"

class filter_wrapper():
    def __init__(self, f_s):
        self.f_s = f_s
        # The file export will be done from a dictionary, add the sample rate to it for now
        self.filter_data = {
            "f_s": self.f_s
        }
    def _fixed_point_converter(self, coeffs: Union[list, np.ndarray], fixed_point_format: Tuple[int, int]) -> list:
        """
        Converts floating point coefficients into fixed point format.
        The fixed_point_format is a tuple of (total_bits, fractional_bits) for example (18, 17).
        The integer filters treat each coefficient c as c / 2**fractional_bits, so
        fractional_bits is exported alongside the coefficients as "frac_bits".
        Raises a ValueError if a coefficient doesn't fit, rather than clipping it
        and changing the filter response
        """
        total_bits, fractional_bits = fixed_point_format
        if not 0 <= fractional_bits < total_bits:
            raise ValueError(f"fractional_bits ({fractional_bits}) must be from 0 to total_bits - 1 ({total_bits - 1})")
        coeffs_np = np.array(coeffs, dtype=float)
        # Scale and round to nearest integer
        scale_factor = 2**fractional_bits
        fixed_coeffs = np.round(coeffs_np * scale_factor).astype(np.int64)
        # The coefficients must be within the 2's complement range
        max_val = 2**(total_bits-1) - 1
        min_val = -2**(total_bits-1)
        if np.any(fixed_coeffs > max_val) or np.any(fixed_coeffs < min_val):
            int_bits = total_bits - 1 - fractional_bits
            raise ValueError(
                f"Coefficients up to {np.max(np.abs(coeffs_np)):.6g} don't fit in Q{int_bits}.{fractional_bits} "
                f"(range {min_val / scale_factor:.6g} to {max_val / scale_factor:.6g}), use fewer fractional bits")
        # Return the converted list
        return fixed_coeffs.tolist()

    def add_fir(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                f_sb: Union[float, List[float]], numtaps: int,
                fixed_point_format: Tuple[int, int] = None, **kwargs):
        """Designs an FIR filter based on the FilterType and adds it to the export list."""
        nyq = self.f_s / 2.0
        
        # Formulate the Remez bands and desired gain arrays based on the Enum
        if ftype == filter_e.LOWPASS:
            bands = [0, f_pb, f_sb, nyq]
            desired = [1, 0]
        elif ftype == filter_e.HIGHPASS:
            bands = [0, f_sb, f_pb, nyq]
            desired = [0, 1]
        elif ftype == filter_e.BANDPASS:
            # For bandpass, f_pb and f_sb must be iterables of length 2
            bands = [0, f_sb[0], f_pb[0], f_pb[1], f_sb[1], nyq]
            desired = [0, 1, 0]
        elif ftype == filter_e.BANDSTOP:
            # For bandstop, f_pb and f_sb must be iterables of length 2
            bands = [0, f_pb[0], f_sb[0], f_sb[1], f_pb[1], nyq]
            desired = [1, 0, 1]
        else:
            raise ValueError(f"Unsupported filter type: {ftype}")

        kwargs['fs'] = self.f_s
        fir_b = signal.remez(numtaps, bands, desired, **kwargs)

        if fixed_point_format:
            fir_b_out = self._fixed_point_converter(fir_b, fixed_point_format)
        else:
            fir_b_out = fir_b.tolist()
        
        self.filter_data[name] = {
            "type": ftype.value,
            "b": fir_b_out
        }
        if fixed_point_format:
            self.filter_data[name]["frac_bits"] = fixed_point_format[1]

    def add_iir_ba(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                   f_sb: Union[float, List[float]], gpass: float, gstop: float,
                   fixed_point_format: Tuple[int, int] = None, **kwargs):
        """Designs an IIR filter (numerator/denominator format) and adds it to the export list."""
        kwargs['fs'] = self.f_s
        kwargs['output'] = "ba"
        
        b, a = signal.iirdesign(wp=f_pb, ws=f_sb, gpass=gpass, gstop=gstop, **kwargs)

        if fixed_point_format:
            b_out = self._fixed_point_converter(b, fixed_point_format)
            a_out = self._fixed_point_converter(a, fixed_point_format)
        else:
            b_out = b.tolist()
            a_out = a.tolist()
        
        self.filter_data[name] = {
            "type": ftype.value,
            "b": b_out,
            "a": a_out
        }
        if fixed_point_format:
            self.filter_data[name]["frac_bits"] = fixed_point_format[1]

    def add_iir_sos(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                    f_sb: Union[float, List[float]], gpass: float, gstop: float, gain: float = 1.0,
                    fixed_point_format: Tuple[int, int] = None, **kwargs):
        """Designs an IIR filter (Second-Order Sections format) and adds it to the export list."""
        kwargs['fs'] = self.f_s
        kwargs['output'] = "sos"
        
        sos = signal.iirdesign(wp=f_pb, ws=f_sb, gpass=gpass, gstop=gstop, **kwargs)

        if fixed_point_format:
            sos_out = self._fixed_point_converter(sos, fixed_point_format)
            gain_out = self._fixed_point_converter(gain, fixed_point_format)
        else:
            sos_out = sos.tolist()
            gain_out = gain

        self.filter_data[name] = {
            "type": ftype.value,
            "sections": sos_out,
            "gain": gain_out
        }
        if fixed_point_format:
            self.filter_data[name]["frac_bits"] = fixed_point_format[1]

    def export_to_json(self, filepath: str):
        """Write the accumulated filter data to a JSON file."""
        if not filepath.endswith(".json"):
            filepath += ".json"
            
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        with open(filepath, "w") as f:
            json.dump(self.filter_data, f, indent=4)

        print(f"Filter coefficients exported to {filepath}")
