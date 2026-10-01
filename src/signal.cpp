#include "signal.h"
#include "iomanip"
#include "tools.h"
#include <cfenv>
#include <vector>
#include <sstream>
#include <fenv.h>

#pragma STDC FENV_ACCESS ON


namespace STLRom {

    // Default semantics
    Semantics Signal::semantics = Semantics::SPACE;

    double Signal::BigM = 10000.;
    double Signal::Eps = 1e-10;

    /* 
     * class Signal member functions
     */
    Signal::Signal(double T, double V, int n) {
        beginTime=T;
        endTime=T;
        push_back(Sample(T, bigM_clamping(V), 0.));
        lower_signal.push_back(Sample(T, -BigM, 0.));
        upper_signal.push_back(Sample(T,  BigM, 0.));
    }

    Signal::Signal(double * T, double * V, int n) {
#ifdef DEBUG__
        printf(">> Signal::Signal:                              IN." );
#endif

        beginTime=T[0];
        endTime = T[n - 1];

        if (n == 1)
            push_back(Sample(T[0], bigM_clamping(V[0]), 0.));
        else
        {
            for (int i = 0; i < n - 1; i++)
            {
                push_back(Sample(T[i], bigM_clamping(V[i]), (V[i + 1] - V[i]) / (T[i + 1] - T[i])));
            }
            push_back(Sample(T[n - 1], bigM_clamping(V[n - 1]), 0.));
        }
        lower_signal.push_back(Sample(T[0], -BigM, 0.));
        upper_signal.push_back(Sample(T[0],  BigM, 0.));

#ifdef DEBUG__
        printf("<< Signal::Signal:                            OUT." );
#endif
    }

    Signal::Signal(double * T, double * V, double * D, int n) {
        
        beginTime=T[0];
        endTime = T[n - 1];

        if (n == 1)
            push_back(Sample(T[0], bigM_clamping(V[0]), D[0]));
        else
        {
            for (int i = 0; i < n; i++)
            {
                push_back(Sample(T[i], bigM_clamping(V[i]), D[i]));
            }
        }
        lower_signal.push_back(Sample(T[0], -BigM, 0.));
        upper_signal.push_back(Sample(T[0],  BigM, 0.));
    }

	Signal::Signal(double T, double V, interval itv, int n) {
        beginTime=T;
        endTime=T;
        push_back(Sample(T, bigM_clamping(V), 0.));
        lower_signal.push_back(Sample(T, bigM_clamping(itv.begin), 0.));
        upper_signal.push_back(Sample(T, bigM_clamping(itv.end), 0.));
    }
	Signal::Signal(double * T, double * V, interval * itv, int n) {
        beginTime=T[0];
        endTime = T[n - 1];

        if (n == 1) {
            push_back(Sample(T[0], bigM_clamping(V[0]), 0.));
            lower_signal.push_back(Sample(T[0], bigM_clamping(itv[0].begin), 0.));
            upper_signal.push_back(Sample(T[0], bigM_clamping(itv[0].end), 0.));
        } else {
            for (int i = 0; i < n - 1; i++) { // TODO lower and upper derivatives more guaranteed?
                double dt = (T[i + 1] - T[i]);
                push_back(Sample(T[i], bigM_clamping(V[i]), (V[i + 1] - V[i]) / dt));
                lower_signal.push_back(Sample(T[i], bigM_clamping(itv[i].begin), (itv[i + 1].begin - itv[i].begin) / dt));
                upper_signal.push_back(Sample(T[i], bigM_clamping(itv[i].end), (itv[i + 1].end - itv[i].end) / dt));
            }
            push_back(Sample(T[n - 1], bigM_clamping(V[n - 1]), 0.));
            lower_signal.push_back(Sample(T[n - 1], bigM_clamping(itv[n - 1].begin), 0.));
            upper_signal.push_back(Sample(T[n - 1], bigM_clamping(itv[n - 1].end), 0.));
        }
        lower_signal.push_back(Sample(T[0], -BigM, 0.));
        upper_signal.push_back(Sample(T[0],  BigM, 0.));
    }

