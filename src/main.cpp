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


	/*Class STLDriver
	py::class_<RobonTL::STLDriver>(m, "STLDriver")
		.def(py::init<>());
		.def("parse_file",&RobonTL::STLDriver::parse_file)
		.def("get_monitor",&RobonTL::STLDriver::get_monitor);
	
	//Class STLMonitor
	py::class_<RobonTL::Point>(m, "Point")
		.def(py::init<>())
		.def("get_lower_rob",&RobonTL::STLMonitor::get_lower_rob)
		.def("get_upper_rob",&RobonTL::STLMonitor::get_upper_rob)*/
