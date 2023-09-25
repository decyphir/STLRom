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
using namespace STLRom;
namespace py = pybind11;


int read_point(){
	double time,value;
	cout<<"Enter time and value"<<endl;
	cin>>time>>value;
	STLRom::Point p(time,value);
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

PYBIND11_MODULE(STLRom, m) {
	//Class Point
	py::class_<STLRom::Point>(m, "Point")
		.def(py::init<double,double>())
		.def("print_point",&STLRom::Point::print_point)
		.def_readwrite("time",&STLRom::Point::time)
		.def_readwrite("value",&STLRom::Point::value);

	//Class STLDriver
	py::class_<STLRom::STLDriver>(m, "STLDriver")
		.def(py::init<>())
		.def("parse_file",&STLRom::STLDriver::parse_file)
		.def("parse_string",&STLRom::STLDriver::parse_string)
		.def("disp",&STLRom::STLDriver::disp)		
		.def("add_sample",&STLRom::STLDriver::add_sample)
		.def("get_monitor",&STLRom::STLDriver::get_monitor)
		.def("get_signals_names",&STLRom::STLDriver::get_signals_names) 
		//.def("get_online_rob",&STLRom::STLDriver::get_online_rob)
		.def("get_online_rob",
		[](STLRom::STLDriver& self,  const string &phi_in, double t0) {
             return self.get_online_rob(phi_in, t0); 
         },
	     py::arg("phi_in") = "phi",
         py::arg("t0") = 0.)
		.def("get_param",&STLRom::STLDriver::get_param)
		.def("set_param",&STLRom::STLDriver::set_param)
		.def_readwrite("data",&STLRom::STLDriver::data);
	
	//Class STLMonitor
	py::class_<STLRom::STLMonitor>(m, "STLMonitor")
		.def(py::init<>())
		.def("add_sample",&STLRom::STLMonitor::add_sample)
		.def("get_lower_rob",&STLRom::STLMonitor::get_lower_rob)
		.def("get_upper_rob",&STLRom::STLMonitor::get_upper_rob)
 		.def("display_formula",&STLRom::STLMonitor::display_formula) 		
		.def_readwrite("rob",&STLRom::STLMonitor::rob)
		.def_readwrite("lower_rob",&STLRom::STLMonitor::lower_rob)
		.def_readwrite("upper_rob",&STLRom::STLMonitor::upper_rob)
		.def_readwrite("formula",&STLRom::STLMonitor::formula)
		.def_readwrite("data",&STLRom::STLMonitor::data)
		.def_readwrite("current_time",&STLRom::STLMonitor::current_time);
	//Class Signal
	py::class_<STLRom::Signal>(m, "Signal")
		.def(py::init<>())
		.def("set_BigM",&STLRom::Signal::set_BigM)
		.def("get_BigM",&STLRom::Signal::get_BigM);

	m.def("read_point",&read_point,"A function that reads and print a point");
	m.def("print_monitor",&print_monitor,"Prints a monitor (temporary test function).");
	m.def("rand_trace_data",&rand_trace_data,"fuction generating random traces");
}
