def plot(self, label=None, ax=None, title='Signal Plot', **kwargs):
    import matplotlib.pyplot as plt

    draw_canvas = kwargs.pop('draw_canvas', True)
    draw_samples = kwargs.pop('draw_samples', False)
    plot_sat = kwargs.pop('plot_sat', False)
    step_plot  = kwargs.pop('step_plot', False)

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

    if step_plot:
        l_line, = ax.step(times, values, linestyle='-', **kwargs)
    else:
        l_line, = ax.plot(times, values, linestyle='-', **kwargs)
    
    c = l_line.get_color()

    if draw_samples:
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

    if plot_sat:
        y_sat = self.copy()
        y_sat.compute_boolean(self)
        ax_bool = ax.twinx(); ax_bool.set_yticks([0, 1]); ax_bool.set_yticklabels(['FALSE', 'TRUE']);
        y_sat.plot(f"{label} (Boolean sat.)", ax=ax_bool, color='black', step_plot=True)


    return ax



def plot_rob_map(rob_map, max_depth=None, to_plot="robustness", ax=None, same_figure=False, title='Robustness Map'):
    keys = sorted(rob_map)
    if max_depth is not None:
        max_depth = min(max_depth, keys[-1])
        keys = [k for k in keys if k <= max_depth]

    if to_plot not in {"robustness", "lower", "upper"}:
        raise ValueError(f"Invalid to_plot value: {to_plot}. Must be one of 'robustness', 'lower', or 'upper'.")
    
    if to_plot == "robustness":
        to_plot = "z"
    elif to_plot == "lower":
        to_plot = "z_low"
        title = "Lower Robustness Map" if title == 'Robustness Map' else title
    elif to_plot == "upper":
        to_plot = "z_up"
        title = "Upper Robustness Map" if title == 'Robustness Map' else title

    first_formula = next(iter(rob_map[keys[0]]))
    ax = rob_map[keys[0]][first_formula][to_plot].plot(label=f"{first_formula} [depth {keys[0]}]", ax=ax, title=title, draw_canvas=False)
    for key in keys[1:]:
        for formula, info in rob_map[key].items():
            if not same_figure:
                ax.figure.canvas.draw()
            ax = info[to_plot].plot(label=f"{formula} [depth {key}]", ax=ax if same_figure else None, title=title, draw_canvas=False)
    ax.figure.canvas.draw()

    return ax



def plot_rob_map_widget(rob_map, title='Robustness Map'):
    import ipywidgets as widgets
    import matplotlib.pyplot as plt

    sorted_depths = sorted(rob_map.keys())

    depth_checkboxes = [widgets.Checkbox(value=(i==0), description=f"Depth {i}") for i in sorted_depths[:-1]]
    depth_checkboxes.append(widgets.Checkbox(value=False, description=f"Plot Data"))

    rob_dropdown = widgets.Dropdown(options=[('Robustness', 'z'), ('Lower', 'z_low'), ('Upper', 'z_up')], description='Robustness:')
    
    def plot_depth(robustness, **depths):
        any_plotted = False
        ax = plt.figure(figsize=(15, 5)).gca()
        selected = [int(depth) for depth, checked in depths.items() if checked]

        if sorted_depths[-1] in selected:
            for formula, info in rob_map[sorted_depths[-1]].items():
                if info["z"] is not None and len(info["z"].get_samples_list()) > 0:
                    info["z"].plot(label=f"{formula}", title=title, ax=ax)
                    any_plotted = True
            selected.remove(sorted_depths[-1])

        for depth in selected:
            for formula, info in rob_map[depth].items():
                if info[robustness] is not None and len(info[robustness].get_samples_list()) > 0:
                    info[robustness].plot(label=f"{formula}", title=title, ax=ax)
                    any_plotted = True
                else:
                    # print(f"No data to plot for formula '{formula}' at depth {depth}.")
                    pass
        if not any_plotted:
            ax.text(0.5, 0.5, 'No data to plot for selected depth(s) and robustness type.', horizontalalignment='center', verticalalignment='center', transform=ax.transAxes)

    widgets.interact(plot_depth, robustness=rob_dropdown, **{str(depth): checkbox for depth, checkbox in zip(sorted_depths, depth_checkboxes)})