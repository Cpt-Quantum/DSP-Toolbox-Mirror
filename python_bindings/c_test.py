import ctypes

# Create the wrapper for the C functions
c_lib = ctypes.cdll.LoadLibrary("./clib.so")

WAVE_LENGTH = 100
SAMPLE_RATE = 250

# Setup the return types for the malloc functions as void pointers
c_lib.create_c_float_arr.restype = ctypes.c_void_p
c_lib.create_c_float_arr.argtypes = [ctypes.c_uint]
c_lib.create_c_wave_properties_arr.restype = ctypes.c_void_p
c_lib.create_c_wave_properties_arr.argtypes = [ctypes.c_uint]

# Setup for gen_time_float
c_lib.gen_time_float.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
c_lib.gen_time_float.restype = None

# Setup for free_c_arr
c_lib.free_c_arr.argtypes = [ctypes.c_void_p]
c_lib.free_c_arr.restype = None

# Create the time and data arrays
print("Creating arrays...")
t_arr = c_lib.create_c_float_arr(WAVE_LENGTH)
x_arr = c_lib.create_c_float_arr(WAVE_LENGTH)

if t_arr is None or x_arr is None:
    print("Failed to allocate memory")
    exit(1)

# Generate the time based on the sample rate
print("Calling gen_time_float...")
c_lib.gen_time_float(t_arr, WAVE_LENGTH, SAMPLE_RATE)
print("gen_time_float called successfully")

# Call free against any created arrays
print("Freeing t_arr...")
c_lib.free_c_arr(t_arr)
print("Freeing x_arr...")
c_lib.free_c_arr(x_arr)
print("Success!")