	Signal::Signal(double T, interval itv, int n) : Signal(T, itv.mid(), itv, n) {}

	Signal::Signal(const std::deque<Sample> &samples) {
        int n = samples.size();

        if (n == 0) {
            beginTime = 0.;
            endTime = 0.;
            return;
        }
        
        beginTime=samples[0].time;
        endTime = samples[n - 1].time;

        if (n == 1)
            push_back(Sample(samples[0].time, bigM_clamping(samples[0].value), samples[0].derivative));
        else
        {
            for (int i = 0; i < n; i++)
            {
                push_back(Sample(samples[i].time, bigM_clamping(samples[i].value), samples[i].derivative));
            }
        }
        lower_signal.push_back(Sample(samples[0].time, -BigM, 0.));
        upper_signal.push_back(Sample(samples[0].time,  BigM, 0.));
    }

    void Signal::addLastSample() {
        if (endTime> back().time) 
            push_back(Sample(endTime, back().valueAt(endTime),0.));
    }
    
    void Signal::appendSample(double t, double v) {
        appendSample(t,v,0.,false);
    }

    void Signal::appendConstantSample(double t, double v) {
        appendSample(t,v,0.,false);
    }

    void Signal::appendLinearSample(double t, double v) {
        appendSample(t,v,0.,true);
    }

    void Signal::appendLinearSample(double t, double v, double d) {
        appendSample(t,v,d,true);
    }

    void Signal::appendSample(double t, double v, double d)
    {
        appendSample(t,v,d,false);
    }

    void Signal::appendSample(double t, double v, double d, bool interp)
    {
        appendSample(t, v, d, interval(-BigM, BigM), interval(0.), interp);
    }
    
	void Signal::appendSample(double t, double v, interval itv) {
		appendSample(t, v, 0., itv, interval(0.), false);
	}

    void Signal::appendSample(double t, double v, double d, interval itv, interval d_itv, bool interp) {

        if ((t <= endTime) && size() > 0)
        {
            return;
        }

        if (size() == 0)
        {
            push_back(Sample(t, bigM_clamping(v), d));
            lower_signal.push_back(Sample(t, bigM_clamping(itv.begin), d_itv.begin));
            upper_signal.push_back(Sample(t, bigM_clamping(itv.end), d_itv.end));
            beginTime = t;
            endTime = t;
        }
        else
        {
            if (interp) 
            {   // we discard the derivative of last sample and make it so it 
                // interpolates linearly with the new one   
                back().derivative = (v - back().value) / (t - back().time);
                lower_signal.back().derivative = (itv.begin - lower_signal.back().value) / (t - lower_signal.back().time);
                upper_signal.back().derivative = (itv.end - upper_signal.back().value) / (t - upper_signal.back().time);
            }
            push_back(Sample(t, bigM_clamping(v), d));
            // we add sample on lower and upper bounds only if it is not redundant
            if (fabs(lower_signal.back().valueAt(t) - lower_signal.front().value) > ZERO_POS || fabs(lower_signal.back().derivative - lower_signal.front().derivative) > ZERO_POS)
                lower_signal.push_back(Sample(t, bigM_clamping(itv.begin), d_itv.begin));
            if (fabs(upper_signal.back().valueAt(t) - upper_signal.front().value) > ZERO_POS || fabs(upper_signal.back().derivative - upper_signal.front().derivative) > ZERO_POS)
                upper_signal.push_back(Sample(t, bigM_clamping(itv.end), d_itv.end));
            endTime = t;
        }
	}
    
    void Signal::inflate(double r) {
		fesetround(FE_DOWNWARD);
        lower_signal = *this; // TODO check is this a copy?
        for (auto s : lower_signal) {
            s.value = bigM_clamping(s.value - r);
        }
        fesetround(FE_UPWARD);
        upper_signal = *this; // TODO same
        for (auto s : upper_signal) {
            s.value = bigM_clamping(s.value + r);
        }
        fesetround(FE_TONEAREST);
    }

