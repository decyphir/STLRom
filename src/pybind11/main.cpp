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
using namespace RobonTL;
namespace py = pybind11;


int read_point(){
	double time,value;
	cout<<"Enter time and value"<<endl;
	cin>>time>>value;
	RobonTL::Point p(time,value);
	p.print_point();
	return 0;
}

void print_monitor(STLDriver& d) {
	stringstream os; 
	
	/*
	auto it = d.formula_map.find("phi");
	transducer *phi;
	if (it != d.formula_map.end())
	{
		//phi = (it->second)->clone();
		phi = (it->second);
		py::print(" found, yeah.");
	}
	else
	{
		py::print(" undefined.");
	}
	*/
	STLMonitor m = d.get_monitor("phi");
	transducer* phi = m.formula;
	os << "Monitor lower rob:" << m.lower_rob <<endl;
	phi->print(os);
	py::print(os.str());
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
		.def("parse_string",&RobonTL::STLDriver::parse_string)
		.def("disp",&RobonTL::STLDriver::disp)		
		.def("add_sample",&RobonTL::STLDriver::add_sample)
		.def("get_monitor",&RobonTL::STLDriver::get_monitor)
		.def("get_signals_names",&RobonTL::STLDriver::get_signals_names)
		.def("get_online_rob",&RobonTL::STLDriver::get_online_rob)
		.def("get_param",&RobonTL::STLDriver::get_param)
		.def("set_param",&RobonTL::STLDriver::set_param)
		.def_readwrite("data",&RobonTL::STLDriver::data);
	
	//Class STLMonitor
	py::class_<RobonTL::STLMonitor>(m, "STLMonitor")
		.def(py::init<>())
		.def("add_sample",&RobonTL::STLMonitor::add_sample)
		.def("get_lower_rob",&RobonTL::STLMonitor::get_lower_rob)
		.def("get_upper_rob",&RobonTL::STLMonitor::get_upper_rob)
 		.def("display_formula",&RobonTL::STLMonitor::display_formula) 		
		.def_readwrite("rob",&RobonTL::STLMonitor::rob)
		.def_readwrite("lower_rob",&RobonTL::STLMonitor::lower_rob)
		.def_readwrite("upper_rob",&RobonTL::STLMonitor::upper_rob)
		.def_readwrite("formula",&RobonTL::STLMonitor::formula)
		.def_readwrite("data",&RobonTL::STLMonitor::data)
		.def_readwrite("current_time",&RobonTL::STLMonitor::current_time);
	//Class Signal
	py::class_<RobonTL::Signal>(m, "Signal")
		.def(py::init<>())
		.def("set_BigM",&RobonTL::Signal::set_BigM)
		.def("get_BigM",&RobonTL::Signal::get_BigM);

	m.def("read_point",&read_point,"A function that reads and print a point");
	m.def("print_monitor",&print_monitor,"Prints a monitor (temporary test function).");
	m.def("rand_trace_data",&rand_trace_data,"fuction generating random traces");
}
