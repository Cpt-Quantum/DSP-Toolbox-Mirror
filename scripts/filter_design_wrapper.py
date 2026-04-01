import json
import os
import scipy.signal as signal
from enum import Enum
from typing import Union, List

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
    def add_fir(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                f_sb: Union[float, List[float]], numtaps: int, **kwargs):
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
        
        self.filter_data[name] = {
            "type": ftype.value,
            "b": fir_b.tolist()
        }

    def add_iir_ba(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                   f_sb: Union[float, List[float]], gpass: float, gstop: float, **kwargs):
        """Designs an IIR filter (numerator/denominator format) and adds it to the export list."""
        kwargs['fs'] = self.f_s
        kwargs['output'] = "ba"
        
        b, a = signal.iirdesign(wp=f_pb, ws=f_sb, gpass=gpass, gstop=gstop, **kwargs)
        
        self.filter_data[name] = {
            "type": ftype.value,
            "b": b.tolist(),
            "a": a.tolist()
        }

    def add_iir_sos(self, name: str, ftype: filter_e, f_pb: Union[float, List[float]],
                    f_sb: Union[float, List[float]], gpass: float, gstop: float, gain: float = 1.0, **kwargs):
        """Designs an IIR filter (Second-Order Sections format) and adds it to the export list."""
        kwargs['fs'] = self.f_s
        kwargs['output'] = "sos"
        
        sos = signal.iirdesign(wp=f_pb, ws=f_sb, gpass=gpass, gstop=gstop, **kwargs)
        
        self.filter_data[name] = {
            "type": ftype.value,
            "sections": sos.tolist(),
            "gain": gain
        }

    def export_to_json(self, filepath: str):
        """Write the accumulated filter data to a JSON file."""
        if not filepath.endswith(".json"):
            filepath += ".json"
            
        os.makedirs(os.path.dirname(filepath), exist_ok=True)
        
        with open(filepath, "w") as f:
            json.dump(self.filter_data, f, indent=4)

        print(f"Filter coefficients exported to {filepath}")
