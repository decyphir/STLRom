#ifndef __TUBE_H
#define __TUBE_H

#include <iostream>
#include "signal.h"

using namespace std;

namespace STLRom {

class Tube {
public:
    Signal lower_signal;
    Signal upper_signal;

	explicit Tube() : lower_signal(Signal(0., -Signal::BigM, 1)), upper_signal(Signal(0., Signal::BigM, 1)) {}
	explicit Tube(Signal& lower_s, Signal& upper_s) : lower_signal(std::move(lower_s)), upper_signal(std::move(upper_s)) {} // TODO assert lower_s <= upper_s? // TODO assert same time domains?
    ~Tube() = default;
    
    void clear();

    void inflate(double r);

    // arithmetics operations?
    
	Tube operator+(const Tube& that) const;
	Tube operator-(const Tube& that) const;
	Tube operator*(double) const;
	Tube operator/(double) const;

    // robustness functions?


    friend std::ostream& operator<<(std::ostream&, const Tube&);
}
;


}

#endif
