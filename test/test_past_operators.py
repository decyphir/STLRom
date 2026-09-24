import stlrom as stl
from stlrom import RandSignalGen, OscillSignalGen

osc_gen = OscillSignalGen()
rand_gen = RandSignalGen(seed=1, interp='LINEAR', dt_min=0.02, dt_max=0.8, v_min=-1, v_max=1)

sin_sig = osc_gen.get_signal(t0=0, tf=5, dt=0.02, damp=-0.5, period=1)
rand_sig = rand_gen.get_signal(t0=0, tf=5)

ax = sin_sig.plot(label='Oscill')
ax = rand_sig.plot(label='Rand', ax=ax)

driver = stl.STLDriver()
driver.parse_string(
    """
    signal x, y
    param a = 0, b = 0.5

    x_sup := x[t] > 0
    y_sup := y[t] > 0

    alw_x := alw_[a, b] x_sup
    ev_x  := ev_ [a, b] x_sup
    x_until_y := x_sup until_[a, b] y_sup

    hist_x := hist_[a,b] x_sup
    once_x := once_[a,b] x_sup
    """
)
driver.set_signals([sin_sig, rand_sig])
print(driver)

phis_unary = ['alw_x', 'ev_x', 'hist_x', 'once_x']

params = [[0, 0.5], [0.3, 0.8]]
rob_windows = [[0, 4.5], [0, 5], [0.5, 4.5], [0.5, 5]]

for phi in phis_unary:
    monitor = driver.get_monitor(phi)
    for a, b in params:
        monitor.set_param('a', a)
        monitor.set_param('b', b)
        for t_begin, t_end in rob_windows:
            label = str(monitor.formula).replace('[a,b]', f'[{a},{b}]')
            print(f"get_rob({t_begin}, {t_end}) of {label}")
            rob = monitor.get_rob_signal(t_begin, t_end)