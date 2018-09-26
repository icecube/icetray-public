# The list of matplotlib color maps considered to be not useful in steamshovel
_bad_maps = ['Accent', 'Dark2', 'Paired', 'Pastel1', 'Pastel2',
             'Set1', 'Set2', 'Set3', 'gist_earth', 'gist_ncar',
             'gist_rainbow', 'gist_stern', 'hsv', 'spectral',
             'terrain', 'binary', 'cool', 'jet',
             # The following maps aren't ugly per se, but have a 3-color, mid-centered
             # color structure that not useful for continuously varying time
             # data
             'BrBG', 'PRGn', 'PiYG', 'PuOr', 'RdBu', 'RdGy',
             'RdYlBu', 'RdYlGn', 'Spectral', 'brg', 'bwr', 'coolwarm', 'seismic']

# List of maps that should be used, but without including their reversals.
_no_reverse = ["Blues", "BuGn", "BuPu", "GnBu", "OrRd", "Oranges", "Greens", "Greys",
               "PuBu", "PuBuGn", "PuRd", "Purples", "RdPu", "Reds", "YlGn", "YlGnBu",
               "YlOrBr", "YlOrRd", "autumn_r", "binary", "bone_r", "copper_r",
               "gray_r", "hot_r", "jet_r", "pink_r", "spring_r", "summer_r", "winter_r"]


def make_custom_maps():
    # adapted from
    # http://matplotlib.org/examples/pylab_examples/custom_cmap.html
    try:
        from matplotlib import colors, cm
        from matplotlib.colors import LinearSegmentedColormap
    except ImportError as e:
        print("Could not make custom maps")
        return

    cdict1 = {'red':   ((0.0, 0.0, 0.0),
                        (0.5, 0.0, 0.1),
                        (1.0, 1.0, 1.0)),

              'green': ((0.0, 0.0, 0.0),
                        (1.0, 0.0, 0.0)),

              'blue':  ((0.0, 0.0, 1.0),
                        (0.5, 0.1, 0.0),
                        (1.0, 0.0, 0.0))
              }

    cdict2 = {'red':   ((0.0, 0.0, 0.0),
                        (0.35, 1.0, 1.0),
                        (0.65, 1.0, 1.0),
                        (1.0, 1.0, 1.0)),

              'green': ((0.0, 0.0, 0.0),
                        (0.35, 0.95, 0.95),
                        (0.65, 0.95, 0.95),
                        (1.0, 0.0, 0.0)),

              'blue':  ((0.0, 0.0, 0.1),
                        (0.5, 0.0, 0.0),
                        (1.0, 0.0, 0.0))
              }

    cm.register_cmap(name='BlueRed1', data=cdict1)
    cm.register_cmap(name='Belgium', data=cdict2)


def get_matplotlib_colormaps():
    try:
        from matplotlib import cm
    except ImportError as e:
        print("Could not import matplotlib, so won't grab any color maps")
        return []

    make_custom_maps()

    ignore = set()
    for r in _no_reverse:
        ignore.add(r[:-2] if r.endswith("_r") else r + "_r")

    ret = []
    for name in sorted(cm.cmap_d):
        # avoid ugly maps and their reversals
        name_base = name[:-2] if name.endswith("_r") else name
        if name_base in _bad_maps or name in ignore:
            continue
        m = cm.get_cmap(name)
        # warning: this code uses a private data member,
        # this unsafe and may lead to conflicts in the future
        if not hasattr(m, '_segmentdata'):
            continue
        sd = m._segmentdata
        rgb = sd['red'], sd['green'], sd['blue']
        # Don't export maps whose colors are functions
        # instead of interpolable values
        if(any(callable(x) for x in rgb)):
            continue
        # turn unicode into string
        try:
            if type(name) == unicode:
                name = str(name)
        except NameError:
            pass # python3 doesn't have 'unicode' type
        ret.append((name, rgb))

    return ret