    //remove redundant sample (no jump and no change in derivative)
    void Signal::simplify() {
#ifdef DEBUG___
        printf(">>>Signal::simplify:                          IN." );
        cout << "IN: " << *this << endl;
#endif
        if (size() < 3)
            return;

        double first_time = front().time;
        push_back(front());
        pop_front();
        
        while (front().time != first_time) {
            if( fabs(back().valueAt(front().time) - front().value) > ZERO_POS || fabs(back().derivative - front().derivative) > ZERO_POS) {
                push_back(front());
            }
            pop_front();
        }

        // check last sample
        if (back().time < endTime)
            push_back(Sample(endTime, back().valueAt(endTime), 0.));

#ifdef DEBUG___	
        cout << "OUT: " << *this << endl;
        printf("<<<Signal::simplify:                          OUT.\n");
#endif
    }

    void Signal::resize(double t_start, double t_end) {

        // Sanitize inputs
        if (empty())
            return;
        
        if ( t_end<t_start-1e-14 ) {
            clear_all();
            beginTime=0.;
            endTime=0.;
            return;
        }
        else if (t_end < t_start)
            t_end = t_start;  
                 
        // if t_start after endTime
        if (t_start>=endTime) {
            double v = back().valueAt(t_start);
            double d = back().derivative;
            clear_all();
            push_front(Sample(t_start, v, d));
            lower_signal.push_front(Sample(t_start, -BigM, 0.));
            upper_signal.push_front(Sample(t_start, BigM, 0.));
        } 
        // if t_end before beginTime
        else if (t_end<beginTime) {
            double v = front().valueAt(t_start);
            double d = front().derivative;
            clear_all();
            push_front(Sample(t_start, v, d));    
            lower_signal.push_front(Sample(t_start, -BigM, 0.));
            upper_signal.push_front(Sample(t_start, BigM, 0.));      
        }
        else {
            //trim or extend front of signal
            while(front().time < t_start) 
                pop_front();
            while(lower_signal.front().time < t_start) 
                lower_signal.pop_front();
            while(upper_signal.front().time < t_start) 
                upper_signal.pop_front();
            
            if (front().time != t_start) {
                Sample new_front = Sample(t_start, front().valueAt(t_start), front().derivative);
                pop_front();
                push_front(new_front);
            }
            if (lower_signal.front().time != t_start) {
                Sample new_front = Sample(t_start, lower_signal.front().valueAt(t_start), lower_signal.front().derivative);
                lower_signal.pop_front();
                lower_signal.push_front(new_front);
            }
            if (upper_signal.front().time != t_start) {
                Sample new_front = Sample(t_start, upper_signal.front().valueAt(t_start), upper_signal.front().derivative);
                upper_signal.pop_front();
                upper_signal.push_front(new_front);
            }
            //trim or extend end of signal
            while(t_end<back().time) pop_back();
            while(t_end<lower_signal.back().time) lower_signal.pop_back();
            while(t_end<upper_signal.back().time) upper_signal.pop_back();
        }
        beginTime = t_start;
        endTime = t_end;
        
    }
    
