#include <interval.h>
#include "tools.h"
#include "transducer.h"

using namespace std;

namespace STLRom {

interval::interval() {
	begin = -1;
	end = -1;
}


interval::interval(const interval& that) {
	copy(that);
}
void interval::copy(const interval& that) {
	begin = that.begin;
	end = that.end;
}

interval interval::operator=(const interval& that) {
	if (this != &that)
		copy(that);
	return *this;
}

double interval::mid() {
	return (this->begin + this->end) / 2;
}

bool interval::operator==(const interval &that) {
	return this->begin == that.begin && this->end == that.end;
}

bool interval::operator!=(const interval &that) {
	return !this->interval::operator==(that);
}

string interval::to_string() const {
	ostringstream o;
	o << "[" << begin << "," << end << "]";
	return o.str();
}

std::ostream& operator<<(std::ostream& os, const interval& I) {
	I.print(os);
	return os;
}
;

}
