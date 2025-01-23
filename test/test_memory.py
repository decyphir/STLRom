import sys
sys.path.append('../build')
import stlrom
import numpy as np
import time
import gc

def rand_sample(t, dim):
    val = np.random.rand(dim)
    return np.concatenate(([t], val)) 

def test_timing(driver,formula, num_sample):
    
    start_time =time.time()
    for i in range(0,num_sample):
        driver.add_sample(rand_sample(i,2))
        rob1 = driver.get_online_rob(formula)
    # display both formula and robustness in one formated line
    end_time = time.time()
    execution_time = end_time - start_time
    print(f"Execution time for {num_sample} samples: {execution_time:.4f} seconds")
    
    return execution_time

s="""signal x,y
param p1=3,p2=-2
mu1 := x[t]>p1
mu2 := abs(y[t])>p2
mmu_or := mu1 or mu2 
mmu_and := mu1 and mu2 
phi:= alw_[0,3] mmu_or"""

s2="""signal x,y
mu1 := x[t]>0
phi:= alw_[0,3] mu1
"""

for n in range(0,50):
    driver=stlrom.STLDriver()
    driver.parse_string(s)
    test_timing(driver, "phi",10000)
    del driver
    gc.collect()