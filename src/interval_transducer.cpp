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
        cout << "z : " << z << endl;
    #endif

        double last_data_t = get_last_data_time();
        Signal z_low(z.lower_signal);
        
        if (end_time>last_data_t) 
        {   
            z_low.resize(start_time, last_data_t, BOTTOM);
            z_low.appendSample(last_data_t+Signal::Eps, BOTTOM, 0., false);
            z_low.endTime = end_time;
        }
        
    #ifdef DEBUG__
        cout << "z_low:" << z_low << endl;
        printf( "<  transducer:compute_lower_rob              OUT.\n");
    #endif
        z_low.simplify();
        z.lower_signal = z_low.getSamplesDeque();
        return z.lower_signal.front().value;
    };

    double transducer::compute_upper_rob(){
    
    #ifdef DEBUG__
        printf( ">  transducer:compute_upper_rob              IN.\n");
    #endif
        double last_data_t =  get_last_data_time();
        Signal z_up(z.upper_signal);
        
        if (end_time>last_data_t) 
        {   
            z_up.resize(start_time, last_data_t, TOP);
            z_up.appendSample(last_data_t+Signal::Eps, TOP, 0., false);
            z_up.endTime = end_time;
        }

    #ifdef DEBUG__
        printf( "<  transducer:compute_upper_rob              OUT.\n");
    #endif
        z_up.simplify();
        z.upper_signal = z_up.getSamplesDeque();
        return z.upper_signal.front().value;
    };
    
    double and_transducer::compute_lower_rob(){
        #ifdef DEBUG__
        printf( ">  and_transducer:compute_lower_rob           IN.\n");
        #endif
        childL->compute_lower_rob();  
        childR->compute_lower_rob();
        Signal z_low(z.lower_signal);
        Signal childL_low(childL->z.lower_signal);
        Signal childR_low(childR->z.lower_signal);
        z_low.compute_and(childL_low,childR_low);
        z_low.resize(start_time, min(childL_low.endTime,childR_low.endTime),BOTTOM);
        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
        z.lower_signal = z_low.getSamplesDeque();
        #ifdef DEBUG__
        printf( "<  and_transducer:compute_lower_rob           OUT.\n");
        #endif
        return z.lower_signal.front().value;
    };

    double and_transducer::compute_upper_rob(){
        #ifdef DEBUG__
        printf( ">  and_transducer:compute_upper_rob           IN.\n");
        #endif
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        Signal z_up(z.upper_signal);
        Signal childL_up(childL->z.upper_signal);
        Signal childR_up(childR->z.upper_signal);
        z_up.compute_and(childL_up,childR_up);
        z_up.resize(start_time,z_up.endTime,TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
        z.upper_signal = z_up.getSamplesDeque();
        #ifdef DEBUG__
        printf( "<  and_transducer:compute_upper_rob           OUT.\n");
        #endif
        return z.upper_signal.front().value;
    };

    double or_transducer::compute_lower_rob(){
        childL->compute_lower_rob();
        childR->compute_lower_rob();
        Signal z_low(z.lower_signal);
        Signal childL_low(childL->z.lower_signal);
        Signal childR_low(childR->z.lower_signal);
        z_low.compute_or(childL_low,childR_low);
        z_low.resize(start_time,z_low.endTime,BOTTOM);
        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
        z.lower_signal = z_low.getSamplesDeque();
        return z.lower_signal.front().value;
    };

    double or_transducer::compute_upper_rob(){
        childL->compute_upper_rob();
        childR->compute_upper_rob();
        Signal z_up(z.upper_signal);
        Signal childL_up(childL->z.upper_signal);
        Signal childR_up(childR->z.upper_signal);
        z_up.compute_or(childL_up,childR_up);
        z_up.resize(start_time,min(childL_up.endTime,childR_up.endTime),TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
		
        z.upper_signal = z_up.getSamplesDeque();
        return z.upper_signal.front().value;
    };

// IMPLIES transducer
    double implies_transducer::compute_lower_rob(){
        childL->compute_upper_rob();
        childR->compute_lower_rob();
        Signal z_low(z.lower_signal);
        Signal childL_up(childL->z.upper_signal);
        Signal childR_low(childR->z.lower_signal);

        Signal z1;
        z1.compute_not(childL_up);
        z_low.compute_or(z1,childR_low);
        z_low.resize(start_time,z_low.endTime,BOTTOM);

        if (z_low.empty())
            z_low.appendSample(start_time, BOTTOM);
        z.lower_signal = z_low.getSamplesDeque();
        return z.lower_signal.front().value;
    };

    double implies_transducer::compute_upper_rob(){
        childL->compute_lower_rob();
        childR->compute_upper_rob();
        Signal z_up(z.upper_signal);
        Signal childL_low(childL->z.lower_signal);
        Signal childR_up(childR->z.upper_signal);

        Signal z1;
        z1.compute_not(childL_low);
        z_up.compute_or(z1,childR_up);
        
        z_up.resize(start_time,min(z1.endTime,childR_up.endTime),TOP);
        if (z_up.empty())
            z_up.appendSample(start_time,TOP);
        z.upper_signal = z_up.getSamplesDeque();
        return z.upper_signal.front().value;
    };
    
    // NOT transducer: swap upper and lower
    double not_transducer::compute_upper_rob(){
        child->compute_lower_rob();
        Signal z_up(z.upper_signal);
        Signal child_low(child->z.lower_signal);
        if (child->z.lower_signal.empty()) {
            z_up.appendSample(start_time,TOP);
            z.upper_signal = z_up.getSamplesDeque();
            return TOP;
        }
        z_up.compute_not(child_low);
        z.upper_signal = z_up.getSamplesDeque();
        return z.upper_signal.front().value;
    }

    double not_transducer::compute_lower_rob(){
        child->compute_upper_rob();
        Signal z_low(z.lower_signal);
        Signal child_up(child->z.upper_signal);
        if (child->z.upper_signal.empty()) {
            z_low.appendSample(start_time,BOTTOM);
            z.lower_signal = z_low.getSamplesDeque();
            return BOTTOM;
        }
        z_low.compute_not(child_up);
        z.lower_signal = z_low.getSamplesDeque();
        return z.lower_signal.front().value;
    }

    // EVENTUALLY
    double ev_transducer::compute_lower_rob() {
        // lower robustness for a max operator. Partial information gives a lower bound for max, so we keep it. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_lower_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        child->compute_lower_rob();
        Signal z_low(z.lower_signal);
        Signal child_low(child->z.lower_signal);

        // Maybe there was/is a good reason for, feels like I'll regret it        
//      if (child_low.endTime < I->begin) {
//          z_low.appendSample(start_time, BOTTOM); 
//          z.lower_signal = z_low.getSamplesDeque();
//          return BOTTOM;
//      }
    
        z_low.compute_timed_eventually(child_low, I->begin, I->end);        
        double et =min(z_low.endTime,end_time);
        z_low.resize(start_time,max(start_time,et), BOTTOM);

        if (z_low.empty()) // why not, but can this really happen ?
            z_low.appendSample(start_time, BOTTOM); 

        z.lower_signal = z_low.getSamplesDeque();
#ifdef DEBUG__
        cout << "OUT: z.lower_signal:"<< z_tube.lower_signal << endl;
        printf( "<  ev_transducer:computer_lower_rob           OUT.\n");
#endif
        return z.lower_signal.front().value;
    }

    double ev_transducer::compute_upper_rob() {
        // upper bound on max. Partial info can always be beaten by new samples, so can't say anything. 

#ifdef DEBUG__
        printf( ">  ev_transducer:computer_upper_rob           IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a = I->begin;
        double b = I->end;

        child->compute_upper_rob();
        Signal z_up(z.upper_signal);
        Signal child_up(child->z.upper_signal);
    
//        if (child_up.endTime < a) {
//            z_up.appendSample(start_time, TOP); 
//            z.upper_signal = z_up.getSamplesDeque();
//            return TOP;
//        }

        z_up.compute_timed_eventually(child_up, a, b);

        // Here we remove values computed with partial data 
        double et =min(z_up.endTime-b+a,end_time);
        z_up.resize(start_time,et, 0.);

        if (z_up.empty()) 
            z_up.appendSample(start_time, TOP); 

        z.upper_signal = z_up.getSamplesDeque();
#ifdef DEBUG__
        cout << "OUT: z.upper_signal:"<< z_tube.upper_signal << endl;
        printf( "<  ev_transducer:computer_upper_rob           OUT.\n");
#endif
        return z.upper_signal.front().value;
    }

    // ALWAYS
    double alw_transducer::compute_lower_rob() {
        // lower bound on a min operator. Partial info cannot help here. 

#ifdef DEBUG__
        printf( ">  alw_transducer:computer_lower_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a = I->begin;
        double b = I->end;

        child->compute_lower_rob();
        Signal z_low(z.lower_signal);
        Signal child_low(child->z.lower_signal);

//        if (child_low_signal.endTime < a) {
//            z_low.appendSample(start_time,BOTTOM);        
//            z.lower_signal = z_low.getSamplesDeque();
//            return BOTTOM;
//        }
    
        z_low.compute_timed_globally(child_low, a, b);

        // Here we remove values computed with partial data 
        double et =min(z_low.endTime-b+a,end_time);
        z_low.resize(start_time,et, 0.);
	
        if (z_low.empty()) 
            z_low.appendSample(start_time,BOTTOM);        

        z.lower_signal = z_low.getSamplesDeque();
#ifdef DEBUG__
        printf( "OUT: z.lower_signal:");
        cout << "<  alw_transducer:computer_lower_rob           OUT."<< endl;
#endif

        return z.lower_signal.front().value;
    }

    double alw_transducer::compute_upper_rob() {
#ifdef DEBUG__
        printf( ">  alw_transducer:computer_upper_rob          IN.\n");
        cout << "   I->a: " << I->begin << "   I->b: " << I->end << endl;
        cout << "   start_time:" << start_time << " end_time:" << end_time << endl;
#endif

        double a = I->begin;
        double b = I->end;

        child->compute_upper_rob();
        Signal z_up(z.upper_signal);
        Signal child_up(child->z.upper_signal);
//        if (child_up.endTime < a) {
//            z_up.appendSample(start_time, TOP); 
//            z.upper_signal = z_up.getSamplesDeque();
//            return TOP;
//        }

        //    cout << "child_up:" << child_up << endl;
        z_up.compute_timed_globally(child_up, a, b);
        double et =min(z_up.endTime,end_time);
        z_up.resize(start_time,max(start_time,et), 0.);

        if (z_up.empty()) 
            z_up.appendSample(start_time, TOP); 
        z.upper_signal = z_up.getSamplesDeque();
#ifdef DEBUG__
        cout << "OUT: z.upper_signal:"<< z.upper_signal << endl;
        printf( "<  alw_transducer:computer_upper_rob          OUT.\n");
#endif
        return z.upper_signal.front().value;

    }

    // TODO the following is a super conservative implementation - (how) can we do better ?
    double until_transducer::compute_lower_rob() {

        //cout << "GETTING INTO until_transducer::compute_lower_rob" << endl;
        double a = I->begin;
        double b = I->end;
        
        if (childL->compute_lower_rob()==BOTTOM) return BOTTOM;
        if (childR->compute_lower_rob()==BOTTOM) return BOTTOM;
        Signal z_low(z.lower_signal);
        Signal childL_low(childL->z.lower_signal);
        Signal childR_low(childR->z.lower_signal);

        z_low.compute_timed_until(childL_low,childR_low, a, b);
        double et =min(z_low.endTime,end_time);
        z_low.resize(start_time,max(start_time,et),0.);
        
        z.lower_signal = z_low.getSamplesDeque();
        if (z.lower_signal.empty())
            return BOTTOM;
        else
            return z.lower_signal.front().value;

    }

    double until_transducer::compute_upper_rob() {

        double a = I->begin;
        double b = I->end;

        if (childL->compute_upper_rob()==TOP) return TOP;
        if (childR->compute_upper_rob()==TOP) return TOP;
        Signal z_up(z.upper_signal);
        Signal childL_up(childL->z.upper_signal);
        Signal childR_up(childR->z.upper_signal);

        z_up.compute_timed_until(childL_up,childR_up, a, b);
        double et =min(z_up.endTime-b,end_time);
        z_up.resize(start_time,max(start_time,et),0.);

        z.upper_signal = z_up.getSamplesDeque();
        if (z.upper_signal.empty())
            return TOP;
        else
            return z.upper_signal.front().value;
    }

}
