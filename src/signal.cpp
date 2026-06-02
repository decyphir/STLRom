#include "signal.h"
#include "iomanip"
#include <vector>
#include <sstream>


namespace STLRom {

    // Default semantics and interpolation
    Semantics Signal::semantics = Semantics::SPACE;
    Interpol Signal::interpol = Interpol::LINEAR; 

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

    // TODO refactor piecewise constant vs piecewise linear...
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
            switch (Signal::interpol)
            {
            case Interpol::PREVIOUS:
                for (int i = 0; i < n - 1; i++)
                {
                    push_back(Sample(T[i], V[i], 0.));
                }
                break;
            case Interpol::LINEAR:
                for (int i = 0; i < n - 1; i++)
                {
                    push_back(Sample(T[i], V[i], (V[i + 1] - V[i]) / (T[i + 1] - T[i])));
                }
                break;
            default:
                throw invalid_argument("Invalid interpolation value");
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
        if ((t <= endTime) && size() > 0)
            return;

        switch (Signal::interpol)
        {
        case Interpol::PREVIOUS:
            if (size() == 0)
            {
                push_back(Sample(t, v, 0.));
                beginTime = t;
                endTime = t;
            }
            else
            {
                push_back(Sample(t, v, 0.));
                endTime = t;
            }
            break;
        case Interpol::LINEAR:
            if (size() == 0)
            {
                push_back(Sample(t, v, 0.));
                beginTime = t;
                endTime = t;
            }
            else
            {
                back().derivative = (v - back().value) / (t - back().time);
                push_back(Sample(t, v, 0.));
                endTime = t;
            }
            break;
        default:
            throw invalid_argument("Invalid interpolation value");
        }
    }

    void Signal::appendSample(double t, double v, double d)
    {
        if ((t <= endTime) && size() > 0)
        {            
            return;
        }
        switch (Signal::interpol)
        {
        case Interpol::PREVIOUS:
            if (size() == 0)
            {
                push_back(Sample(t, v, 0.));
                beginTime = t;
                endTime = t;
            }
            else
            {
                push_back(Sample(t, v, 0.));
                endTime = t;
            }
            break;
        case Interpol::LINEAR:
            if (size() == 0)
            {
                push_back(Sample(t, v, d));
                beginTime = t;
                endTime = t;
            }
            else
            {
                // back().derivative = (v - back().value) / (t - back().time);
                push_back(Sample(t, v, d));
                endTime = t;
            }
            break;
        default:
            throw invalid_argument("Invalid interpolation value");
        }
    }

    void Signal::appendSignal(Signal s) {

        Signal::const_iterator iter_s;

        for(iter_s = s.begin(); iter_s != s.end(); iter_s++) {
            appendSample((*iter_s).time,(*iter_s).value,(*iter_s).derivative);
        }

    }

    //remove linear interpolations
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
            while(front().time < t_start) pop_front();        
            push_front(Sample(t_start, front().valueAt(t_start), front().derivative));
        
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
            push_front(Sample(t_start, front().value, 0));
        }
        else {
            while((!empty())&&(front().time < t_start)) {
                first=front();
                pop_front();
            }
            if (empty()) {
                //			cout << "push empty " << first << endl;
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

    void Signal::removeInf() {

        while((!empty())&&
              (back().value==TOP ||
               back().derivative == TOP ||
               back().value==BOTTOM ||
               back().derivative == BOTTOM)) {
            pop_back();
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
