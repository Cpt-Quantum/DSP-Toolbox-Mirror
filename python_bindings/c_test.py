import ctypes

# Create the wrapper for the C functions
c_lib = ctypes.cdll.LoadLibrary("./clib.so")

WAVE_LENGTH = 100
SAMPLE_RATE = 250

# Create the time and data arrays
t_arr = c_lib.create_c_float_arr(WAVE_LENGTH)
x_arr = c_lib.create_c_float_arr(WAVE_LENGTH)

# Generate the time based on the sample rate
c_lib.gen_time(t_arr, WAVE_LENGTH, SAMPLE_RATE)

# Call free against any created arrays
c_lib.free_c_arr(t_arr)
c_lib.free_c_arr(x_arr)


