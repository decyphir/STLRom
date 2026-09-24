#include "tube.h"
#include <fenv.h>

#pragma STDC FENV_ACCESS ON

namespace STLRom {
	void Tube::clear() {
		upper_signal = Signal();
		lower_signal = Signal();
	}

	void Tube::appendSample(double t, interval itv) {
		appendSample(t, itv, interval(0.), false);
	}
	void Tube::appendSample(double t, interval itv, interval d) {
		appendSample(t, itv, d, false);
	}
    void Tube::appendSample(double t, interval itv, interval d, bool interp) {

        if ((t <= get_endTime()) && get_size() > 0)
        {            
            return;
        }

        if (get_size() == 0)
        {
			lower_signal.push_back(Sample(t, itv.begin, d.begin));
			upper_signal.push_back(Sample(t, itv.end, d.end));
            set_beginTime(t);
            set_endTime(t);
        }
        else
        {
            if (interp) 
            {   // we discard the derivative of last sample and make it so it 
                // interpolates linearly with the new one   
                lower_signal.back().derivative = (itv.begin - lower_signal.back().value) / (t - lower_signal.back().time);
                upper_signal.back().derivative = (itv.end - upper_signal.back().value) / (t - upper_signal.back().time);
            }
            lower_signal.push_back(Sample(t, itv.begin, d.begin));
            upper_signal.push_back(Sample(t, itv.end, d.end));
            set_endTime(t);
        }
	}
	void Tube::appendConstantSample(double t, interval itv) {
		appendSample(t, itv, interval(0.), false);
	}
	void Tube::appendLinearSample(double t, interval itv) {
		appendSample(t, itv, interval(0.), false);
	}
	void Tube::appendLinearSample(double t, interval itv, interval d) {
		appendSample(t, itv, d, false);
	}

    int Tube::get_size() const {
		return lower_signal.size();
	}

    double Tube::get_beginTime() const {
		return lower_signal.beginTime;
	}

    double Tube::get_endTime() const {
		return lower_signal.endTime;
	}

    void Tube::set_beginTime(double begin) {
		lower_signal.beginTime = begin;
		upper_signal.beginTime = begin;
	}

    void Tube::set_endTime(double end) {
		lower_signal.endTime = end;
		upper_signal.endTime = end;
	}
	

	// add (resp. retract) radius r to the upper_signal (lower_signal)
	void Tube::inflate(double r) {
		Signal signal_r = Signal(0, r, 1);
		signal_r.appendConstantSample(Signal::BigM, r);
		fesetround(FE_DOWNWARD);
		this->lower_signal = this->lower_signal - signal_r;
		fesetround(FE_UPWARD);
		this->upper_signal = this->upper_signal + signal_r;
		fesetround(FE_TONEAREST);
	}

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

	/*
	* friend functions
	*/

	std::ostream & operator<<(std::ostream & out, const Tube & T) {
		return out << "lower_signal: " << std::endl << T.lower_signal << std::endl << \
		"upper_signal: " << std::endl << T.upper_signal << std::endl;
	}

} // namespace STLRom
