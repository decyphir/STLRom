#include <transducer.h>
#include <algorithm>
#include <math.h>
//#define DEBUG__

namespace STLRom {

    /* Interval robustness */

    // default (static): interval robustness is the same as normal robustness, with TOPs and BOTTOMs at
    // the ends 
    double transducer::compute_lower_rob(){
    #ifdef DEBUG__
        printf(">  transducer:compute_lower_rob              IN.\n");
        cout<< "start_time:" << start_time << " end_time:" << end_time << endl;
        cout << "last data time:" << get_last_data_time() << endl; 
    #endif
        
        // if for some reason, z was not computed before
        if (z.empty()) // is it the best test ?
            compute_robustness();

    #ifdef DEBUG__
        cout << "z :" << z << endl;
    #endif

        z_tube.lower_signal = Signal(z);
        double last_data_t =  get_last_data_time();
        
        if (end_time>last_data_t) 
        {   
            z_tube.lower_signal.resize(start_time, last_data_t, BOTTOM);
            z_tube.lower_signal.appendSample(last_data_t+Signal::Eps, BOTTOM, 0., false);
            z_tube.lower_signal.endTime = end_time;
        }
        
    #ifdef DEBUG__
        cout << "z_tube.lower_signal:" << z_tube.lower_signal << endl;
        printf( "<  transducer:compute_lower_rob              OUT.\n");
         
    #endif
        z_tube.lower_signal.simplify();
        return z_tube.lower_signal.front().value;
    };

    double transducer::compute_upper_rob(){
    
    #ifdef DEBUG__
        printf( ">  transducer:compute_upper_rob              IN.\n");
    #endif
        //compute_robustness();
    #ifdef DEBUG__
        cout << "z:" << z << endl;
    #endif
        // if for some reason, z was not computed before
        if (z.empty()) // is it the best test ?
            compute_robustness();
        z_tube.upper_signal = Signal(z);
        double last_data_t =  get_last_data_time();
        
        if (end_time>last_data_t) 
        {   
            z_tube.upper_signal.resize(start_time, last_data_t, TOP);
            z_tube.upper_signal.appendSample(last_data_t+Signal::Eps, TOP, 0., false);
            z_tube.upper_signal.endTime = end_time;
        }

    #ifdef DEBUG__
        printf( "<  transducer:compute_upper_rob              OUT.\n");
    #endif
        z_tube.upper_signal.simplify();
        return z_tube.upper_signal.front().value;
    };
    
    double and_transducer::compute_lower_rob(){
        #ifdef DEBUG__
        printf( ">  and_transducer:compute_lower_rob           IN.\n");
        #endif
        childL->compute_lower_rob();  
        childR->compute_lower_rob();
        z_tube.lower_signal.compute_and(childL->z_tube.lower_signal,childR->z_tube.lower_signal);
        z_tube.lower_signal.resize(start_time, min(childL->z_tube.lower_signal.endTime,childR->z_tube.lower_signal.endTime),BOTTOM);
        if (z_tube.lower_signal.empty())
            z_tube.lower_signal.appendSample(start_time, BOTTOM);
        #ifdef DEBUG__
        printf( "<  and_transducer:compute_lower_rob           OUT.\n");
        #endif
        return z_tube.lower_signal.front().value;
    };

    double and_transducer::compute_upper_rob(){
        #ifdef DEBUG__
        printf( ">  and_transducer:compute_upper_rob           IN.\n");
        #endif
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        z_tube.upper_signal.compute_and(childL->z_tube.upper_signal,childR->z_tube.upper_signal);
        z_tube.upper_signal.resize(start_time,z_tube.upper_signal.endTime,TOP);
        if (z_tube.upper_signal.empty())
            z_tube.upper_signal.appendSample(start_time,TOP);
        #ifdef DEBUG__
        printf( "<  and_transducer:compute_upper_rob           OUT.\n");
        #endif
        return z_tube.upper_signal.front().value;
    };

    double or_transducer::compute_lower_rob(){
        childL->compute_lower_rob();
        childR->compute_lower_rob();
        z_tube.lower_signal.compute_or(childL->z_tube.lower_signal,childR->z_tube.lower_signal);
        z_tube.lower_signal.resize(start_time,z_tube.lower_signal.endTime,BOTTOM);
        if (z_tube.lower_signal.empty())
            z_tube.lower_signal.appendSample(start_time, BOTTOM);
        return z_tube.lower_signal.front().value;
    };

