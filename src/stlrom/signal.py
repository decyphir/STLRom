def plot(self, label=None, ax=None):
    import matplotlib.pyplot as plt

    samples_list = self.get_samples_list()

    if ax is None:
        ax = plt.figure(figsize=(15, 5)).gca()

    if label is None:
        label = f'Signal {len(ax.get_lines()) + 1}'

    ax.set_xlabel('Time')
    ax.set_ylabel('Value')
    ax.set_title('Signal Plot')
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
    else:
        sn = samples_list[0]
        col = None
    
    col, l = plot_sample(sn, self.end_time-sn.time, col)
    l[0].set_label(label)
    #ax.plot(sn.time, sn.value, marker='o', color=col)
    ax.legend()
    ax.figure.canvas.draw()

    return ax