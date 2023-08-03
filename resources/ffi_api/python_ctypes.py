# This is a Python implementation.
# Please reconsider using the bindings provided by the repo as this api is limited in comparison.
# That is, the python bindings allow for more features and flexibility

import ctypes
import os

# assuming api.dll is in the same directory as python script
libname = os.path.abspath(os.path.join(os.getcwd(), "api.dll")) # Windows
# libname = os.path.abspath(os.path.join(os.getcwd(), "api.so")) # Linux
# libname = os.path.abspath(os.path.join(os.getcwd(), "api.dylib")) # MacOS
c_lib = ctypes.CDLL(libname)

c_lib.api.restype = ctypes.c_int # the return value of the function is an int
c_lib.api.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p] # function takes 3 C-strings

# Lets see 3 ways of constructing and passing the strings

# Python 3
input_file = b"resources/text/commandline_sample_input.txt" # in this case the same as in the console example
# Python 2 doesn't need conversion
# input_file = "YOUR_INPUT_FILE_HERE"

# Python 3
resource_dir = bytes("./resources", 'utf-8')
# Python 2
# resource_dir = "./resources"

# Python 3
result = c_lib.api(input_file, resource_dir, b"holdem") # return the exit code, successfull or error
# Python 2
# result = c_lib.api(input_file, resource_dir, "holdem")

# Now verify that the "solve" file was generated correctly