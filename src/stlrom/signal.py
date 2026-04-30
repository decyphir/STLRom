def plot(self, label=None, ax=None, title='Signal Plot'):
    import matplotlib.pyplot as plt

    samples_list = self.get_samples_list()

    if ax is None:
        ax = plt.figure(figsize=(15, 5)).gca()

    if label is None:
        label = f'Signal {len(ax.get_lines()) + 1}'

    ax.set_xlabel('Time')
    ax.set_ylabel('Value')
    ax.set_title(title)
    ax.grid(True)
        
    def plot_sample(s, dt, color=None):
        if color is None:
            l, = ax.plot(s.time, s.value, marker='o')
            color = l.get_color()
            l = ax.plot([s.time, s.time+dt], [s.value, s.value_at(s.time+dt)], color=color, linestyle='-')
        else:
            l = ax.plot(s.time, s.value, marker='o', color=color)
            l = ax.plot([s.time, s.time+dt], [s.value, s.value_at(s.time+dt)], color=color, linestyle='-')
        return color,l
    
    if len(samples_list) > 1:
        s = samples_list[0]
        sn = samples_list[1]
        dt = sn.time-s.time
            
        col,l = plot_sample(s, dt)
                
        for i in range(1, len(samples_list)-1):
            s = samples_list[i]
            sn = samples_list[i+1]
            dt = sn.time-s.time
            plot_sample(s,dt,color=col)
    elif len(samples_list) == 1:
        sn = samples_list[0]
        col = None
    else:
        print("No samples to plot.")
        return ax
    
    col, l = plot_sample(sn, self.end_time-sn.time, col)
    l[0].set_label(label)
    #ax.plot(sn.time, sn.value, marker='o', color=col)
    ax.legend()
    ax.figure.canvas.draw()

    return ax

def plot_rob_map(rob_map, ax=None, same_figure=False, title='Robustness Map'):
    keys = sorted(rob_map, key=len)
    ax = rob_map[keys[0]].plot(label=keys[0], ax=ax, title=title)
    for key in keys[1:]:
        ax = rob_map[key].plot(label=key, ax=ax if same_figure else None, title=title)