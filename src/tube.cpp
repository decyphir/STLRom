#include "tube.h"
#include <fenv.h>

#pragma STDC FENV_ACCESS ON

using namespace std;

namespace STLRom {

Tube::Tube()
    : signal(), lower_signal(0., -Signal::BigM, 0.),
      upper_signal(0., Signal::BigM, 0.) {}

Tube::Tube(Signal s)
    : signal(s), lower_signal(0., -Signal::BigM, 0.),
      upper_signal(0., Signal::BigM, 0.) {}

Tube::Tube(Signal s, Signal lower_s, Signal upper_s)
    : signal(s), lower_signal(lower_s), upper_signal(upper_s) {
} // TODO assert lower_s <= s <= upper_s? // TODO assert same time domains?

Tube Tube::operator+(const Tube& that) const {
	Signal s = this->signal + that.signal;
	fesetround(FE_DOWNWARD);
	Signal lower_s = this->lower_signal + that.lower_signal;
	fesetround(FE_UPWARD);
	Signal upper_s = this->upper_signal + that.upper_signal;
	fesetround(FE_TONEAREST);
	return Tube(s, lower_s, upper_s);
}

Tube Tube::operator-(const Tube& that) const {
	Signal s = this->signal - that.signal;
	fesetround(FE_DOWNWARD);
	Signal lower_s = this->lower_signal - that.lower_signal;
	fesetround(FE_UPWARD);
	Signal upper_s = this->upper_signal - that.upper_signal;
	fesetround(FE_TONEAREST);
	return Tube(s, lower_s, upper_s);
}

Tube Tube::operator*(double p) const {
	Signal s = this->signal * p;
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
	return Tube(s, lower_s, upper_s);
}


Tube Tube::operator/(double p) const {
	return *this * (1/p); // TODO p != 0
}

} // namespace STLRom
