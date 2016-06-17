
from icecube.icetray import memory

def print_snapshot(snap):
    """
    Prints an :class:`icecube.icetray.memory.Snapshot` object

    Args:
        snap (Snapshot): The snapshot object.
    """
    print('{:<40} | {:>10}'.format('Label','Size'))
    print('{:-<53}'.format(''))
    total = 0
    for k,v in snap.items():
        total += v
        print('{:<40} | {:>10d}'.format(k,v))
    print('{:-<53}'.format(''))
    print('{:<40} | {:>10d}'.format('Total',total))


def graph_timeline(timeline, filename, log=False, limit=10, exclude=None):
    """
    Graph a MemoryTimeline object

    Args:
        timeline (MemoryTimeline): The :class:`icecube.icetray.memory.MemoryTimeline` object.
        filename (str): A filename to write to.
        log (bool): Make the y-axis log scale.
        limit (int): Number of lines to display (from highest to lowest).
        exclude (iterable): Iterable of names to exclude.
    """
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    if not exclude:
        exclude = {}

    fig = plt.figure()
    ax = fig.add_subplot(111)
    ax.set_ylabel('Memory (MB)')
    ax.set_xlabel('Time (s)')

    series = {}
    for ts in timeline:
        t = float(ts.first)
        for k,v in ts.second.items():
            if k in exclude:
                continue
            if k not in series:
                series[k] = {'t':[],'v':[]}
            series[k]['t'].append(t*1.0/1000000)
            series[k]['v'].append(v*1.0/1000000)
    highest_series = sorted(series,key=lambda k:max(series[k]['v']),reverse=True)[:limit]
    
    max_mem = max(series[highest_series[0]]['v'])
    ax.set_ylim([1 if log else 0,int(max_mem*1.1)])

    plot = getattr(ax,'semilogy') if log else getattr(ax,'plot')
    for k in highest_series:
        plot(series[k]['t'],series[k]['v'],label=k)
    
    # Shrink current axis's height by 20% on the bottom
    #box = ax.get_position()
    #ax.set_position([box.x0, box.y0 + box.height * 0.4,
    #                 box.width, box.height * 0.6])

    # Put a legend below current axis
    #lgd = ax.legend(loc='upper center', bbox_to_anchor=(0.5, -0.05),
    #                fancybox=False, shadow=False, ncol=1)
    lgd = ax.legend(bbox_to_anchor=(1.05, 1), loc=2, borderaxespad=0.)

    plt.savefig(filename, dpi=300, bbox_extra_artists=(lgd,), bbox_inches='tight')

try:
    import tracemalloc
    import linecache
except ImportError:
    pass
else:
    def tracemalloc_display_highest(snapshot, group_by='lineno', limit=10):
        snapshot = snapshot.filter_traces((
            tracemalloc.Filter(False, "<frozen importlib._bootstrap>"),
            tracemalloc.Filter(False, "<frozen importlib._bootstrap_external>"),
            tracemalloc.Filter(False, "<unknown>"),
        ))
        top_stats = snapshot.statistics(group_by)

        print("Top %s lines" % limit)
        for index, stat in enumerate(top_stats[:limit], 1):
            frame = stat.traceback[0]
            # replace "/path/to/module/file.py" with "module/file.py"
            filename = os.sep.join(frame.filename.split(os.sep)[-2:])
            print("#%s: %s:%s: %.1f KiB"
                  % (index, filename, frame.lineno, stat.size / 1024))
            line = linecache.getline(frame.filename, frame.lineno).strip()
            if line:
                print('    %s' % line)

        other = top_stats[limit:]
        if other:
            size = sum(stat.size for stat in other)
            print("%s other: %.1f KiB" % (len(other), size / 1024))
        total = sum(stat.size for stat in top_stats)
        print("Total allocated size: %.1f KiB" % (total / 1024))
