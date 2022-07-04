import gym
import numpy as np
import pyrobonTL
import matplotlib.pyplot as plt


driver=pyrobonTL.STLDriver()
s="signal x\nphi:=x[t]>2"
driver.parse_string(s)


env = gym.make("CartPole-v1")
env.action_space.seed(42)
t=0
trace=[]
time=[]

observation, info = env.reset(seed=42, return_info=True)

for _ in range(100):
    observation, reward, done, info = env.step(env.action_space.sample())
    env.render()
    trace.append(observation)
    time.append(t)
    t=t+1
    a=[t-1,observation[0]]
    #print(a)
    driver.add_sample(a)
    if done:  
        trace=np.array(trace)
        #print T[0] for cart position,
        #      T[1] for cart velocity,
        #      T[2] for pole angle,
        #      T[3] for pole angular velocity

        #print(trace.T[0])
        #print(time)
        #print("\n\n")
        time=[]
        trace=[]
        t=0
        observation, info = env.reset(return_info=True)
        robs = driver.get_online_rob("phi")
        print("robs : ")
        print(robs[0],robs[1],robs[2])
        exit()
env.close()
