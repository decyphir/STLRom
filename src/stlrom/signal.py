def plot(self, label=None, ax=None, title='Signal Plot', **kwargs):
    import matplotlib.pyplot as plt

    draw_canvas = kwargs.pop('draw_canvas', True)
    
    samples_list = self.get_samples_list()

    if ax is None:
        ax = plt.figure(figsize=(15, 5)).gca()

    if label is None:
        label = f'Signal {len(ax.get_lines()) + 1}'

    ax.set_xlabel('Time')
    ax.set_ylabel('Value')
    ax.set_title(title)
    ax.grid(True)
        
    import numpy as np
    if len(samples_list) == 0:
        print(f"{label}: No samples to plot.")
        return ax

    times = []
    values = []

    for i in range(len(samples_list) - 1):
        s = samples_list[i]
        sn = samples_list[i + 1]

        times += [s.time, sn.time, np.nan]
        values += [s.value, s.value_at(sn.time), np.nan]

    # Last segment (to end_time)
    sn = samples_list[-1]
    times += [sn.time, self.end_time, np.nan]
    values += [sn.value, sn.value_at(self.end_time), np.nan]

    l_line, = ax.plot(times, values, linestyle='-')

    c = l_line.get_color()

    ax.plot(
        [s.time for s in samples_list],
        [s.value for s in samples_list],
        linestyle='None',
        marker='o',
        color=c
    )

    l_line.set_label(label)

    ax.legend()

    if draw_canvas:
        ax.figure.canvas.draw()

    return ax

def plot_rob_map(rob_map, ax=None, same_figure=False, title='Robustness Map'):
    keys = sorted(rob_map, key=len)
    ax = rob_map[keys[0]].plot(label=keys[0], ax=ax, title=title, draw_canvas=False)
    for key in keys[1:]:
        if not same_figure:
            ax.figure.canvas.draw()
        ax = rob_map[key].plot(label=key, ax=ax if same_figure else None, title=title, draw_canvas=False)
    ax.figure.canvas.draw()

    # return last ax
    return ax

def plot_rob_map_widget(rob_map, title='Robustness Map'):
    import ipywidgets as widgets

    sorted_keys = sorted(rob_map.keys(), key=len, reverse=True)
    dropdown = widgets.Dropdown(options=sorted_keys, description='Subformula:', layout=widgets.Layout(width=f'{len(sorted_keys[0])*0.5+6}em'))

    def plot_subformula_rob_map(subformula):
        rob_map[subformula].plot(label=f'{subformula}', title=title)

    widgets.interact(plot_subformula_rob_map, subformula=dropdown)