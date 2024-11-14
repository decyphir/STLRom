    #include <stdio.h>
#include "stl_driver.h"
#include <string>
#include <vector>

using namespace std;
using namespace STLRom;

int main(int argc, char** argv) {

    /*  Read formula(s). 

        Formulas can be defined in a file or a string. At minima there needs to be: 
        - a signal(s) declaration  
        - one formula assignement, e.g.:

        signal x,y
        phi := alw_[0, 5.] (x[t]>0 and y[t]<2)

    */

    // STLDriver is the class implementing the parser.            
    
    STLDriver stl_driver = STLDriver();	  
    string s ="signal x,y\n";
    s+="param p=3\n";
    s+="mu := x[t]>p\n";
    s+="param t1=3\n";
    s+="phi:= alw_[0, t1] (y[t]>0)";
 
    bool parse_success = stl_driver.parse_string(s);
    
    if (parse_success) {
        cout << "Formula parsed successfully" << endl;    
        }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  

    // Testing get_monitor
    auto mu= stl_driver.get_monitor("mu");
    cout << mu << endl;

    auto phi = stl_driver.get_monitor("phi");
    cout << endl << "phi:" << endl;
    cout << phi << endl;

    phi.set_eval_time(0., 0.);
    unary_transducer* ut = dynamic_cast<unary_transducer*>(phi.formula);

    cout << endl << "phi:" << endl;
    cout << phi << endl;

    // Testing a few samples
    vector<double> v;
    v = {0,-2, 4.43};
    mu.add_sample(v);    
    phi.add_sample(v);
    v = {2.5, 0, 0};        
    mu.add_sample(v);
    phi.add_sample(v);
    v = {5, 5, -1.3};        
    mu.add_sample(v);
    phi.add_sample(v);
    

 //   double rho_mu = mu.update_rob();
 //   cout << "rho(mu): " << rho_mu << endl;
    
    double rho_phi = phi.update_rob();
    cout << "rho(phi): " << rho_phi << endl;
    

    // vector<double> robs;
    // robs = stl_driver.get_online_rob("phi");
    
    // // set param 
    // cout << "Changing param p to -5" << endl;
    // stl_driver.set_param("p", -5);
    // robs = stl_driver.get_online_rob("phi");
    // cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    
    // cout << "Monitoring aphi" << endl;
    // robs = stl_driver.get_online_rob("aphi");
    // cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    // cout << "Changing param t1 to 10" << endl;
    // stl_driver.set_param("t1", 10);
    // robs = stl_driver.get_online_rob("aphi");
    // cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    



    return 0;

}