    void Signal::resize(double t_start, double t_end, double v) {
        // Resize signal to begin at time t_start and end at time t_end 
        // Consider obsoleting this implementation and using the one above instead
        // TODO remove or deal with lower and upper _signal
    #ifdef DEBUG__
            printf(">>>Signal::resize:                            IN.\n");
        cout << "to start_time:" << t_start << " and end_time:" << t_end << endl;
        cout << "IN: " << *this << endl;
    #endif
        if ( t_end<t_start-1e-14 ) {
            clear_all();
            beginTime=0.;
            endTime=0.;

    #ifdef DEBUG__
            cout << "OUT(premature): " << *this << endl;
            printf("<<<Signal::resize:                            OUT.\n");
    #endif
            return;
        }
        else 
            if (t_end < t_start)
                t_end = t_start;  // hope I don't_end regret this.
        Sample first;

        //trim or extend front of signal
        if(beginTime > t_start) {
            //double der = (front().value-v)/(front().time-t_start);
            cout << "push font ?? " << Sample(t_start, front().value, 0) << endl;
            push_front(Sample(t_start, front().value, 0));
        }
        else {
            while((!empty())&&(front().time < t_start)) {
                first=front();
                pop_front();
            }
            if (empty()) {
                cout << "push empty " << first << endl;
                push_front(Sample(t_start, first.valueAt(t_start), 0));
                if (endTime < t_start)
                    endTime = t_start;
            }
            else {
                if (front().time > t_start)  {
                    double val = first.valueAt(t_start);				
                    push_front(Sample(t_start,val,first.derivative));
                }
            }
        }

        v = bigM_clamping(v);
        //trim or extend back of signal
        if(endTime < t_end) {
            //		cout << "push_back here" << endl;
            if (back().value != v || back().derivative != 0.)
                push_back(Sample(endTime, v, 0));
        }
        else {
            while(!empty()&&back().time >t_end) {
                pop_back();
            }
        }
        if (empty()) {
            //		cout << "push_back empty" << endl;
            push_back(Sample(t_start, v, 0));
        }
        beginTime=t_start;
        endTime=t_end;
#ifdef DEBUG__
        cout << "OUT: " << *this << endl;
        printf("<<<Signal::resize:                            OUT.\n");
#endif
    }

    void Signal::shift(double a) {
        Signal::iterator i;

        beginTime=beginTime + a;
        endTime=endTime + a;

        for(i = begin(); i != end(); i++) {
            i->time=i->time + a;
        }
        for(i = lower_signal.begin(); i != lower_signal.end(); i++) {
            i->time=i->time + a;
        }
        for(i = upper_signal.begin(); i != upper_signal.end(); i++) {
            i->time=i->time + a;
        }
    }

    // TODO also write and read lower and upper signal
    void Signal::read_from_file(const string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::invalid_argument("signal file " + filename + " not found");
        }
        
        clear_all();

