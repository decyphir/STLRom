import sys
sys.path.append('../build')
import stlrom
import numpy as np
import time
import gc
import argparse

def rand_sample(t, dim):
    val = np.random.rand(dim)
    return np.concatenate(([t], val)) 

def test_timing(driver,formula, num_sample):
    
    start_time =time.time()
    for i in range(0,num_sample):
        driver.add_sample(rand_sample(i,2))
        driver.get_online_rob(formula)
    
    # display both formula and robustness in one formated line
    end_time = time.time()
    execution_time = end_time - start_time
    print(f"Execution time for {num_sample} samples: {execution_time:.4f} seconds")
    
    return execution_time

num_trace = 1
num_samples = 10000
s= "stress.stl"


if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--num_trace', type=int, default=1, help='Number of traces')
    parser.add_argument('--num_samples', type=int, default=10000, help='Number of samples')

    args = parser.parse_args()

    num_trace = args.num_trace
    num_samples = args.num_samples

    for n in range(0, num_trace):
        driver = stlrom.STLDriver()
        driver.parse_file(s)
        test_timing(driver, "phi", num_samples)