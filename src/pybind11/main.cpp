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


PYBIND11_MODULE(stlrom, m) {
	//Class Point
	py::class_<STLRom::Point>(m, "Point")
		.def(py::init<double,double>())
		.def("print_point",&STLRom::Point::print_point)
		.def_readwrite("time",&STLRom::Point::time)
		.def_readwrite("value",&STLRom::Point::value);

	//Class Sample	
	py::class_<STLRom::Sample>(m, "Sample")
		.def(py::init<double,double,double>())
		.def("value_at", &STLRom::Sample::valueAt)
		.def("time_intersect", &STLRom::Sample::timeIntersect)
		.def("print_sample",&STLRom::Sample::print_sample)
		.def_readwrite("time",&STLRom::Sample::time)
		.def_readwrite("value",&STLRom::Sample::value)
		.def_readwrite("derivative",&STLRom::Sample::derivative);
		
	//Class Signal
	py::class_<STLRom::Signal>(m, "Signal")
		.def(py::init<>())
		.def("append_sample", (void (STLRom::Signal::*)(double, double)) &STLRom::Signal::appendSample)
		.def("append_sample", (void (STLRom::Signal::*)(double, double, double)) &STLRom::Signal::appendSample)
		.def("compute_not",&STLRom::Signal::compute_not)	
		.def("compute_and", &STLRom::Signal::compute_and)
		.def("compute_or", &STLRom::Signal::compute_or)
		.def("compute_boolean",&STLRom::Signal::compute_boolean)	
		.def("compute_left_time_rob",&STLRom::Signal::compute_left_time_rob)	
		.def("compute_right_time_rob",&STLRom::Signal::compute_right_time_rob)	
		.def("get_samples_list",&STLRom::Signal::getSamplesDeque)
		.def_readwrite("begin_time",&STLRom::Signal::beginTime)
		.def_readwrite("end_time",&STLRom::Signal::endTime)
		.def_static("set_semantics",  &STLRom::Signal::set_semantics)
		.def_static("get_semantics",  &STLRom::Signal::get_semantics)
		.def_static("set_interpol",  &STLRom::Signal::set_interpol)
		.def_static("get_interpol",  &STLRom::Signal::get_interpol)
		.def_static("set_BigM",&STLRom::Signal::set_BigM)
		.def_static("get_BigM",&STLRom::Signal::get_BigM);

	m.def("read_point",&read_point,"A function that reads and print a point");
	m.def("print_monitor",&print_monitor,"Prints a monitor (temporary test function).");
	m.def("rand_trace_data",&rand_trace_data,"function generating random traces");

	//Class STLMonitor
	py::class_<STLRom::STLMonitor>(m, "STLMonitor")
		.def(py::init<>())
		.def("__repr__", [](const STLMonitor &monitor) {
            std::ostringstream oss;
            oss << monitor;
            return oss.str();
        })
		.def("add_sample",&STLRom::STLMonitor::add_sample)
		.def("get_lower_rob",&STLRom::STLMonitor::get_lower_rob)
		.def("get_upper_rob",&STLRom::STLMonitor::get_upper_rob)
		.def("set_semantics",&STLRom::STLMonitor::set_semantics)
		.def("get_semantics",&STLRom::STLMonitor::get_semantics)
		.def("set_interpol",&STLRom::STLMonitor::set_interpol)
		.def("get_interpol",&STLRom::STLMonitor::get_interpol)	
		.def("eval_rob",(double (STLRom::STLMonitor::*)()) &STLRom::STLMonitor::eval_rob)		
		.def("eval_rob",(double (STLRom::STLMonitor::*)(double)) &STLRom::STLMonitor::eval_rob)		
		.def("eval_rob",(double (STLRom::STLMonitor::*)(double,double)) &STLRom::STLMonitor::eval_rob)		
		.def("set_eval_time",&STLRom::STLMonitor::set_eval_time)
		.def("set_param",&STLRom::STLMonitor::set_param)
		.def("get_param",&STLRom::STLMonitor::get_param)
		.def("get_signal",&STLRom::STLMonitor::get_signal)
 		.def("display_formula",&STLRom::STLMonitor::display_formula) 		
		.def_readwrite("rob",&STLRom::STLMonitor::rob)
		.def_readwrite("lower_rob",&STLRom::STLMonitor::lower_rob)
		.def_readwrite("upper_rob",&STLRom::STLMonitor::upper_rob)
		.def_readwrite("formula",&STLRom::STLMonitor::formula)
		.def_readwrite("data",&STLRom::STLMonitor::data)
		.def_readwrite("start_time",&STLRom::STLMonitor::start_time)
		.def_readwrite("end_time",&STLRom::STLMonitor::end_time)
		;

	//Class STLDriver
	py::class_<STLRom::STLDriver>(m, "STLDriver")
		.def(py::init<>())
		.def("parse_file",&STLRom::STLDriver::parse_file)
		.def("parse_string",&STLRom::STLDriver::parse_string)
		.def("disp",&STLRom::STLDriver::disp)		
		.def("add_sample",&STLRom::STLDriver::add_sample)
		.def("get_monitor",&STLRom::STLDriver::get_monitor)
		.def("get_signals_names",&STLRom::STLDriver::get_signals_names) 
		.def("set_semantics",&STLRom::STLDriver::set_semantics)
		.def("get_semantics",&STLRom::STLDriver::get_semantics)
		.def("set_interpol",&STLRom::STLDriver::set_interpol)
		.def("get_interpol",&STLRom::STLDriver::get_interpol)
		
		.def("get_online_rob",
		[](STLRom::STLDriver& self,  const string &phi_in, double t0) {
             return self.get_online_rob(phi_in, t0); 
         },
	     py::arg("phi_in") = "phi",
         py::arg("t0") = 0.)
		.def("get_param",&STLRom::STLDriver::get_param)
		.def("set_param",&STLRom::STLDriver::set_param)
		.def_readwrite("data",&STLRom::STLDriver::data);
	
	
}
