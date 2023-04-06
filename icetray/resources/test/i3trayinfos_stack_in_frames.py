#!/usr/bin/env python3

from pathlib import Path
from tempfile import NamedTemporaryFile

from I3Tray import I3Tray
from icecube import dataio, icetray
from icecube.icetray.i3logging import log_debug

# silence I3Tray
icetray.logging.set_level_for_unit("I3Tray", icetray.I3LogLevel.LOG_WARN)

foo = Path(NamedTemporaryFile(delete=False).name)
tray = I3Tray()
tray.Add("BottomlessSource")
tray.Add("I3Writer", Filename=str(foo))
tray.Execute(1)

foonew = Path(NamedTemporaryFile(delete=False).name)
for _i in range(11):
    tray = I3Tray()
    tray.Add("I3Reader", Filename=str(foo))
    tray.Add("I3Writer", Filename=str(foonew))
    tray.Execute()
    foo.unlink()
    foonew.rename(foo)

assert foo.exists(), "lost our tempfile!"
f = dataio.I3File(str(foo))

# should be our info frame
frame = f.pop_frame(icetray.I3Frame.TrayInfo)
log_debug(str(frame))
assert len(frame.keys()) == 11, "eleven keys expected in frame"

# should be our physics frame
frame = f.pop_frame()
log_debug(str(frame))
assert not frame, "only one frame expected in file"

# clean up our tempfiles
for fname in [foo, foonew]:
    if fname.exists():
        fname.unlink()
    assert not fname.exists(), "Unable to remove tempfile {str(fname)}"
