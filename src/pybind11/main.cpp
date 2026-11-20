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

static const double INF = std::numeric_limits<double>::infinity();

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
	STLMonitor m = d.get_monitor("phi");
	transducer* phi = m.formula;
	os << "Monitor lower rob:" << m.lower_rob <<endl;
	phi->print(os);
	py::print(os.str());
}

// Robustness maps: {depth: {subformula: {"z": Signal, "z_low": Signal, "z_up": Signal}}}
static py::dict rob_map_to_dict(const robustness_map_t &rob_map)
{
	py::dict rob_dict;
	for (const auto& item : rob_map) {
		py::dict info_dict;
		info_dict["z"] = item.second.z;
		info_dict["z_low"] = item.second.z_low;
		info_dict["z_up"] = item.second.z_up;

		py::int_ depth(item.second.depth);
		py::dict depth_dict = rob_dict.contains(depth) ? py::dict(rob_dict[depth]) : py::dict();
		depth_dict[py::str(item.first)] = info_dict;
		rob_dict[depth] = depth_dict;
	}
	return rob_dict;
}

// Shared docstrings
static const char *DOC_ADD_SAMPLE =
	"Append one sample for every declared signal.\n\n"
	"Args:\n"
	"    sample: [t, v1, v2, ...], values in signal declaration order.\n"
	"    interp: True (default) joins the sample to the previous one with a straight line;\n"
	"        False holds the previous value until t (piecewise-constant).\n\n"
	"A sample whose time is not after the last sample's time is ignored.\n\n"
	"Raises:\n"
	"    ValueError: if len(sample) is not 1 + the number of signals.";

static const char *DOC_SET_SIGNALS =
	"Replace all data with one Signal per declared signal, in declaration order.\n\n"
	"Raises:\n"
	"    ValueError: if the number of signals does not match the declaration.";

static const char *DOC_LOAD_CSV =
	"Load one CSV file per declared signal, in declaration order.\n\n"
	"Each row is 't,value' or 't,value,slope'.\n\n"
	"Raises:\n"
	"    ValueError: if the number of files does not match the declaration, or a file is missing or malformed.";

static const char *DOC_WRITE_CSV =
	"Write one '<signal name>.csv' per signal into an existing directory (rows 't,value,slope').";

static const char *DOC_SET_SEMANTICS =
	"Set the robustness semantics: 'SPACE' (default), 'LEFT_TIME', 'RIGHT_TIME' or 'BOOLEAN'.\n\n"
	"Raises:\n"
	"    ValueError: for any other name.";

