import math
import re
from icecube.shovelart import Vec3dList, vec3d
from icecube import dataclasses
from icecube import icetray
I3Units = icetray.I3Units

# imports may not fail, since util is used by everybody
try: import numpy as np
except ImportError: pass
try:
    from matplotlib import patches as mpatches
    from matplotlib import transforms as mtransforms
except ImportError:
    pass


# CamelCase because it behaves like a class
def TankPositionGenerator(geometry):
    """
    Returns the OMKeys and positions of IceTop tanks.
    """
    # for omkey, geo in geometry.omgeo:
    #     if 60 < omkey.om < 65:
    #         yield omkey, geo.position
    for station in geometry.stationgeo.values():
        for tank in station:
            yield tank.omkey_list[0], tank.position
            yield tank.omkey_list[1], tank.position


def parse_engineering( string, unit = "" ):
    """
    Parses a string in engineering notation (k,M,m,...).

    :param string: string to parse
    :param unit: unit to expect, if it is not there, raise ValueError
    :returns: parsed float

    Use u for micro. We cannot parse the mu sign since
    we are limited to ASCII.
    """
    if not string.endswith(unit):
        raise ValueError("string '%s' is missing the unit '%s'" % (string, unit))
    if unit:
        string = string[:-len(unit)]

    m = re.match(r"\s*([\+\-]?[.0-9]+)\s*([a-zA-Z]*)\s*", string)
    if not m:
        raise ValueError("string '%s' cannot be parsed" % string)
    x = m.group(1)
    mod = m.group(2)
    conv = {'a':1e-18, 'f':1e-15, 'p':1e-12, 'n':1e-9, 'u':1e-6,
            'm':1e-3 , 'c':1e-2 , 'd':1e-1 ,  '':1.0 , 'k':1e3 ,
            'M':1e6  , 'G':1e9  , 'T':1e12 , 'P':1e15, 'E':1e18}
    return float(x) * conv[mod]


def format_engineering( number, unit = "" ):
    """
    Writes a string in engineering notation (k,M,m,...).

    :param number: number to format
    :param unit: optional unit string to append
    :returns: formatted string

    Should be the inverse of parse_engineering(...).
    """
    if math.isnan(number):
        return "nan"
    if number == 0.0:
        return 0

    convert_table = {-18:'a', -15:'f', -12:'p', -9:'n', -6:'u',
                      -3:'m',  -2:'c',  -1:'d',  0:'',   3:'k',
                       6:'M',   9:'G',  12:'T', 15:'P', 18:'E'}
    l10 = math.log10(abs(number))
    ten_exp = int(l10)

    sci_places = int(ten_exp / 3) * 3
    sci_signific = (ten_exp % 3)

    expo_char = convert_table[sci_places]
    trailing = number / 10.0 ** sci_places

    # print z, ten_exp, sci_places, sci_signific
    if trailing >= 10:
        lead = "{:d}".format(int(round(trailing)))
    elif trailing >= 1:
        lead = "{:.1f}".format(trailing)
    else:
        lead = "{:.2f}".format(trailing)
    return lead + " " + expo_char + unit


