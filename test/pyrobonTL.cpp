#include <iostream>
#include <stdlib.h>
#include "signal.h"
#include <pybind11/pybind11.h>

using namespace std;
namespace py = pybind11;

int read_point(){
	double time,value;
	cout<<"Enter time and value"<<endl;
	cin>>time>>value;
	RobonTL::Point p(time,value);
	p.print_point();
	return 0;
}

PYBIND11_MODULE(pyrobonTL, m) {
	//Class Point
	py::class_<RobonTL::Point>(m, "Point")
		.def(py::init<double,double>())
		.def("print_point",&RobonTL::Point::print_point)
		.def_readwrite("time",&RobonTL::Point::time)
		.def_readwrite("value",&RobonTL::Point::time);

	
	m.def("read_point",&read_point,"A function that reads and print a point");
}