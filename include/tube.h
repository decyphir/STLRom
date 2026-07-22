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

	Tube();
    Tube(Signal lower_s, Signal upper_s);

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
