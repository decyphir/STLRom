# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Compilation

STLRom is written in C++ with Python bindings using pybind11. Python 3.10 or higher is recommended. To compile, run  
```
cd build
cmake ..
make
```

## Testing 
In the test folder, `test_driver.cpp` is an example of using the C++ library, while `test_prog*.py` are small test programs demonstrating the library usage.
``` 
make test
```
in the build folder will compile and run these programs.

More documentation coming soon. 
