#include "signal.h"
#include "iomanip"
#include <vector>
#include <sstream>


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
        push_back(Sample(T, V, 0.));

    }

    Signal::Signal(double * T, double * V, int n) {
#ifdef DEBUG__
        printf(">> Signal::Signal:                              IN." );
#endif

        beginTime=T[0];
        endTime = T[n - 1];

        if (n == 1)
            push_back(Sample(T[0], V[0], 0.));
        else
        {
            for (int i = 0; i < n - 1; i++)
            {
                push_back(Sample(T[i], V[i], (V[i + 1] - V[i]) / (T[i + 1] - T[i])));
            }
            push_back(Sample(T[n - 1], V[n - 1], 0.));
        }

#ifdef DEBUG__
        printf("<< Signal::Signal:                            OUT." );
#endif
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
        if ((t <= endTime) && size() > 0)
        {            
            return;
        }

        if (size() == 0)
        {
            push_back(Sample(t, v, d));
            beginTime = t;
            endTime = t;
        }
        else
        {
            if (interp) 
            {   // we discard the derivative of last sample and make it so it 
                // interpolates linearly with the new one   
                back().derivative = (v - back().value) / (t - back().time);
            }
            push_back(Sample(t, v, d));
            endTime = t;
        }
    }

    //remove redundant sample (no jump and no change in derivative)
    void Signal::simplify() {
#ifdef DEBUG___
        printf(">>>Signal::simplify:                          IN." );
        cout << "IN: " << *this << endl;
#endif

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
            clear();
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
            clear();
            push_front(Sample(t_start, v, d));            
        } 
        // if t_end before beginTime
        else if (t_end<beginTime) {
            double v = front().valueAt(t_start);
            double d = front().derivative;
            clear();
            push_front(Sample(t_start, v, d));          
        }
        else {
            //trim or extend front of signal
            while(front().time < t_start) 
                pop_front();        
            
            if (front().time != t_start) {
                Sample new_front = Sample(t_start, front().valueAt(t_start), front().derivative);
                pop_front();
                push_front(new_front);
            }
            //trim or extend end of signal
            while(t_end<back().time) pop_back();
        }
        beginTime = t_start;
        endTime = t_end;
        
    }
    
    void Signal::resize(double t_start, double t_end, double v) {
        // Resize signal to begin at time t_start and end at time t_end 
        // Consider obsoleting this implementation and using the one above instead
    #ifdef DEBUG__
            printf(">>>Signal::resize:                            IN.\n");
        cout << "to start_time:" << t_start << " and end_time:" << t_end << endl;
        cout << "IN: " << *this << endl;
    #endif
        if ( t_end<t_start-1e-14 ) {
            clear();
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
    }

    void Signal::read_from_file(const string& filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::invalid_argument("signal file " + filename + " not found");
        }
        
        clear();

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
                appendSample(row[0], row[1]);
            } else {
                appendSample(row[0], row[1], row[2]);
            }

        }
    }

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
    
    
	bool Signal::operator==(const Signal& that) const {
        if (beginTime != that.beginTime || endTime != that.endTime) {
            return false;
        }
        auto it = this->getSamplesDeque().cbegin();
        for (auto s : that.getSamplesDeque()) {
            if (it == this->getSamplesDeque().cend() || s != *it) {
                return false;
            }
            it++;
        }
        return true;
    }


	bool Signal::operator!=(const Signal& that) const {
        if (beginTime != that.beginTime || endTime != that.endTime) {
            return true;
        }
        auto it = this->getSamplesDeque().cbegin();
        for (auto s : that.getSamplesDeque()) {
            if (it == this->getSamplesDeque().cend() || s != *it) {
                return true;
            }
            it++;
        }
        return false;
    }

    Signal Signal::operator+(const Signal& that) const {
        // Addition of two signals defined on the union of time domains.
        // The value of a signal is assumed to be 0 when outside time domain. // TODO Result can be discontinuous
        Signal result = Signal();
        auto sig1 = this->getSamplesDeque();
        auto sig2 = that.getSamplesDeque();
        auto s2 = sig2.cbegin();
        while (s2 != sig2.cend() && s2->time < this->beginTime) {
            result.appendSample(s2->time, s2->value, s2->derivative);
            s2++;
        }
        for (Sample s1 : sig1) {
            while (s2 != sig2.cend() && s2->time < s1.time) {
                result.appendSample(s2->time, s2->value + this->valueAt(s2->time), s2->derivative + this->derivativeAt(s2->time));
                s2++;
            }
            if (s1.time < that.beginTime) {
                result.appendSample(s1.time, s1.value, s1.derivative);
            } else {
                result.appendSample(s1.time, s1.value + that.valueAt(s1.time), s1.derivative + that.derivativeAt(s1.time));
            }
        }
        while (s2 != sig2.cend()) {
            result.appendSample(s2->time, s2->value, s2->derivative);
            s2++;
        }

        result.simplify();
        return result;
    }

    Signal Signal::operator-(const Signal& that) const {
        return *this + (that * -1.);
    }

	Signal Signal::operator*(double p) const {
        Signal result = Signal();
        for (Sample s : this->getSamplesDeque()) {
            result.appendSample(s.time, p*s.value, p*s.derivative);
        }
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

        if(y.begin() == y.end()) return out << "Empty Signal." << std::endl;
        
        out << "begin_time: " << y.beginTime;
        out << "  end_time: " << y.endTime <<  std::endl;
        
        for(i = y.begin(); i != y.end(); i++) {
            out << *i << std::endl;
        }
        return out;
    }

}
