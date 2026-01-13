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

    int num_samples = 100; // default value
    int num_traces = 100;
    for (int i = 1; i < argc; ++i) {
        if (string(argv[i]) == "-n" && i + 1 < argc) {
            num_samples = stoi(argv[++i]);
        }
        if (string(argv[i]) == "-N" && i+1<argc) {
            num_traces  = stoi(argv[++i]);
        }
    }
    cout << "Number of samples: " << num_samples << endl;

    // STLDriver is the class implementing the parser.            

    STLDriver stl_driver = STLDriver();	  
    string s ="signal x,y\n";
    s+="param p=3\n";
    s+="phi:=";
    s+="x[t]>p\n";
    s+="param t1=3\n";
    s+="aphi:=";
    s+="alw_[0, t1] (y[t]>0)\n";
 
    bool parse_success = stl_driver.parse_string(s);
    
    if (parse_success) {
        cout << "Formula parsed successfully" << endl;    
        stl_driver.print();                    
        }
    else {
        cout << "Something went wrong." <<endl;
        return 1; 
    }  

    // Testing one sample
    vector<double> v ;
    v = {0,-2, 4.43};
    stl_driver.add_sample(v);    
    v = {2.5, 0, 0};        
    stl_driver.add_sample(v);
    v = {5, 5, -1.3};        
    stl_driver.add_sample(v);
    
    vector<double> robs;
    robs = stl_driver.get_online_rob("phi");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    
    // set param 
    cout << "Changing param p to -5" << endl;
    stl_driver.set_param("p", -5);
    robs = stl_driver.get_online_rob("phi");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    
    cout << "Monitoring aphi" << endl;
    robs = stl_driver.get_online_rob("aphi");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    cout << "Changing param t1 to 10" << endl;
    stl_driver.set_param("t1", 10);
    robs = stl_driver.get_online_rob("aphi");
    cout << robs[0] << " " << robs[1] << " " << robs[2] << endl;
    

    // Testing get_monitor
    STLMonitor monitor= stl_driver.get_monitor("phi");

    cout << "monitor:" << monitor << endl;


    return 0;

}
