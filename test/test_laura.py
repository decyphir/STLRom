import stlrom

# Zone search test

# Predicates:
# p(t): the entity in the search zone at time t, [0, 1]
# q(t): the UAV in the search zone at time t, [0, 1]

# Specification
# G[a, b](p(t) -> F[a, b]q(t))

# number of samples
#nb_samples = 20

# number of signals
#nb_signal = 2

# instantiate an STLDriver object
monitor = stlrom.STLDriver()

# declare signals, parameters (optional), and formulas
monitor.parse_string("signal p, q")
monitor.parse_string("param tau=20")

# declare specification
monitor.parse_string("mu := ev_[0, tau] q[t]")
monitor.parse_string("phi := alw_[0, tau] p[t] => mu")


sig_names= monitor.get_signals_names()
print("Signal Names are: ", sig_names)


#Samples of the trace from ZoneSearchTrace2.csv
monitor.add_sample([0, 1, 0])
monitor.add_sample([1, 1, 0])
monitor.add_sample([2, 0, 0])
monitor.add_sample([3, 0, 0])
monitor.add_sample([4, 0, 1])
monitor.add_sample([5, 0, 1])
monitor.add_sample([6, 1, 1])
monitor.add_sample([7, 0, 0])
monitor.add_sample([8, 1, 0])
monitor.add_sample([9, 0, 1])
monitor.add_sample([10, 0, 1])
monitor.add_sample([11, 0, 0])
monitor.add_sample([12, 1, 0])
monitor.add_sample([13, 0, 0])
monitor.add_sample([14, 1, 0])
monitor.add_sample([15, 0, 1])
monitor.add_sample([16, 1, 1])
monitor.add_sample([17, 1, 1])
monitor.add_sample([18, 0, 0])
monitor.add_sample([19, 0, 0])

print("Trace data: ", monitor.data)
#monitor.disp()

rob_phi = monitor.get_online_rob("phi")
print("rob_phi = ", rob_phi)
