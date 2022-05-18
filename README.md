# RobonTL 

RobonTL is a C++ library for ROBust ONline monitoring of signal Temporal Logic. It computes interval robustness as defined in 'Robust online monitoring of signal temporal logic' by Deshmuk et al, although the algorithm is not exactly the one described in this paper. 


## Compilation

RobonTL is written in C++ with no special requirement. A makefile is provided that compiles every source file in `src` folder, and creates a simple static library file called `robonlib.a`. 
```
make all
```

## Testing

A simple program is given in the `test` folder. To compile: 

```
g++ test_prog.cpp ../lib/robonlib.a -I ../include -o test_prog
```

It reads a formula specification file `spec.stl` in the same folder and reads signal samples input on stdin, computing interval robustness at each sample. 



# pyRobonTL 

pyRobonTL is a python library for Robust online monitoring of temporal logic properties.

## Compilation

RobonTL is written in C++ with no special requirement. A makefile is provided that compiles every source file in `src` folder, and creates a simple static library file called `robonlib.a`. 
```
make all
```

## Testing

A simple program is given in the `test` folder. To compile: 

```
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) ../lib/robonlib.a -I ../include pyrobonTL.cpp -o pyrobonTL$(python3-config --extension-suffix)
```
A python script in given in the same folder, do

```
python test_prog.py

```
