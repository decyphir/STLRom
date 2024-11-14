#ifndef SIGNAL_H
#define SIGNAL_H

#include <deque>
#include <iostream>
#include <fstream>
#include <limits>
#include <iomanip>
//#include <math.h>
//#include <algorithm>
// TODO maybe make TOP and BOTTOM static signal attribute ...
#define BOTTOM (-Signal::BigM) //-std::numeric_limits<double>::infinity()
#define TOP (Signal::BigM) //std::numeric_limits<double>::infinity()

using namespace std;

/*
double fmin(double a,double b)
{
  return (a<b)?a:b;
}

double fmax(double a,double b)
{
  return (b<a)?a:b;
}
*/
namespace STLRom {

class Point {
public:
	double time;
	double value;

	Point(): time(0.), value(0.) { }
	Point(double t, double v) : time(t), value(v) { }
	void print_point(){cout << std::fixed << std::setprecision(5) <<"Time "<<time<<" Value "<<value<<endl;}
	
	friend std::ostream & operator<<(std::ostream &, const Point &);
};

class Sample : public Point {
public:
	double derivative;

	Sample() {derivative =0;}
	Sample(double t, double v, double d) : Point(t,v), derivative(d) { }	

	double valueAt(const double &) const;
	double timeIntersect(const Sample &) const;

	void print_sample(){cout << std::fixed << std::setprecision(5) <<"Time "<<time<<" Value "<<value<< " Derivative "<< derivative << endl;}
	
	friend std::ostream & operator<<(std::ostream &, const Sample &);
};

inline
double Sample::valueAt(const double & t) const {
	return value + derivative * (t - time);
}

inline
double Sample::timeIntersect(const Sample & point) const {
	return (value - point.value + (point.derivative * point.time) - (derivative * time)) / (point.derivative - derivative);
}

//piecewise-constant, right-continuous signals
class Sequence : public std::deque<Point> { 
public:
	Sequence() { }

	friend std::ostream & operator<<(std::ostream &, const Sequence &);
};

enum class Semantics {
		SPACE,
		LEFT_TIME,
		RIGHT_TIME
	};

enum class Interpol {
		PREVIOUS,
		LINEAR
	};

//piecewise-linear, right-continuous signals
class Signal : public std::deque<Sample> {
public:

	static double BigM;
	
	// Guess that should be else where... 
	
	static Semantics semantics;
	static Interpol interpol;


	inline static void set_semantics(const std::string& sem) {
		if (sem == "SPACE") {
			semantics = Semantics::SPACE;
		} else if (sem == "LEFT_TIME") {
			semantics = Semantics::LEFT_TIME;
		} else if (sem == "RIGHT_TIME") {
			semantics = Semantics::RIGHT_TIME;
		} else {
			throw std::invalid_argument("Invalid semantics string");
		}
	}

	inline static std::string get_semantics() {
		switch (semantics) {
			case Semantics::SPACE:
				return "SPACE";
			case Semantics::LEFT_TIME:
				return "LEFT_TIME";
			case Semantics::RIGHT_TIME:
				return "RIGHT_TIME";
			default:
				throw std::invalid_argument("Invalid semantics value");
		}
	}

	inline static void set_interpol(const std::string& interp) {
		if (interp == "PREVIOUS") {
			interpol = Interpol::PREVIOUS;
		} else if (interp == "LINEAR") {
			interpol = Interpol::LINEAR;
		} else {
			throw std::invalid_argument("Invalid interpol string");
		}
	}

	inline static std::string get_interpol() {
		switch (interpol) {
			case Interpol::PREVIOUS:
				return "PREVIOUS";
			case Interpol::LINEAR:
				return "LINEAR";
			default:
				throw std::invalid_argument("Invalid interpol value");
		}
	}


	inline static void set_BigM(double val){
		BigM=val;
	}

	inline static int get_BigM(){
		return BigM;
	}


	double beginTime;
	double endTime;
	Signal(): beginTime(0.), endTime(0.) { };
	Signal(double, double, int); 
	Signal(double *, double *, int); //create continuous signal from array of sampling points (time, value) with linear interpolation
	void appendSample(double, double);
	void appendSample(double, double, double);
    void appendSignal(Signal);
    void simplify(); //remove sampling points where (y,dy) is continuous.
	void resize(double, double, double); //restricts/extends the signal to [s,t) with default value v where not defined
	void shift(double); //shifts the signal of delta_t time units
    void addLastSample(); // add a sample at endTime. 
	void removeInf();

	inline const std::deque<Sample>& getSamplesDeque() const {
    	return *this;
    }

	// write signal to file
	void dumpFile(const string filename) const {
		fstream ofs;
		ofs.open(filename.c_str(), std::ofstream::out);
		if (ofs.is_open()) {
			for (auto i = begin(); i != end(); i++) {
				ofs << *i << std::endl;
			}
			ofs.close();
		}
		else {
			cout << "Couldn't open file " << filename.c_str() << " for writing signal" << endl; // TODO implement exception
		}
	}

    /** Robustness computation functions - all these methods first clear content */

    /// Signal becomes boolean version of signal argument (0 if <=0, 1 if > 0)
    void compute_boolean(const Signal&); 

	// Time robustness
    void compute_left_time_rob(const Signal&); 	
	void compute_right_time_rob(const Signal&); 
	

	/// Signal becomes neg of signal argument
    void compute_not(const Signal&); 

    /// Signal becomes conjunction (min) of the two arguments
    void compute_and(const Signal&, const Signal&);

    /// Signal becomes disjunction (max) of the two arguments
    void compute_or(const Signal&, const Signal&);

    /// Signal becomes implication (max of not x and y) of the two arguments
    void compute_implies(const Signal&, const Signal&);

    /// Signal becomes robust eventually of argument
    void compute_eventually(const Signal&); 

    /// ev_[0, a) 
    void compute_bounded_eventually(const Signal&, double);
    
    /// ev_[a,b]
    void compute_timed_eventually(const Signal&, double, double);

    /// Signal becomes robust globally of argument
    void compute_globally(const Signal&); 

    /// alw_[0, a) 
    void compute_bounded_globally(const Signal&, double);
    
    /// alw_[a,b]
    void compute_timed_globally(const Signal&, double, double);
    
    /// Signal becomes robust untimed until 
    void compute_until(const Signal &, const Signal &);
    
    /// until_[a,b]
    void compute_timed_until(const Signal&, const Signal&, double, double );

    /** TODO Auxiliary routines (should be privatized probably) */
    
    /// computes max lemire window of size a of argument signal 
    void compute_plateau_max(const Signal &, double);
    void compute_plateau_min(const Signal &, double);


	friend std::ostream & operator<<(std::ostream &, const Signal &);
};
}
#endif
