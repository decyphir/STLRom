#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>
#include "stl_driver.h"
#include "signal.h"
#include "tools.h"
#include "transducer.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

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
		.def_readwrite("value",&RobonTL::Point::value);

	//Class STLDriver
	py::class_<RobonTL::STLDriver>(m, "STLDriver")
		.def(py::init<>())
		.def("parse_file",&RobonTL::STLDriver::parse_file)
		.def("get_monitor",&RobonTL::STLDriver::get_monitor)
		.def("get_signals_names",&RobonTL::STLDriver::get_signals_names);

	//Class STLMonitor
	py::class_<RobonTL::STLMonitor>(m, "STLMonitor")
		.def(py::init<>())
		.def("add_sample",&RobonTL::STLMonitor::add_sample)
		.def("get_lower_rob",&RobonTL::STLMonitor::get_lower_rob)
		.def("get_upper_rob",&RobonTL::STLMonitor::get_upper_rob)
 		.def_readwrite("rob",&RobonTL::STLMonitor::rob)
		.def_readwrite("lower_rob",&RobonTL::STLMonitor::lower_rob)
		.def_readwrite("upper_rob",&RobonTL::STLMonitor::upper_rob)
		.def_readwrite("formula",&RobonTL::STLMonitor::formula)
		.def_readwrite("data",&RobonTL::STLMonitor::data)
		.def_readwrite("current_time",&RobonTL::STLMonitor::current_time);

	m.def("read_point",&read_point,"A function that reads and print a point");
}