    double or_transducer::compute_upper_rob(){
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        z_tube.upper_signal.compute_or(childL->z_tube.upper_signal,childR->z_tube.upper_signal);
        z_tube.upper_signal.resize(start_time,min(childL->z_tube.upper_signal.endTime,childR->z_tube.upper_signal.endTime),TOP);
        if (z_tube.upper_signal.empty())
            z_tube.upper_signal.appendSample(start_time,TOP);
		
        return z_tube.upper_signal.front().value;
    };

// IMPLIES transducer
    double implies_transducer::compute_lower_rob(){
        childL->compute_upper_rob();
        childR->compute_lower_rob();

        Signal z1;
        z1.compute_not(childL->z_tube.upper_signal);
        z_tube.lower_signal.compute_or(z1,childR->z_tube.lower_signal);
        z_tube.lower_signal.resize(start_time,z_tube.lower_signal.endTime,BOTTOM);

        if (z_tube.lower_signal.empty())
            z_tube.lower_signal.appendSample(start_time, BOTTOM);
        return z_tube.lower_signal.front().value;
    };

    double implies_transducer::compute_upper_rob(){
        childL->compute_lower_rob();
        childR->compute_upper_rob();

        Signal z1;
        z1.compute_not(childL->z_tube.lower_signal);
        z_tube.upper_signal.compute_or(z1,childR->z_tube.upper_signal);
        
        z_tube.upper_signal.resize(start_time,min(z1.endTime,childR->z_tube.upper_signal.endTime),TOP);
        if (z_tube.upper_signal.empty())
            z_tube.upper_signal.appendSample(start_time,TOP);
        return z_tube.upper_signal.front().value;
    };
    
    // NOT transducer: swap upper and lower
    double not_transducer::compute_upper_rob(){
        child->compute_lower_rob();
        if (child->z_tube.lower_signal.empty()) {
            z_tube.upper_signal.appendSample(start_time,TOP);
            return TOP;
        }
        z_tube.upper_signal.compute_not(child->z_tube.lower_signal);
        return z_tube.upper_signal.front().value;
    }

    double not_transducer::compute_lower_rob(){
        child->compute_upper_rob();
        if (child->z_tube.upper_signal.empty()) {
            z_tube.lower_signal.appendSample(start_time,BOTTOM);
            return BOTTOM;
        }
        z_tube.lower_signal.compute_not(child->z_tube.upper_signal);
        return z_tube.lower_signal.front().value;
    }

    // EVENTUALLY
    double ev_transducer::compute_lower_rob() {
        // lower robustness for a max operator. Partial information gives a lower bound for max, so we keep it. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_lower_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_lower_rob(); // 

        // Maybe there was/is a good reason for, feels like I'll regret it        
//      if (child->z_tube.lower_signal.endTime < a) {
//         z_tube.lower_signal.appendSample(start_time, BOTTOM); 
//          return BOTTOM;
//      }
    
        z_tube.lower_signal.compute_timed_eventually(child->z_tube.lower_signal, a, b);        
        double et =min(z_tube.lower_signal.endTime,end_time);
        z_tube.lower_signal.resize(start_time,max(start_time,et), BOTTOM);

        if (z_tube.lower_signal.empty()) // why not, but can this really happen ?
            z_tube.lower_signal.appendSample(start_time, BOTTOM); 

#ifdef DEBUG__
        cout << "OUT: z_tube.lower_signal:"<< z_tube.lower_signal << endl;
        printf( "<  ev_transducer:computer_lower_rob           OUT.\n");
#endif
        return z_tube.lower_signal.front().value;
    }

