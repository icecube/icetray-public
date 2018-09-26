#! /usr/bin/env python


def GetArtist(arg):
    import os
    import imp
    import re

    # locate and load module
    if arg.endswith(".py") and os.path.exists(arg):
        pieces = os.path.split(arg)
        name = os.path.splitext(pieces[-1])[0]
        path = arg
        module = imp.load_source(name, path)
    else:
        import sys
        for path2 in sys.path:
            path = os.path.join(
                path2, "icecube", "steamshovel", "artists", arg + ".py")
            if os.path.exists(path):
                module = imp.load_source(arg, path)
                break
        else:
            raise StandardError("Could not locate module")

    # find artist class and make instance
    from icecube.steamshovel.artists.mplart import MPLArtist
    for var in dir(module):
        obj = module.__dict__[var]
        if issubclass(obj, MPLArtist.MPLArtist) and not obj is MPLArtist.MPLArtist:
            cl = obj
            break
    else:
        raise StandardError("Could not find subclass of MPLArtist")

    return cl()


def GetFrame(fileList, nFrame):
    from icecube.shovelio import I3FileGroup, I3File
    import os
    fg = I3FileGroup()
    for f in fileList:
        nf = os.path.expanduser(f)
        fg.addFile(I3File(nf))
    if nFrame < 0:
        nFrame = fg.size() + nFrame
    return fg.getFrame(nFrame)


def Draw(artist, frame, keys):
    from matplotlib import pyplot as plt
    # override keys method
    artist.keys = lambda: keys
    fig = plt.figure()
    artist.create_plot(frame, fig)
    plt.show()


def main():
    import argparse
    import types
    parser = argparse.ArgumentParser(
        description="Script to test and debug MPL artists without actually running steamshovel.")
    parser.add_argument(
        "artist", help="Name of artist. May be file path or a module name for standard artist.")
    parser.add_argument(
        "file", help="Comma-separated list of I3Files (put file with GCD frames first).")
    parser.add_argument(
        "num", type=int, help="Frame index. May be negative to count from the end (fails for incremental sources).")
    parser.add_argument(
        "--keys", help="Comma-separated list of keys to use for the artist.", default="")
    parser.add_argument(
        "--settings", help="Comma-separated key=value pairs of artist settings.", default="")

    args = parser.parse_args()
    fileList = args.file.split(",")
    keys = args.keys.split(",")
    settings = dict([kv.split("=")
                     for kv in args.settings.split(",") if "=" in kv])
    settings["TWUpdate"] = False

    artist = GetArtist(args.artist)

    # settings are only writable through a scenario object;
    # overriding setting method to pass our values
    artist.setting_ = artist.setting
    artist.settings_store = settings

    def setting(self, key):
        return self.settings_store[key] if key in self.settings_store else self.setting_(key)
    artist.setting = types.MethodType(setting, artist, artist.__class__)

    frame = GetFrame(fileList, args.num)
    Draw(artist, frame, keys)

if __name__ == "__main__":
    main()
