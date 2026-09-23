#include <transducer.h>
#include <algorithm>

namespace STLRom {

    /* init horizon */
    void transducer::init_horizon() {
    }

    void unary_transducer::init_horizon() {
        child->set_horizon(start_time, end_time);
        child->init_horizon();
    }

    void binary_transducer::init_horizon() {
        childL->set_horizon(start_time, end_time);
        childR->set_horizon(start_time, end_time);
        childL->init_horizon();
        childR->init_horizon();
    }

    void timed_unary_transducer::init_horizon() {

        // checks whether a and b are given by parameters, and assign corresponding values
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        // update start_time and end_time of child
        child->set_horizon(fmax(0, start_time+a),end_time+b);
        child->init_horizon();

    }

    void past_timed_unary_transducer::init_horizon() {

        // checks whether a and b are given by parameters, and assign corresponding values
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        // update start_time and end_time of child
        // TODO: what happens when start_time-b is negative?
        child->set_horizon(start_time-b, end_time-a);
        child->init_horizon();

    }

    void timed_binary_transducer::init_horizon() {

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        // update start_time and end_time of children
        childL->set_horizon(start_time, end_time+b);
        childR->set_horizon(fmax(0, start_time+a), end_time+b);
        childL->init_horizon();
        childR->init_horizon();
    }

    /* set param */
    void transducer::set_param(const string &param, double val) {
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
    
    }

    void unary_transducer::set_param(const string &param, double val) {        
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
        child->set_param(param, val);
    }

    void binary_transducer::set_param(const string &param, double val) {        
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
        childL->set_param(param, val);
        childR->set_param(param, val);
    }

    /* reset */
    void transducer::reset() {        
        z.clear();z.beginTime=0.; z.endTime=0.;
        z_low.clear();z_low.beginTime=0.; z_low.endTime=0.;
        z_up.clear();z_up.beginTime=0.; z_up.endTime=0.;
        
    }

    void unary_transducer::reset() {        
        z.clear();z.beginTime=0.; z.endTime=0.;
        z_low.clear();z_low.beginTime=0.; z_low.endTime=0.;
        z_up.clear();z_up.beginTime=0.; z_up.endTime=0.;
        
        child->reset();
    }

    void binary_transducer::reset() {        
        z.clear();z.beginTime=0.; z.endTime=0.;
        z_low.clear();z_low.beginTime=0.; z_low.endTime=0.;
        z_up.clear();z_up.beginTime=0.; z_up.endTime=0.;
        
        childL->reset();
        childR->reset();
    }


    void timed_unary_transducer::set_param(const string &param, double val) {
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
        child->set_param(param, val);
    }

    void past_timed_unary_transducer::set_param(const string &param, double val) {
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
        child->set_param(param, val);
    }

    void timed_binary_transducer::set_param(const string &param, double val) {
    	if (param_map.find(param)!=param_map.end()){
		param_map[param]=val;
	}
        childL->set_param(param, val);
        childR->set_param(param, val);
    }

    // get end time complete for transducers
    double transducer::get_end_complete() {
        return z.endTime;
    }
    double transducer::get_end_complete_low() {
        return z_low.endTime;
    }
    double transducer::get_end_complete_up() {
        return z_up.endTime;
    }

    // get end time complete for timed transducers
    double timed_unary_transducer::get_end_complete() {
        double b;
        if (!get_param(I->end_str,b)) b = I->end;
        return child->z.endTime-b;
    }

    double timed_unary_transducer::get_end_complete_low() {
        double b;
        if (!get_param(I->end_str,b)) b = I->end;	
        return child->z_low.endTime-b;
    }

    double timed_unary_transducer::get_end_complete_up() {
        double b;
        if (!get_param(I->end_str,b)) b = I->end;
        return child->z_up.endTime-b;
    }


    /* Compute robustness */
    double not_transducer::compute_robustness() {
        //	cout << "Computing robustness of NOT"
        child->compute_robustness();
        z.compute_not(child->z);
        return z.front().value;
    }

    void not_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void not_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_online_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double ev_transducer::compute_robustness() {

        #ifdef DEBUG__
        printf(">  ev_transducer::compute_robustness:         IN." );
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
        #endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_robustness();        
        z.compute_timed_eventually(child->z, a, b);
        double et = min(z.endTime,end_time);
        z.resize(start_time,max(start_time,et), 0.);

#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        cout << "<  ev_transducer::compute_robustness:         OUT." << endl;
#endif
        return z.front().value;
    }

    void ev_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void ev_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_online_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    void once_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void once_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_online_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double once_transducer::compute_robustness() {

        #ifdef DEBUG__
        printf(">  once_transducer::compute_robustness:         IN." );
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
        #endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_robustness();
        Signal child_z = child->z; // is this a copy?
        child_z.reverse();
        child_z.shift(b+a);
        z.compute_timed_eventually(child_z, a, b);
        double et = min(z.endTime,end_time);
        z.resize(start_time,max(start_time,et));
        z.reverse();
#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        cout << "<  once_transducer::compute_robustness:         OUT." << endl;
#endif
        return z.front().value;
    }

    double hist_transducer::compute_robustness() {

        #ifdef DEBUG__
        printf(">  hist_transducer::compute_robustness:         IN." );
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
        #endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_robustness();
        Signal child_z = child->z; // is this a copy?
        child_z.reverse();
        child_z.shift(b+a);
        z.compute_timed_globally(child_z, a, b);
        double et =min(z.endTime,end_time);
        z.resize(start_time,max(start_time,et));
        z.reverse();
#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        cout << "<  hist_transducer::compute_robustness:         OUT." << endl;
#endif
        return z.front().value;
    }