PYBIND11_MODULE(_stlrom, m) {
	m.doc() = "STLRom: robust online monitoring of Signal Temporal Logic (compiled core).";

	//Class Point
	py::class_<STLRom::Point>(m, "Point", "A (time, value) pair.")
		.def(py::init<double,double>(), py::arg("time"), py::arg("value"))
		.def(py::init<const STLRom::Point&>(), py::arg("other"), "Copy constructor.")
		.def("print_point",&STLRom::Point::print_point, "Print the point.")
		.def_readwrite("time",&STLRom::Point::time)
		.def_readwrite("value",&STLRom::Point::value)
		.def("copy", [](const STLRom::Point &self) { return STLRom::Point(self); }, "Return a copy.")
		.def("__copy__", [](const STLRom::Point &self) { return STLRom::Point(self); })
		.def("__deepcopy__", [](const STLRom::Point &self, py::dict) { return STLRom::Point(self); }, py::arg("memo"))
		.def("__del__", [](STLRom::Point &self) {});

	//Class Sample
	py::class_<STLRom::Sample>(m, "Sample",
		"One piece of a piecewise-linear Signal: from `time`, the value is value + derivative * (t - time).")
		.def(py::init<double,double,double>(), py::arg("time"), py::arg("value"), py::arg("derivative"))
		.def("value_at", &STLRom::Sample::valueAt, py::arg("t"), "Value of this sample's line at time t.")
		.def("time_intersect", &STLRom::Sample::timeIntersect, py::arg("other"),
			"Time at which this sample's line meets the other sample's line.")
		.def("print_sample",&STLRom::Sample::print_sample, "Print the sample.")
		.def_readwrite("time",&STLRom::Sample::time)
		.def_readwrite("value",&STLRom::Sample::value)
		.def_readwrite("derivative",&STLRom::Sample::derivative, "Slope of the piece starting at this sample.")
		.def("copy", [](const STLRom::Sample &self) { return STLRom::Sample(self); }, "Return a copy.")
		.def("__copy__", [](const STLRom::Sample &self) { return STLRom::Sample(self); })
		.def("__deepcopy__", [](const STLRom::Sample &self, py::dict) { return STLRom::Sample(self); }, py::arg("memo"))
		.def("__del__", [](STLRom::Sample &self) {});

	//Class Signal
	py::class_<STLRom::Signal>(m, "Signal",
		"A piecewise-linear signal: a sequence of samples (time, value, slope).\n\n"
		"Between two samples the value follows the first sample's slope, so a jump happens where the\n"
		"next sample's value differs. Robustness results are Signals.")
		.def(py::init<>(), "Create an empty signal.")
		.def("__str__", [](const Signal &sig) {
            std::ostringstream oss;
            oss << sig;
            return oss.str();
        })
		.def("append_sample", (void (STLRom::Signal::*)(double, double)) &STLRom::Signal::appendSample,
			py::arg("t"), py::arg("v"),
			"Append a sample with slope 0 (the value is held until the next sample).\n\n"
			"Ignored if t is not after the last sample's time.")
		.def("append_sample", (void (STLRom::Signal::*)(double, double, double)) &STLRom::Signal::appendSample,
			py::arg("t"), py::arg("v"), py::arg("slope"),
			"Append a sample with an explicit slope.\n\n"
			"Ignored if t is not after the last sample's time.")
		.def("append_sample", (void (STLRom::Signal::*)(double, double, double, bool)) &STLRom::Signal::appendSample,
			py::arg("t"), py::arg("v"), py::arg("slope"), py::arg("interp"),
			"Append a sample. With interp=True, the previous sample's slope is replaced so that the two\n"
			"are joined by a straight line.\n\n"
			"Ignored if t is not after the last sample's time.")
		.def("append_constant_sample", (void (STLRom::Signal::*)(double, double)) &STLRom::Signal::appendConstantSample,
			py::arg("t"), py::arg("v"),
			"Append a sample; the previous value is held until t.")
		.def("append_linear_sample", (void (STLRom::Signal::*)(double, double)) &STLRom::Signal::appendLinearSample,
			py::arg("t"), py::arg("v"),
			"Append a sample joined to the previous one by a straight line.")
		.def("append_linear_sample", (void (STLRom::Signal::*)(double, double, double)) &STLRom::Signal::appendLinearSample,
			py::arg("t"), py::arg("v"), py::arg("slope"),
			"Append a sample joined to the previous one by a straight line; `slope` is the slope after t.")
		.def("value_at", &STLRom::Signal::valueAt, py::arg("t"),
			"Value at time t. Before the first sample, the first piece is extended backwards.\n"
			"An empty signal prints a message and returns 0.")
		.def("read_from_file", &STLRom::Signal::read_from_file, py::arg("filename"),
			"Replace the samples with those of a CSV file with rows 't,value' or 't,value,slope'.\n\n"
			"Raises:\n"
			"    ValueError: if the file is missing or malformed.")
		.def("write_to_file", &STLRom::Signal::write_to_file, py::arg("filename"),
			"Write the samples to a CSV file with rows 't,value,slope'.")
		.def("resize",(void (STLRom::Signal::*)(double,double)) &STLRom::Signal::resize,
			py::arg("t_start"), py::arg("t_end"),
			"Restrict or extend the signal to [t_start, t_end], holding the end values where it is extended.")
		.def("shift",(void (STLRom::Signal::*)(double)) &STLRom::Signal::shift, py::arg("delta"),
			"Move the signal in time by delta.")
		.def("reverse",(void (STLRom::Signal::*)()) &STLRom::Signal::reverse,
			"Reverse the signal in time over [begin_time, end_time].")
		.def("compute_not",&STLRom::Signal::compute_not, py::arg("s"),
			"Replace this signal with -s.")
		.def("compute_and", &STLRom::Signal::compute_and, py::arg("a"), py::arg("b"),
			"Replace this signal with min(a, b).")
		.def("compute_or", &STLRom::Signal::compute_or, py::arg("a"), py::arg("b"),
			"Replace this signal with max(a, b).")
		.def("compute_boolean",&STLRom::Signal::compute_boolean, py::arg("s"),
			"Replace this signal with 1 where s > 0 and 0 elsewhere.")
		.def("compute_left_time_rob",&STLRom::Signal::compute_left_time_rob, py::arg("s"),
			"Replace this signal with the left time robustness of s (how long s has kept its sign).")
		.def("compute_right_time_rob",&STLRom::Signal::compute_right_time_rob, py::arg("s"),
			"Replace this signal with the right time robustness of s (how long s will keep its sign).")
		.def("get_samples_list",&STLRom::Signal::getSamplesDeque, "Return the samples as a list.")
		.def_readwrite("begin_time",&STLRom::Signal::beginTime, "Start of the time range the signal covers.")
		.def_readwrite("end_time",&STLRom::Signal::endTime,
			"End of the time range the signal covers (can be after the last sample).")
		.def_static("set_semantics",  &STLRom::Signal::set_semantics, py::arg("semantics"),
			"Set the global semantics. Drivers and monitors overwrite it with their own setting at each\n"
			"computation: use STLDriver.set_semantics instead.")
		.def_static("get_semantics",  &STLRom::Signal::get_semantics, "Return the global semantics.")
		.def_static("set_BigM",&STLRom::Signal::set_BigM, py::arg("value"),
			"Set the value that stands for +infinity in robustness and online bounds (default 10000).")
		.def_static("get_BigM",&STLRom::Signal::get_BigM,
			"Return BigM, rounded down to an integer.")
		.def_static("set_Eps",&STLRom::Signal::set_Eps, py::arg("value"),
			"Set the margin used where a signal crosses an atom's threshold (default 1e-10).")
		.def_static("get_Eps",&STLRom::Signal::get_Eps, "Return Eps.")
		.def("empty",&STLRom::Signal::empty, "True if the signal has no samples.")
		.def("copy", [](const STLRom::Signal &self) { return STLRom::Signal(self); }, "Return a copy.")
		.def("__copy__", [](const STLRom::Signal &self) { return STLRom::Signal(self); })
		.def("__deepcopy__", [](const STLRom::Signal &self, py::dict) { return STLRom::Signal(self); }, py::arg("memo"))
		.def("__del__", [](STLRom::Signal &self) {});


	// Class STLData
	py::class_<STLRom::STLData>(m, "STLData",
		"Declared signals and their samples (STLDriver.data, STLMonitor.data).")
		.def(py::init<>(), "Create empty data with no signals.")
		.def(py::init<int>(), py::arg("nb_signals"), "Create data with nb_signals empty signals named x0, x1, ...")
		.def(py::init<trace_data>(), py::arg("signals"), "Create data from a list of Signals, named x0, x1, ...")
		.def("__str__", [](const STLData &dd) {
            std::ostringstream oss;
            oss << dd;
            return oss.str();
        })
		.def("get_signal",(Signal (STLRom::STLData::*)(int) const) &STLRom::STLData::get_signal, py::arg("index"),
			"Return a copy of the signal at this index.")
		.def("get_signal",(Signal (STLRom::STLData::*)(const std::string &) const) &STLRom::STLData::get_signal, py::arg("name"),
			"Return a copy of the signal with this name.")
		.def("get_signal_idx", (int (STLRom::STLData::*)(const string &) const) &STLRom::STLData::get_signal_idx, py::arg("name"),
			"Index of a signal, or -1 if there is no signal with this name.")
		.def("get_signame_from_idx", (string (STLRom::STLData::*)(int) const) &STLRom::STLData::get_signame_from_idx, py::arg("index"),
			"Name of the signal at this index.")
		.def("add_sample", (void (STLRom::STLData::*)(vector<double>, bool)) &STLRom::STLData::add_sample,
			py::arg("sample"), py::arg("interp") = true, DOC_ADD_SAMPLE)
		.def("add_signal_sample", (void (STLRom::STLData::*)(string, double, double)) &STLRom::STLData::add_signal_sample,
			py::arg("name"), py::arg("t"), py::arg("v"),
			"Append a sample to one signal only. Unknown names are ignored.")
		.def("add_signal_sample", (void (STLRom::STLData::*)(string, double, double, double)) &STLRom::STLData::add_signal_sample,
			py::arg("name"), py::arg("t"), py::arg("v"), py::arg("slope"),
			"Append a sample with an explicit slope to one signal only. Unknown names are ignored.")
		.def("add_signal_sample", (void (STLRom::STLData::*)(string, double, double, double, bool)) &STLRom::STLData::add_signal_sample,
			py::arg("name"), py::arg("t"), py::arg("v"), py::arg("slope"), py::arg("interp"),
			"Append a sample to one signal only; interp=True joins it to the previous one by a line.\n"
			"Unknown names are ignored.")
		.def_readwrite("data_vector", &STLRom::STLData::data_vector, "The signals, in index order.")
		.def_readwrite("signal_map", &STLRom::STLData::signal_map, "{signal name: index}.")
		;

	m.def("read_point",&read_point,"Internal test function: read a point from stdin and print it.");
//	m.def("rand_trace_data",&rand_trace_data,"function generating random traces");

	//Class transducer
	py::class_<STLRom::transducer>(m, "transducer",
		"Internal: a node of a formula tree (one operator). Not part of the supported API.")
		.def(py::init<>())
		.def("__str__", [](const transducer &transducer) {
            std::ostringstream oss;
            oss << transducer;
            return oss.str();
        })
		.def("get_child",&STLRom::transducer::get_child, py::return_value_policy::reference_internal,
			"Child of a unary operator, or None.")
		.def("get_childL",&STLRom::transducer::get_childL, py::return_value_policy::reference_internal,
			"Left child of a binary operator, or None.")
		.def("get_childR",&STLRom::transducer::get_childR, py::return_value_policy::reference_internal,
			"Right child of a binary operator, or None.")
		.def("get_formula_string",&STLRom::transducer::get_formula_string, "The formula as text.")
		.def_readwrite("z",&STLRom::transducer::z, "Robustness from the last computation.")
		.def_readwrite("z_low",&STLRom::transducer::z_low, "Lower bound from the last online computation.")
		.def_readwrite("z_up",&STLRom::transducer::z_up, "Upper bound from the last online computation.")
		;

	//Class STLMonitor
	py::class_<STLRom::STLMonitor>(m, "STLMonitor",
		"One formula with its own copy of the data. Create it with STLDriver.get_monitor(name).\n\n"
		"Samples added to the monitor do not reach the driver, and the other way round.")
		.def(py::init<>(), "Create an empty monitor (no formula). Use STLDriver.get_monitor instead.")
		.def("__str__", [](const STLMonitor &monitor) {
            std::ostringstream oss;
            oss << monitor;
            return oss.str();
        })
		.def("add_sample", (void (STLRom::STLMonitor::*)(vector <double>, bool)) &STLRom::STLMonitor::add_sample,
			py::arg("sample"), py::arg("interp") = true, DOC_ADD_SAMPLE)
		.def("set_signals",&STLRom::STLMonitor::set_signals, py::arg("signals"), DOC_SET_SIGNALS)
		.def("load_csv",&STLRom::STLMonitor::load_csv, py::arg("files"), DOC_LOAD_CSV)
		.def("write_csv",&STLRom::STLMonitor::write_csv, py::arg("directory"), DOC_WRITE_CSV)
		.def("get_lower_rob",&STLRom::STLMonitor::get_lower_rob,
			"Lower bound at t_start from the last online computation.")
		.def("get_upper_rob",&STLRom::STLMonitor::get_upper_rob,
			"Upper bound at t_start from the last online computation.")
		.def("set_semantics",&STLRom::STLMonitor::set_semantics, py::arg("semantics"), DOC_SET_SEMANTICS)
		.def("get_semantics",&STLRom::STLMonitor::get_semantics, "Return the semantics name.")
		.def("get_rob",
			[](STLRom::STLMonitor& self, double t0, double dt) { return self.get_rob(t0, dt); },
			py::arg("t0") = 0., py::arg("dt") = INF,
			"Robustness of the formula at time t0.\n\n"
			"Args:\n"
			"    t0: evaluation time.\n"
			"    dt: largest time step used inside products of signals; inf uses the data points only.\n\n"
			"Raises:\n"
			"    ValueError: if the data does not cover the formula's time windows at t0, or dt <= 0.\n"
			"    RuntimeError: if the monitor has no formula.")
		.def("get_online_rob",
			[](STLRom::STLMonitor& self, double t0, double dt) { return self.get_online_rob(t0, dt); },
			py::arg("t0") = 0., py::arg("dt") = INF,
			"Online robustness at time t0 from the data so far.\n\n"
			"Returns:\n"
			"    [rob, lower, upper]: the robustness computed so far and bounds that hold for any future data.\n"
			"    With no data at all: [nan, -BigM, BigM].\n\n"
			"Raises:\n"
			"    ValueError: if dt <= 0.\n"
			"    RuntimeError: if the monitor has no formula.")
		.def("get_rob_signal",(Signal (STLRom::STLMonitor::*)()) &STLRom::STLMonitor::get_rob_signal,
			"Robustness over the range set by set_eval_time (default [0, 0]).")
		.def("get_rob_signal",(Signal (STLRom::STLMonitor::*)(double)) &STLRom::STLMonitor::get_rob_signal,
			py::arg("t"), "Robustness at time t, as a one-sample Signal.")
		.def("get_rob_signal",(Signal (STLRom::STLMonitor::*)(double,double)) &STLRom::STLMonitor::get_rob_signal,
			py::arg("t_start"), py::arg("t_end"),
			"Robustness over [t_start, t_end], cut to the part covered by the data (empty if none).")
		.def("get_rob_signal",(Signal (STLRom::STLMonitor::*)(double,double,double)) &STLRom::STLMonitor::get_rob_signal,
			py::arg("t_start"), py::arg("t_end"), py::arg("dt"),
			"Robustness over [t_start, t_end], cut to the part covered by the data (empty if none).\n\n"
			"dt: largest time step used inside products of signals; must be > 0.")
		.def("get_online_rob_signal",(vector<Signal> (STLRom::STLMonitor::*)()) &STLRom::STLMonitor::get_online_rob_signal,
			"[z, z_low, z_up] over the range set by set_eval_time: robustness so far, lower and upper bounds.")
		.def("get_online_rob_signal",(vector<Signal> (STLRom::STLMonitor::*)(double)) &STLRom::STLMonitor::get_online_rob_signal,
			py::arg("t"), "[z, z_low, z_up] at time t: robustness so far, lower and upper bounds.")
		.def("get_online_rob_signal",(vector<Signal> (STLRom::STLMonitor::*)(double,double)) &STLRom::STLMonitor::get_online_rob_signal,
			py::arg("t_start"), py::arg("t_end"),
			"[z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.")
		.def("get_online_rob_signal",(vector<Signal> (STLRom::STLMonitor::*)(double,double,double)) &STLRom::STLMonitor::get_online_rob_signal,
			py::arg("t_start"), py::arg("t_end"), py::arg("dt"),
			"[z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.\n\n"
			"dt: largest time step used inside products of signals; must be > 0.")
		.def("get_robustness_map",
			[](STLRom::STLMonitor& self) { return rob_map_to_dict(self.get_robustness_map()); },
			"Robustness of every subformula from the last computation:\n"
			"{depth: {subformula: {'z': Signal, 'z_low': Signal, 'z_up': Signal}}}, the root at depth 0.")
		.def("get_online_robustness_map",
			[](STLRom::STLMonitor& self) { return rob_map_to_dict(self.get_online_robustness_map()); },
			"Same as get_robustness_map, with the online bounds from the last online computation.")
		.def("set_eval_time",&STLRom::STLMonitor::set_eval_time, py::arg("t_start"), py::arg("t_end"),
			"Set the default range used by get_rob_signal() and get_online_rob_signal() with no arguments.")
		.def("set_param",&STLRom::STLMonitor::set_param, py::arg("name"), py::arg("value"),
			"Change the value of a parameter for this monitor.")
		.def("get_param",&STLRom::STLMonitor::get_param, py::arg("name"),
			"Return the value of a parameter.")
 		.def("display_formula",&STLRom::STLMonitor::display_formula, "Print the formula.")
		.def("copy", [](const STLRom::STLMonitor &self) { return STLRom::STLMonitor(self); }, "Return an independent copy.")
		.def("__copy__", [](const STLRom::STLMonitor &self) { return STLRom::STLMonitor(self); })
		.def("__deepcopy__", [](const STLRom::STLMonitor &self, py::dict) { return STLRom::STLMonitor(self); }, py::arg("memo"))
		.def("__del__", [](STLRom::STLMonitor *instance) { delete instance; }) // Bind the destructor
		.def("get_signals_names",&STLRom::STLMonitor::get_signals_names,
			"The declared signal names, space-separated.")
		.def_readwrite("rob",&STLRom::STLMonitor::rob, "Robustness at t_start from the last computation.")
		.def_readwrite("lower_rob",&STLRom::STLMonitor::lower_rob, "Lower bound at t_start from the last online computation.")
		.def_readwrite("upper_rob",&STLRom::STLMonitor::upper_rob, "Upper bound at t_start from the last online computation.")
		.def_readwrite("up_to_date",&STLRom::STLMonitor::up_to_date, "Internal.")
		.def_readwrite("formula",&STLRom::STLMonitor::formula, "Internal: the root of the formula tree.")
		.def_readwrite("data",&STLRom::STLMonitor::data, "The monitor's data (STLData).")
		.def_readwrite("start_time",&STLRom::STLMonitor::start_time, "Start of the last requested range.")
		.def_readwrite("end_time",&STLRom::STLMonitor::end_time, "End of the last requested range.")
		.def_readwrite("param_map",&STLRom::STLMonitor::param_map, "Parameter values, {name: value}.")
		;

	//Class STLDriver
	py::class_<STLRom::STLDriver>(m, "STLDriver",
		"Parses STL specifications, holds the formulas, the parameters and one data set, and computes robustness.\n\n"
		"Example:\n"
		"    driver = STLDriver()\n"
		"    driver.parse_string('signal x\\nphi := ev_[0, 5] x[t] > 3')\n"
		"    driver.add_sample([0, 1.0])\n"
		"    rob, lower, upper = driver.get_online_rob('phi')")
		.def(py::init<>(), "Create an empty driver.")
		.def("__str__", [](const STLDriver &dd) {
            std::ostringstream oss;
            oss << dd;
            return oss.str();
        })
		.def("parse_file", [](STLRom::STLDriver &self, const std::string &filename, bool verbose) {
				return self.parse_file(filename, verbose);
			},
			py::arg("filename"), py::arg("verbose") = false,
			"Parse a specification file. With verbose=True, also print the file and each definition.\n\n"
			"Returns:\n"
			"    False if the file cannot be opened or has a syntax error (the error is printed to stderr).")
		.def("parse_string",&STLRom::STLDriver::parse_string, py::arg("text"),
			"Parse a specification. Can be called several times to add definitions.\n\n"
			"Returns:\n"
			"    False on a syntax error (the error is printed to stderr).")
		.def("disp",&STLRom::STLDriver::disp, "Print the declarations, formulas and a summary of the data.")
		.def("add_sample", (void (STLRom::STLDriver::*)(vector <double>, bool)) &STLRom::STLDriver::add_sample,
			py::arg("sample"), py::arg("interp") = true, DOC_ADD_SAMPLE)
		.def("set_signals",&STLRom::STLDriver::set_signals, py::arg("signals"), DOC_SET_SIGNALS)
		.def("load_csv",&STLRom::STLDriver::load_csv, py::arg("files"), DOC_LOAD_CSV)
		.def("write_csv",&STLRom::STLDriver::write_csv, py::arg("directory"), DOC_WRITE_CSV)
		.def("get_monitor",&STLRom::STLDriver::get_monitor, py::arg("name"),
			"Return a monitor for one formula, with a copy of the current data, parameters and semantics.\n\n"
			"Raises:\n"
			"    ValueError: if no formula has this name.")
		.def("get_signals_names",&STLRom::STLDriver::get_signals_names,
			"The declared signal names, space-separated.")
		.def("set_semantics",&STLRom::STLDriver::set_semantics, py::arg("semantics"), DOC_SET_SEMANTICS)
		.def("get_semantics",&STLRom::STLDriver::get_semantics, "Return the semantics name.")
		.def("get_rob",
			[](STLRom::STLDriver& self,  const string &phi_in, double t0, double dt) {
				return self.get_rob(phi_in, t0, dt);
			},
			py::arg("phi_in") = "phi", py::arg("t0") = 0., py::arg("dt") = INF,
			"Robustness of a formula at time t0.\n\n"
			"Args:\n"
			"    phi_in: formula name.\n"
			"    t0: evaluation time.\n"
			"    dt: largest time step used inside products of signals; inf uses the data points only.\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined, the data does not cover its time windows at t0,\n"
			"        or dt <= 0.")
		.def("get_online_rob",
			[](STLRom::STLDriver& self,  const string &phi_in, double t0, double dt) {
				return self.get_online_rob(phi_in, t0, dt);
			},
			py::arg("phi_in") = "phi", py::arg("t0") = 0., py::arg("dt") = INF,
			"Online robustness of a formula at time t0 from the data so far.\n\n"
			"Returns:\n"
			"    [rob, lower, upper]: the robustness computed so far and bounds that hold for any future data.\n"
			"    With no data at all: [nan, -BigM, BigM].\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined or dt <= 0.")
		.def("get_rob_signal",
			(Signal (STLRom::STLDriver::*)(const std::string &)) &STLRom::STLDriver::get_rob_signal,
			py::arg("phi_in"),
			"Robustness of a formula at time 0, as a one-sample Signal.\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined.")
		.def("get_rob_signal",
			(Signal (STLRom::STLDriver::*)(const std::string &, double)) &STLRom::STLDriver::get_rob_signal,
			py::arg("phi_in"), py::arg("t"),
			"Robustness of a formula at time t, as a one-sample Signal.")
		.def("get_rob_signal",
			(Signal (STLRom::STLDriver::*)(const std::string &, double, double)) &STLRom::STLDriver::get_rob_signal,
			py::arg("phi_in"), py::arg("t_start"), py::arg("t_end"),
			"Robustness of a formula over [t_start, t_end], cut to the part covered by the data (empty if none).")
		.def("get_rob_signal",
			(Signal (STLRom::STLDriver::*)(const std::string &, double, double, double)) &STLRom::STLDriver::get_rob_signal,
			py::arg("phi_in"), py::arg("t_start"), py::arg("t_end"), py::arg("dt"),
			"Robustness of a formula over [t_start, t_end], cut to the part covered by the data (empty if none).\n\n"
			"dt: largest time step used inside products of signals; must be > 0.")
		.def("get_online_rob_signal",
			(vector<Signal> (STLRom::STLDriver::*)(const std::string &)) &STLRom::STLDriver::get_online_rob_signal,
			py::arg("phi_in"),
			"[z, z_low, z_up] at time 0: robustness so far, lower and upper bounds.\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined.")
		.def("get_online_rob_signal",
			(vector<Signal> (STLRom::STLDriver::*)(const std::string &, double)) &STLRom::STLDriver::get_online_rob_signal,
			py::arg("phi_in"), py::arg("t"),
			"[z, z_low, z_up] at time t: robustness so far, lower and upper bounds.")
		.def("get_online_rob_signal",
			(vector<Signal> (STLRom::STLDriver::*)(const std::string &, double, double)) &STLRom::STLDriver::get_online_rob_signal,
			py::arg("phi_in"), py::arg("t_start"), py::arg("t_end"),
			"[z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.")
		.def("get_online_rob_signal",
			(vector<Signal> (STLRom::STLDriver::*)(const std::string &, double, double, double)) &STLRom::STLDriver::get_online_rob_signal,
			py::arg("phi_in"), py::arg("t_start"), py::arg("t_end"), py::arg("dt"),
			"[z, z_low, z_up] over [t_start, t_end]: robustness so far, lower and upper bounds.\n\n"
			"dt: largest time step used inside products of signals; must be > 0.")
		.def("get_robustness_map",
			[](STLRom::STLDriver& self, const string &phi_in) { return rob_map_to_dict(self.get_robustness_map(phi_in)); },
			py::arg("phi_in") = "phi",
			"Robustness of every subformula from the last computation of the formula:\n"
			"{depth: {subformula: {'z': Signal, 'z_low': Signal, 'z_up': Signal}}}, the root at depth 0.\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined.")
		.def("get_online_robustness_map",
			[](STLRom::STLDriver& self, const string &phi_in) { return rob_map_to_dict(self.get_online_robustness_map(phi_in)); },
			py::arg("phi_in") = "phi",
			"Same as get_robustness_map, with the online bounds from the last online computation.\n\n"
			"Raises:\n"
			"    ValueError: if the formula is not defined.")
		.def("get_param",&STLRom::STLDriver::get_param, py::arg("name"), "Return the value of a parameter.")
		.def("set_param",&STLRom::STLDriver::set_param, py::arg("name"), py::arg("value"),
			"Change the value of a parameter; applies to the next computation.")
		.def_readwrite("data",&STLRom::STLDriver::data, "The driver's data (STLData).")
		.def("copy", [](const STLRom::STLDriver &self) { return STLRom::STLDriver(self); }, "Return an independent copy.")
		.def("__copy__", [](const STLRom::STLDriver &self) { return STLRom::STLDriver(self); })
		.def("__deepcopy__", [](const STLRom::STLDriver &self, py::dict) { return STLRom::STLDriver(self); }, py::arg("memo"))
		.def("__del__", [](STLRom::STLDriver *instance) {delete instance; }); // Bind the destructor


	m.def("print_monitor",&print_monitor, py::arg("driver"), "Internal test function: print the monitor of formula 'phi'.");
}
