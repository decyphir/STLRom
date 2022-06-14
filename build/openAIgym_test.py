import gym
import numpy as np

env = gym.make("CartPole-v1")
env.action_space.seed(42)
t=0
trace=[]
x=[]
observation, info = env.reset(seed=42, return_info=True)

for _ in range(100):
    observation, reward, done, info = env.step(env.action_space.sample())
    env.render()
    trace.append(observation)
    x.append(t)
    t=t+1
    if done:  
        trace=np.array(trace)

        #print T[0] for cart position,
        #      T[1] for cart velocity,
        #      T[2] for pole angle,
        #      T[3] for pole angular velocity
        
        print(trace.T[0])
        print(x)
        print("\n\n")
        x=[]
        t=0
        trace=[]
        observation, info = env.reset(return_info=True)
        env.close()
        exit()

