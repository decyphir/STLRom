#include "tube.h"
#include <fenv.h>

#pragma STDC FENV_ACCESS ON

namespace STLRom {

	Tube::Tube() {
		lower_signal = new Signal(0., -Signal::BigM, 1);
		upper_signal = new Signal(0., Signal::BigM, 1);
	}

	Tube::Tube(Signal& lower_s, Signal& upper_s) {
		lower_signal = new Signal(*lower_signal);
		upper_signal = new Signal(*upper_signal);
	} // TODO assert lower_s <= upper_s? // TODO assert same time domains?

	Tube::~Tube() {
		this->clear();
		delete lower_signal;
		delete upper_signal;
	}

	void Tube::clear() {
		upper_signal->clear();
		lower_signal->clear();
	}

	// add (resp. retract) radius r to the upper_signal (lower_signal)
	void Tube::inflate(double r) {
		Signal signal_r = Signal(0, r, 1);
		signal_r.appendConstantSample(Signal::BigM, r);
		fesetround(FE_DOWNWARD);
		*this->lower_signal = *this->lower_signal - signal_r;
		fesetround(FE_UPWARD);
		*this->upper_signal = *this->upper_signal + signal_r;
		fesetround(FE_TONEAREST);
	}

	Tube Tube::operator+(const Tube& that) const {
		fesetround(FE_DOWNWARD);
		Signal lower_s = *this->lower_signal + *that.lower_signal;
		fesetround(FE_UPWARD);
		Signal upper_s = *this->upper_signal + *that.upper_signal;
		fesetround(FE_TONEAREST);
		return Tube(lower_s, upper_s);
	}

	Tube Tube::operator-(const Tube& that) const {
		fesetround(FE_DOWNWARD);
		Signal lower_s = *this->lower_signal - *that.lower_signal;
		fesetround(FE_UPWARD);
		Signal upper_s = *this->upper_signal - *that.upper_signal;
		fesetround(FE_TONEAREST);
		return Tube(lower_s, upper_s);
	}

	Tube Tube::operator*(double p) const {
		Signal lower_s, upper_s;
		if (p >= 0) {
			fesetround(FE_DOWNWARD);
			lower_s = *this->lower_signal * p;
			fesetround(FE_UPWARD);
			upper_s = *this->upper_signal * p;
		} else {
			fesetround(FE_DOWNWARD);
			lower_s = *this->upper_signal * p;
			fesetround(FE_UPWARD);
			upper_s = *this->lower_signal * p;
		}
		fesetround(FE_TONEAREST);
		return Tube(lower_s, upper_s);
	}


	Tube Tube::operator/(double p) const {
		return *this * (1/p); // TODO p != 0
	}

	/*
	* friend functions
	*/

	std::ostream & operator<<(std::ostream & out, const Tube & T) {
		return out << "lower_signal: " << std::endl << *T.lower_signal << std::endl << \
		"upper_signal: " << std::endl << *T.lower_signal << std::endl;
	}

} // namespace STLRom
