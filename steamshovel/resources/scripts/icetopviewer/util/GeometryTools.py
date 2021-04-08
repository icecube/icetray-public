import numpy as np
from icecube.dataclasses import I3Constants

def get_radius(particle, pos):
    # Particle is the primary particle and pos is the detector position
    x_c = particle.pos.x
    y_c = particle.pos.y
    z_c = particle.pos.z

    nx = particle.dir.x
    ny = particle.dir.y

    abs_x_sq = (pos[0] - x_c) * (pos[0] - x_c) \
               + (pos[1] - y_c) * (pos[1] - y_c) \
               + (pos[2] - z_c) * (pos[2] - z_c)

    n_prod_x = nx * (pos[0] - x_c) \
               + ny * (pos[1] - y_c) \
               - np.sqrt(1. - nx * nx - ny * ny) * (pos[2] - z_c)

    return np.sqrt(abs_x_sq - n_prod_x * n_prod_x)


def ProjectToObslev(point, direction, obslev=(I3Constants.SurfaceElev - I3Constants.OriginElev)):
    # Simple geometric projection of a point to a given z value (obslev)
    return point + direction * (point.z - obslev) / np.cos(direction.zenith)