    double alw_transducer::compute_robustness() {
#ifdef DEBUG__
        printf(">  alw_transducer::compute_robustness:        IN." );
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;
        
        child->compute_robustness();
        z.compute_timed_globally(child->z, a, b);
        double et =min(z.endTime,end_time);
        z.resize(start_time,max(start_time, et ),0.);

#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        printf("<  alw_transducer::compute_robustness:        OUT." );
#endif
        return z.front().value;
    }

    void alw_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void alw_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_online_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    void hist_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void hist_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        child->fill_online_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double and_transducer::compute_robustness() {
#ifdef DEBUG__
        printf( ">  and_transducer::compute_robustness:        IN." );
#endif

        childL->compute_robustness();
        childR->compute_robustness();

        Signal z1= childL->z;
        Signal z2= childR->z;

        double et = max(z1.endTime, z2.endTime);
        z1.endTime = et;
        z2.endTime = et;
        z.compute_and(z1, z2);

        //	z.resize(start_time,max(start_time, z.endTime),0.);
#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        printf("<  and_transducer::compute_robustness:        OUT." );
#endif
        return z.front().value;
    }

    void and_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_robustness_map(rob_map, depth+1);
        childR->fill_robustness_map(rob_map, depth+1);

        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void and_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_online_robustness_map(rob_map, depth+1);
        childR->fill_online_robustness_map(rob_map, depth+1);
        
        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double or_transducer::compute_robustness() {
#ifdef DEBUG__
        printf( ">  or_transducer::compute_robustness:         IN." );
#endif

        childL->compute_robustness();
        childR->compute_robustness();

        Signal z1= childL->z;
        Signal z2= childR->z;

        double et = max(z1.endTime, z2.endTime);
        z1.endTime = et;
        z2.endTime = et;

        z.compute_or(z1,z2);

#ifdef DEBUG__
        cout << "OUT:" << z << endl;
        printf("<  or_transducer::compute_robustness:         OUT." );
#endif
        return z.front().value;
    }

    void or_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_robustness_map(rob_map, depth+1);
        childR->fill_robustness_map(rob_map, depth+1);


        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void or_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_online_robustness_map(rob_map, depth+1);
        childR->fill_online_robustness_map(rob_map, depth+1);

        
        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double implies_transducer::compute_robustness() {

        childL->compute_robustness();
        childR->compute_robustness();

        Signal z1= childL->z;
        Signal z2= childR->z;
        double et = max(z1.endTime, z2.endTime);

        z.compute_implies(z1,z2);

        return z.front().value;
    }

    void implies_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_robustness_map(rob_map, depth+1);
        childR->fill_robustness_map(rob_map, depth+1);


        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void implies_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_online_robustness_map(rob_map, depth+1);
        childR->fill_online_robustness_map(rob_map, depth+1);
        
        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double until_transducer::compute_robustness() {

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        // update robustness of children
        childL->compute_robustness();
        childR->compute_robustness();

        z.compute_timed_until(childL->z, childR->z, a, b);
        z.resize(0.,z.endTime,0.);
        return z.front().value;
    }

    void until_transducer::fill_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_robustness_map(rob_map, depth+1);
        childR->fill_robustness_map(rob_map, depth+1);


        rob_map[this->get_formula_string()] = robustness_info{depth, &z, nullptr, nullptr};
    }

    void until_transducer::fill_online_robustness_map(robustness_map_t &rob_map, int depth) {
        childL->fill_online_robustness_map(rob_map, depth+1);
        childR->fill_online_robustness_map(rob_map, depth+1);


        rob_map[this->get_formula_string()] = robustness_info{depth, &z, &z_up, &z_low};
    }

    double past_transducer::compute_robustness() {
        child-> compute_robustness();

        double T = child->z.beginTime + child->z.endTime;
        
        auto iter = child->z.rbegin();
        
        double d;
        
        if (iter != child->z.rend())
        {
            if (fabs(child->z.endTime - iter->time) < ZERO_POS) {
                z.appendSample(-child->z.endTime + T, (*iter).valueAt(child->z.endTime), -(*iter).derivative);	
            }

            for (; (iter+1) != child->z.rend(); iter++) {
                double t = (*iter).time;
                double v = (*iter).value;
                d = (iter+1)->derivative;
                z.appendSample(-t+T, v, -d);
            }

            if (iter != child->z.rend())
                z.appendSample(-iter->time+T, iter->value, -iter->derivative);
        }

        // z.resize(start_time, end_time);
        return z.front().value;
    }

    /* Utility functions */

    bool transducer::get_param(const string & param, double & val) {
        
        map<string, double>::const_iterator it;            
        if ((param_map_ptr!=NULL)&&( (it= param_map_ptr->find(param)) != param_map_ptr->end())) {            
            val = it->second;
            return true;
        }
        else{
            val = 0.;
            return false;
        }
    }

    std::ostream& operator<<(std::ostream& os, const transducer& T) {
        T.print(os);
        return os;
    }

    // I don't think we need that here
    //void transducer::print_trace() {
    //    for (auto ii = trace_data_ptr->begin(); ii != trace_data_ptr->end(); ii++){
    //        for (auto jj = (*ii).begin(); jj != (*ii).end(); jj++) {
    //            cout << *jj << " ";
    //        }
    //        cout << endl;
    //    }
    //}
    
}