        std::string line;
        while(std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            string token;
            vector<double> row;

            while (std::getline(ss, token, ',')) row.push_back(std::stod(token));

            int row_size = row.size();

            if (row_size != 2 && row_size != 3) {
                throw std::invalid_argument("Invalid signal format in file " + filename);
            }

            if (row_size == 2) {
                appendSample(row[0], bigM_clamping(row[1]));
            } else {
                appendSample(row[0], bigM_clamping(row[1]), row[2]);
            }

        }
    }

    // TODO also write and read lower and upper signal
    void Signal::write_to_file(const string& filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::invalid_argument("Could not open file " + filename + " for writing");
        }

        for (const auto& sample : *this) {
            file << sample.time << "," << sample.value << "," << sample.derivative << "\n";
        }

        file << endTime << "," << back().valueAt(endTime) << ",0\n"; // FIXME: is 0 derivative correct here ?
    }
    

    // TO TEST
	bool Signal::operator==(const Signal& that) const {
        if (beginTime != that.beginTime || endTime != that.endTime) {
            return false;
        }

        auto i1 = cbegin();
        for (auto i2 : that) {
            if (i1 == cend() || *i1 != i2) {
                return false;
            }
            i1++;
        }
        if (i1 != cend())
            return false;

        i1 = lower_signal.cbegin();
        for (auto i2 : that.lower_signal) {
            if (i1 == lower_signal.cend() || *i1 != i2) {
                return false;
            }
            i1++;
        }
        if (i1 != lower_signal.cend())
            return false;

        i1 = upper_signal.cbegin();
        for (auto i2 : that.upper_signal) {
            if (i1 == upper_signal.cend() || *i1 != i2) {
                return false;
            }
            i1++;
        }
        if (i1 != upper_signal.cend())
            return false;

        return true;
    }


	bool Signal::operator!=(const Signal& that) const {
        return !(*this == that);
    }

    Signal Signal::operator+(const Signal& that) const {
        // Addition of two signals defined on the intersection of time domains.
        // TODO Compute on the union of time domains?
        Signal result;
    	merge_signals_with_op(result, *this, that, [](double a, double b){return a + b;}, [](double, double, double dL, double dR){return dL + dR;});
        fesetround(FE_DOWNWARD);
        Signal low(lower_signal);
        Signal t_low(that.lower_signal);
        Signal r_low(result.lower_signal);
        r_low.clear();
    	merge_signals_with_op(r_low, low, t_low, [](double a, double b){return a + b;}, [](double, double, double dL, double dR){return dL + dR;});
        r_low.simplify();
        result.lower_signal = r_low;
        fesetround(FE_UPWARD);
        Signal up(upper_signal);
        Signal t_up(that.upper_signal);
        Signal r_up(result.upper_signal);
        r_up.clear();
    	merge_signals_with_op(r_up, up, t_up, [](double a, double b){return a + b;}, [](double, double, double dL, double dR){return dL + dR;});
        r_up.simplify();
        result.upper_signal = r_up;
        fesetround(FE_TONEAREST);
        result.simplify();
        return result;
    }

    Signal Signal::operator-(const Signal& that) const {
        return *this + (that * -1.);
    }

	Signal Signal::operator*(double p) const {
        Signal result = Signal();
        for (const Sample &s : *this) {
            result.appendSample(s.time, bigM_clamping(p*s.value), p*s.derivative);
        }
        // remove default [-BigM,BigM]
        result.lower_signal.clear();
        result.upper_signal.clear();
        if (p >= 0) {
            fesetround(FE_DOWNWARD);
            for (const Sample &s : lower_signal) {
                result.lower_signal.push_back(Sample(s.time, std::min(p*s.value, BigM), p*s.derivative));
            }
            fesetround(FE_UPWARD);
            for (const Sample &s : upper_signal) {
                result.upper_signal.push_back(Sample(s.time, std::min(p*s.value, BigM), p*s.derivative));
            }
        } else {
            fesetround(FE_DOWNWARD);
            for (const Sample &s : upper_signal) {
                result.lower_signal.push_back(Sample(s.time, std::max(p*s.value, -BigM), p*s.derivative));
            }
            fesetround(FE_UPWARD);
            for (const Sample &s : lower_signal) {
                result.upper_signal.push_back(Sample(s.time, std::max(p*s.value, -BigM), p*s.derivative));
            }
        }
        fesetround(FE_TONEAREST);
        return result;
    }

	Signal Signal::operator/(double p) const {
        return *this * (1/p);
    }


    /*
     * friend functions
     */
    std::ostream & operator<<(std::ostream & out, const Point & point) {
        out <<"time: " << point.time << " value: " << point.value ;
        return out;
    }

    std::ostream & operator<<(std::ostream & out, const Sample & sample) {

        out << "time: " << std::setprecision(4) << std::setw(10) << sample.time; 
        out << "  value: " << std::setprecision(4) << std::setw(10)  << sample.value; 
        out << "  derivative: " << std::setprecision(4) << std::setw(10)  << sample.derivative;
        return out;
    }

    std::ostream & operator<<(std::ostream & out, const Sequence & M) {
        Sequence::const_iterator i;

        if(M.empty()) return out << "EMPTY" << std::endl;

        for(i = M.begin(); i != M.end(); i++) {
            out << *i << std::endl;
        }
        return out;
    }

    std::ostream & operator<<(std::ostream & out, const Signal & y) {
        Signal::const_iterator i;

        if(y.empty()) return out << "Empty Signal." << std::endl;
        
        out << "begin_time: " << y.beginTime;
        out << "  end_time: " << y.endTime <<  std::endl;
        
        for(i = y.begin(); i != y.end(); i++) {
            out << *i << std::endl;
        }
        out << "lower signal: " << endl;
        for(i = y.lower_signal.begin(); i != y.lower_signal.end(); i++) {
            out << *i << std::endl;
        }
        out << "upper signal: " << endl;
        for(i = y.upper_signal.begin(); i != y.upper_signal.end(); i++) {
            out << *i << std::endl;
        }
        return out;
    }

}