def detector_outline( bottom_vec3d_list, top_vec3d_list ):
    """
    Returns detector outline by detecting points at the border.

    Only works for standard hexagon grid.
    """
    # hardcoded angular offset for hexagon
    phi0 = -20.0 * I3Units.degree 

    # hardcoded threshold for an edge
    cos_angle_threshold = math.cos( 7.0 * I3Units.degree ) 

    bottom = Vec3dList()
    top = Vec3dList()

    string_coords = []
    for b, t in zip( bottom_vec3d_list, top_vec3d_list ):
        if t[2] < 450.0 * I3Units.meter: # ignore deep-core
            continue
        string_coords.append(( math.atan2(t[1], t[0]),
                               t[0], t[1], b[2], t[2] ))

    # border detection:
    # check if there is a point in each angular segment of hexagon
    border = []
    for i, cur in enumerate( string_coords ):
        counts = [False, False, False, False, False , False]
        for j, other in enumerate( string_coords ):
            if i == j: continue
            dx = cur[1] - other[1]
            dy = cur[2] - other[2]
            phi = int((math.atan2( dy, dx ) - phi0) / I3Units.degree)
            if phi < 0:
                phi += 360
            counts[phi // 60] = True
        neighbor_count = sum( counts )
        # border points don't have a full hexagon of neighbors
        if neighbor_count < 6:
            border.append( cur )

    border.sort() # put in circular order

    # edge detection:
    # check if differential vectors of three consecutive points have an angle
    for i in range( len(border) ):
        ax = border[i - 1][1] - border[i - 2][1]
        ay = border[i - 1][2] - border[i - 2][2]
        bx = border[i][1] - border[i - 1][1]
        by = border[i][2] - border[i - 1][2]
        anorm = (ax ** 2 + ay ** 2) ** 0.5
        bnorm = (bx ** 2 + by ** 2) ** 0.5
        cos_angle = (bx * ax  + by * ay) / (anorm * bnorm)
        if cos_angle < cos_angle_threshold:
            cur = border[i - 1]
            bottom.append( vec3d(cur[1], cur[2], cur[3]) )
            top.append( vec3d(cur[1], cur[2], cur[4]) )

    return bottom, top


def particle_pos(particle, time):
    """
    Returns the position of a particle at the given time.
    """
    return particle.pos + particle.dir * particle.speed * (time - particle.time)


def to_shower_cs(i3direction):
    """
    Returns rotation matrix to shower CS for given i3direction.

    Requires numpy.
    """
    cos = math.cos
    sin = math.sin
    phi = i3direction.phi
    theta = i3direction.theta
    cp = cos(phi)
    sp = sin(phi)
    ct = cos(theta)
    st = sin(theta)
    # counter-clockwise (pi + phi) rotation
    d_phi = np.matrix([[-cp, -sp, 0],
                       [sp , -cp, 0],
                       [0  ,   0, 1]])
    # clock-wise (pi - theta) rotation
    d_theta = np.matrix([[-ct, 0, -st],
                         [0  , 1,   0],
                         [st , 0, -ct]])
    return d_theta * d_phi


def signed_r_z(rotation_matrix, positions, core):
    """
    Returns signed lateral distance with r > 0 as early and r < 0 as late hits.
    For the rotation matrix, use to_shower_cs(...).

    Requires numpy.
    """
    arr = np.array
    pos = arr(positions) - core
    rot_pos = arr(rotation_matrix * pos.T).T
    x = rot_pos[:, 0].T
    y = rot_pos[:, 1].T
    z = rot_pos[:, 2].T
    r = (x ** 2 + y ** 2) ** 0.5 * np.sign(-x)
    return r, z


def propagate_covariance(f, x, cov):
    """
    Computes the covariance matrix of y for the transformation y = f(x),
    given x with covariance matrix cov.

    Requires numpy.

    :param f: function-like, has to be callable as f(x)
    :param x: array-like, vector of parameters
    :param cov: 2-d array of floats, covariance matrix of x

    :returns: fcov: matrix of floats, covariance matrix of the output of f

    Examples:
    >>> import numpy as np
    >>> v = np.ones(2)
    >>> cov = np.ones((2,2))
    >>> def f(r):return np.dot(r,r)
    >>> "%.3g" % propagate_covariance(f,v,cov)
    '16'
    >>> def f(r):return 2*r
    >>> propagate_covariance(f,v,cov)
    array([[ 4.,  4.],
         [ 4.,  4.]])

    Authors:
    Hans Dembinski <hans.dembinski@kit.edu>
    """
    ncol = len(x)
    diag = np.diag(cov)
    dx = diag ** 0.5 * 1e-3
    dx[diag == 0] = 1e-6
    jacobi = jacobian(f, x, dx)
    return np.dot(jacobi, np.dot(cov, jacobi.T))


def jacobian(f, x, dx):
    """
    Numerically calculate matrix of first derivatives.
    Needed for error propagation.

    Requires numpy.

    :param f: function-like, has to be callable as f(x)
    :param x: array-like, vector of parameters
    :param steps: array-like (optional), vector of deltas to use in numerical approximation

    :returns: Jacobi matrix of first derivatives

    Examples:
    >>> def f(v): return 0.5*np.dot(v,v)
    >>> jacobian(f,np.ones(2))
    array([[ 1.,  1.]])
    >>> def f(v): return np.dot(v,v)*v
    >>> jacobian(f,np.ones(2))
    array([[ 4.,  2.],
           [ 2.,  4.]])

    Authors:
    Hans Dembinski <hans.dembinski@kit.edu>
    """
    x = np.atleast_1d(x)
    dx = np.atleast_1d(dx)
    nx = len(x)
    ny = 0
    jacobi = None
    e = np.zeros(nx)
    for ix in range(nx):
        e *= 0
        e[ix] = 1
        deriv = np.atleast_1d((f(x + e * dx) - f(x - e * dx)) / (2 * dx[ix]))
        if ix == 0:
            ny = len(deriv)
            jacobi = np.empty((ny, nx))
        jacobi[:, ix] = deriv
    return jacobi


def cornertext(text, loc=2, color=None, frameon=False,
               axes=None, borderpad=None, borderaxespad=None, handletextpad=None,
               **kwargs):
    """
    Conveniently places text in a corner of an MPL plot. Mimics pyplot.legend(...).
    Requires matplotlib.

    :param text: string or tuple of strings
      Text to be placed in the plot. May be a tuple of strings to get
      several lines of text.
    :param loc: integer or string
      Location of text, same as in legend(...).
    :param frameon: boolean (optional)
      Whether to draw a border around the text. Default is False.
    :param axes: Axes (optional, default: None)
      Axes object which houses the text (defaults to the current axes).

    Other keyword arguments are forwarded to the text instance.

    Authors:
    Hans Dembinski <hans.dembinski@icecube.wisc.edu>
    """

    from matplotlib.offsetbox import AnchoredOffsetbox, VPacker, TextArea
    from matplotlib import rcParams
    from matplotlib.font_manager import FontProperties
    import warnings

    if axes is None:
        from matplotlib import pyplot as plt
        axes = plt.gca()

    locTranslate = {
        'upper right': 1,
        'upper left': 2,
        'lower left': 3,
        'lower right': 4,
        'right': 5,
        'center left': 6,
        'center right': 7,
        'lower center': 8,
        'upper center': 9,
        'center': 10
    }

    if isinstance(loc, str):
        if loc in locTranslate:
            loc = locTranslate[loc]
        else:
            message = ('Unrecognized location "%s". '
                       'Falling back on "upper left"; '
                       'valid locations are\n\t%s') \
                       % (loc, '\n\t'.join(locTranslate.keys()))
            warnings.warn(message)
            loc = 2

    if borderpad is None:
        borderpad = rcParams["legend.borderpad"]

    if borderaxespad is None:
        borderaxespad = rcParams["legend.borderaxespad"]

    if handletextpad is None:
        handletextpad = rcParams["legend.handletextpad"]

    if "fontproperties" in kwargs:
        fontproperties = kwargs["fontproperties"]
        del kwargs["fontproperties"]
    else:
        if "size" in kwargs:
            size = kwargs["size"]
            del kwargs["size"]
        elif "fontsize" in kwargs:
            size = kwargs["fontsize"]
            del kwargs["fontsize"]
        else:
            size = rcParams["legend.fontsize"]
        fontproperties = FontProperties(size=size)

    texts = [text] if isinstance(text, str) else text

    colors = [color for t in texts] if (
        isinstance(color, str) or color is None) else color

    tas = []
    for t, c in zip(texts, colors):
        ta = TextArea(t,
                      textprops={"color": c,
                                 "fontproperties": fontproperties},
                      multilinebaseline=True,
                      minimumdescent=True,
                      **kwargs)
        tas.append(ta)

    vpack = VPacker(children=tas, pad=0, sep=handletextpad)

    aob = AnchoredOffsetbox(loc, child=vpack,
                            pad=borderpad,
                            borderpad=borderaxespad,
                            frameon=frameon)

    axes.add_artist(aob)
    return aob


def mpl_hist(x, y, **kwargs):
    m = len(y)
    n = 2 * m + 2

    xy = np.zeros((n, 2))
    xy[0][0] = x[0]
    xy[-1][0] = x[-1]
    for i in range(m):
        xy[1 + 2 * i][0] = x[i]
        xy[1 + 2 * i][1] = y[i]
        xy[1 + 2 * i + 1][0] = x[i + 1]
        xy[1 + 2 * i + 1][1] = y[i]

    return mpatches.Polygon(xy, **kwargs)


def rainbow_text(axes, x, y, strings, colors, **kwargs):
    """
    Draws a sequence of strings in various colors.

    Based on http://stackoverflow.com/questions/9169052.
    """
    fontweights = None
    if "fontweights" in kwargs:
        fontweights = kwargs["fontweights"]
        del kwargs["fontweights"]

    renderer = axes.figure.canvas.get_renderer()
    for i, s in enumerate(strings):
        kwargs["color"] = colors[i]
        if fontweights is not None:
            kwargs["fontweight"] = fontweights[i]
        tx = axes.text(x, y, s, **kwargs)
        tx.draw(renderer)
        ex = tx.get_window_extent()
        kwargs["transform"] = mtransforms.offset_copy(tx._transform,
                                                      x=ex.width,
                                                      units='dots')
