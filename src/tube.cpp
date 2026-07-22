#include "tube.h"
#include <fenv.h>

#pragma STDC FENV_ACCESS ON

using namespace std;

namespace STLRom {

Tube::Tube()
    : lower_signal(0., -Signal::BigM, 0.),
      upper_signal(0., Signal::BigM, 0.) {}

Tube::Tube(Signal lower_s, Signal upper_s)
    : lower_signal(lower_s), upper_signal(upper_s) {
} // TODO assert lower_s <= upper_s? // TODO assert same time domains?

Tube Tube::operator+(const Tube& that) const {
	fesetround(FE_DOWNWARD);
	Signal lower_s = this->lower_signal + that.lower_signal;
	fesetround(FE_UPWARD);
	Signal upper_s = this->upper_signal + that.upper_signal;
	fesetround(FE_TONEAREST);
	return Tube(lower_s, upper_s);
}

Tube Tube::operator-(const Tube& that) const {
	fesetround(FE_DOWNWARD);
	Signal lower_s = this->lower_signal - that.lower_signal;
	fesetround(FE_UPWARD);
	Signal upper_s = this->upper_signal - that.upper_signal;
	fesetround(FE_TONEAREST);
	return Tube(lower_s, upper_s);
}

Tube Tube::operator*(double p) const {
	Signal lower_s, upper_s;
	if (p >= 0) {
		fesetround(FE_DOWNWARD);
		lower_s = this->lower_signal * p;
		fesetround(FE_UPWARD);
		upper_s = this->upper_signal * p;
	} else {
		fesetround(FE_DOWNWARD);
		lower_s = this->upper_signal * p;
		fesetround(FE_UPWARD);
		upper_s = this->lower_signal * p;
	}
	fesetround(FE_TONEAREST);
	return Tube(lower_s, upper_s);
}


Tube Tube::operator/(double p) const {
	return *this * (1/p); // TODO p != 0
}

} // namespace STLRom
