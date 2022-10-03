# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Compilation

STLRom is written in C++ with Python bindings using pybind11. Python 3.10 or higher is recommended. To compile, run  
```
cd build
cmake ..
make
```

## Testing the C++ Library

In the test folder, `test_driver.cpp` is an example of using the C++ library. Go into build/test and run it with 
```
./test_driver
```

## Testing the python bindings

In the build folder, execute
```
make test
```
It will run the unit tests  python programs from the `test` folder.
