#include <transducer.h>
#include <string>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "tools.h"
#include <exception>
#include "signal_expr.h"
#include <cmath>

using namespace std;

namespace STLRom {

/* Unary operators on signals */

double formula_signal_transducer::compute_robustness() {
	child->compute_robustness();

	auto iter = child->z.begin();
	
	for (; iter != child->z.end();
			iter++) {
		double t= (*iter).time;
		double v = (*iter).value;
		double d = (*iter).derivative;
		z.appendSample(t, v, d);
	}

	return z.front().value;
}

double shifted_transducer::compute_robustness() {
	child->compute_robustness();

	auto iter = child->z.begin();
	
	for (; iter != child->z.end();
			iter++) {
		double t= (*iter).time;
		double v = (*iter).value;
		double d = (*iter).derivative;
		z.appendSample(t-_shift, v, d);
	}
	
	z.resize(start_time, end_time);

	return z.front().value;
}

double abs_transducer::compute_robustness() {

	// update child robustness
	child->compute_robustness();

	auto iter = child->z.begin();

	double t_cross = std::numeric_limits<double>::infinity();
	double d = 0;
	
	for (; iter != child->z.end();
			iter++) {

		double t= (*iter).time;
		double v = (*iter).value;

		if (t_cross < t - ZERO_POS) {
			z.appendSample(t_cross, 0, fabs(d));
		}

		d = (*iter).derivative;
		double abs_d = (v > 0) ? d : (v < 0) ? -d : fabs(d);
		
		if (abs_d < 0)
			t_cross = t - fabs(v)/abs_d;
		else
			t_cross = std::numeric_limits<double>::infinity();

		if (t_cross <= t + ZERO_POS) {
			abs_d = fabs(d);
			t_cross = std::numeric_limits<double>::infinity();
		}		
			z.appendSample(t, fabs(v), abs_d);
	}

	if (t_cross < child->z.endTime) {
		z.appendSample(t_cross, 0, fabs(d));
	}

	return z.front().value;
}

double unary_minus_transducer::compute_robustness() {

	// update child robustness
	child->compute_robustness();

	auto iter = child->z.begin();
	
	for (; iter != child->z.end();
			iter++) {
		double t= (*iter).time;
		double v = (*iter).value;
		double d = (*iter).derivative;
		z.appendSample(t, -v, -d);
	}
	return z.front().value;
}

/* Binary signal operators - all operators assume that left and right operand have the
 * same number of samples.
 */
double plus_transducer::compute_robustness() {

	// update children robustness
	childL->compute_robustness();
	childR->compute_robustness();

	merge_signals_with_op(z, childL->z, childR->z, [](double a, double b){return a + b;}, [](double vL, double vR, double dL, double dR){return dL + dR;}, false);

	// auto iterL = childL->z.begin();
	// auto iterR = childR->z.begin();

	// for (; iterL != childL->z.end();
	// 		iterL++, iterR++) {
	// 	double t= (*iterL).time;
	// 	double vL = (*iterL).value;
	// 	double vR = (*iterR).value;
	// 	z.appendSample(t, vL+vR);
	// }
	z.simplify();
	return z.front().value;
}

double minus_transducer::compute_robustness() {

	childL->compute_robustness();
	childR->compute_robustness();

	merge_signals_with_op(z, childL->z, childR->z, [](double a, double b){return a - b;}, [](double vL, double vR, double dL, double dR){return dL - dR;}, false);
	
	// auto iterL = childL->z.begin();
	// auto iterR = childR->z.begin();

	// for (; iterL != childL->z.end();
	// 		iterL++, iterR++) {
	// 	double t= (*iterL).time;
	// 	double vL = (*iterL).value;
	// 	double vR = (*iterR).value;
	// 	z.appendSample(t, vL-vR);
	// }
	z.simplify();
	return z.front().value;
}

double mult_transducer::compute_robustness() {

	childL->compute_robustness();
	childR->compute_robustness();

	merge_signals_with_op(z, childL->z, childR->z, [](double a, double b){return a * b;}, [](double vL, double vR, double dL, double dR){return dL*vR + dR*vL;}, false);
	// auto iterL = childL->z.begin();
	// auto iterR = childR->z.begin();

	// for (; iterL != childL->z.end();
	// 		iterL++, iterR++) {
	// 	double t= (*iterL).time;
	// 	double vL = (*iterL).value;
	// 	double vR = (*iterR).value;
	// 	z.appendSample(t, vL*vR);
	// }
	z.simplify();
	return z.front().value;
}

}
