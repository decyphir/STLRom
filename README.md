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

TODO
It reads a formula specification file `spec.stl` in the same folder and reads signal samples input on stdin, computing interval robustness at each sample. 

## Testing the python bindings

In the build folder, execute
```
python3

import STLrom
STLrom.read_point()
```
