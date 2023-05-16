#!/usr/bin/env python3

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
        from matplotlib import colors, cm  # noqa: F401
        from matplotlib.colors import LinearSegmentedColormap
    except ImportError:
        print("Could not make custom maps: ")
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

    try:
        # matplotlib 3.7+
        from matplotlib import colormaps
        colormaps.register(LinearSegmentedColormap('BlueRed1', cdict1))
        colormaps.register(LinearSegmentedColormap('Belgium',  cdict2))
    except ImportError:
        # at least matplotlib 2.2.4, maybe earlier
        cm.register_cmap(cmap=LinearSegmentedColormap('BlueRed1', cdict1))
        cm.register_cmap(cmap=LinearSegmentedColormap('Belgium',  cdict2))

def get_matplotlib_colormaps():
    try:
        from matplotlib import cm
    except ImportError:
        print("Could not import matplotlib, so won't grab any color maps: ")
        return []

    make_custom_maps()

    ignore = set()
    for r in _no_reverse:
        ignore.add(r[:-2] if r.endswith("_r") else r + "_r")

    ret = []

    try:
        # matplotlib 3.7+
        from matplotlib import colormaps
        cmaps = sorted(colormaps)
        def get_cmap_name(name):
            return colormaps[name]
    except ImportError:
        cmaps = sorted(cm.cmap_d)
        def get_cmap_name(name):
            return cm.get_cmap(name)

    for name in cmaps:
        # avoid ugly maps and their reversals
        name_base = name[:-2] if name.endswith("_r") else name
        if name_base in _bad_maps or name in ignore:
            continue
        m = get_cmap_name(name)
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


if __name__ == "__main__":
    #from pprint import pprint
    import matplotlib

    try:
        c = get_matplotlib_colormaps()
        #pprint(c)
    except Exception as e:
        import sys
        print(f"{matplotlib.__version__=}")
        print("Something failed: ", e)
        sys.exit(1)
