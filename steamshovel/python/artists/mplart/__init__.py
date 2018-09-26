try:
    from .QtMPLArtist import QtMPLArtist as MPLArtist
except ImportError:
    from .AggMPLArtist import AggMPLArtist as MPLArtist
