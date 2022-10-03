# STLRom 

STLRom is a C++ library with python bindings for Robust online monitoring of Signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 

## Compilation

STLRom is written in C++ with no special dependencies. It can be built with Cmake:

```
make all
mkdir build
cd build
cmake ..
make
```

## Testing the C++ Library

Go into build/test and run 
```
./test_driver
```

## Testing the python bindings

In the build folder, execute
```
make test
```
It will run the unit tests  python programs from the `test` folder.