    double ev_transducer::compute_upper_rob() {
        // upper bound on max. Partial info can always be beaten by new samples, so can't say anything. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_upper_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_upper_rob();
    
//        if (child->z_tube.upper_signal.endTime < a) {
//            z_tube.upper_signal.appendSample(start_time, TOP); 
//            return TOP;
//        }

        z_tube.upper_signal.compute_timed_eventually(child->z_tube.upper_signal, a, b);

        // Here we remove values computed with partial data 
        double et =min(z_tube.upper_signal.endTime-b+a,end_time);
        z_tube.upper_signal.resize(start_time,et, 0.);

        if (z_tube.upper_signal.empty()) 
            z_tube.upper_signal.appendSample(start_time, TOP); 

#ifdef DEBUG__
        cout << "OUT: z_tube.upper_signal:"<< z_tube.upper_signal << endl;
        printf( "<  ev_transducer:computer_upper_rob           OUT.\n");
#endif
        return z_tube.upper_signal.front().value;
    }

    // ALWAYS
    double alw_transducer::compute_lower_rob() {
        // lower bound on a min operator. Partial info cannot help here. 

#ifdef DEBUG__
        printf( ">  alw_transducer:computer_lower_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_lower_rob();

//        if (child->z_tube.lower_signal.endTime < a) {
//            z_tube.lower_signal.appendSample(start_time,BOTTOM);        
//            return BOTTOM;
//        }
    
        z_tube.lower_signal.compute_timed_globally(child->z_tube.lower_signal, a, b);

        // Here we remove values computed with partial data 
        double et =min(z_tube.lower_signal.endTime-b+a,end_time);
        z_tube.lower_signal.resize(start_time,et, 0.);
	
        if (z_tube.lower_signal.empty()) 
            z_tube.lower_signal.appendSample(start_time,BOTTOM);        

#ifdef DEBUG__
        printf( "OUT: z_tube.lower_signal:");
        cout << "<  alw_transducer:computer_lower_rob           OUT."<< endl;
#endif

        return z_tube.lower_signal.front().value;
    }

    double alw_transducer::compute_upper_rob() {
#ifdef DEBUG__
        printf( ">  alw_transducer:computer_upper_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        child->compute_upper_rob();
//        if (child->z_tube.upper_signal.endTime < a) {
//            z_tube.upper_signal.appendSample(start_time, TOP); 
//            return TOP;
//        }

        //    cout << "child->z_tube.upper_signal:" << child->z_tube.upper_signal << endl;
        z_tube.upper_signal.compute_timed_globally(child->z_tube.upper_signal, a, b);
        double et =min(z_tube.upper_signal.endTime,end_time);
        z_tube.upper_signal.resize(start_time,max(start_time,et), 0.);

        if (z_tube.upper_signal.empty()) 
            z_tube.upper_signal.appendSample(start_time, TOP); 

#ifdef DEBUG__
        cout << "OUT: z_tube.upper_signal:"<< z_tube.upper_signal << endl;
        printf( "<  alw_transducer:computer_upper_rob          OUT.\n");
#endif
        return z_tube.upper_signal.front().value;

    }

    // TODO the following is a super conservative implementation - (how) can we do better ?
    double until_transducer::compute_lower_rob() {

        //cout << "GETTING INTO until_transducer::compute_lower_rob" << endl;
        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;
        
        if (childL->compute_lower_rob()==BOTTOM) return BOTTOM;
        if (childR->compute_lower_rob()==BOTTOM) return BOTTOM;

        z_tube.lower_signal.compute_timed_until(childL->z_tube.lower_signal,childR->z_tube.lower_signal, a, b);
        double et =min(z_tube.lower_signal.endTime,end_time);
        z_tube.lower_signal.resize(start_time,max(start_time,et),0.);
        
        
        if (z_tube.lower_signal.empty())
            return BOTTOM;
        else
            return z_tube.lower_signal.front().value;

    }

    double until_transducer::compute_upper_rob() {

        double a,b;
        if (!get_param(I->begin_str,a)) a = I->begin;
        if (!get_param(I->end_str,b)) b = I->end;

        if (childL->compute_upper_rob()==TOP) return TOP;
        if (childR->compute_upper_rob()==TOP) return TOP;

        z_tube.upper_signal.compute_timed_until(childL->z_tube.upper_signal,childR->z_tube.upper_signal, a, b);
        double et =min(z_tube.upper_signal.endTime-b,end_time);
        z_tube.upper_signal.resize(start_time,max(start_time,et),0.);

        if (z_tube.upper_signal.empty())
            return TOP;
        else
            return z_tube.upper_signal.front().value;
    }

}
