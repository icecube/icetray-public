#!/usr/bin/env python

from multiprocessing import Process
import signal, os
from icecube import icetray, dataio
from I3Tray import I3Tray

def hangy():
	icetray.logging.set_level_for_unit('I3Module', 'TRACE')
	tray = I3Tray()
	tray.Add("I3InfiniteSource")
	tray.Add("TrashCan")
	tray.Execute(1)

if __name__ == "__main__":
	p = Process(target=hangy)
	p.start()
	p.join(1)
	if p.is_alive():
		os.kill(p.pid, signal.SIGKILL)
		p.terminate()
		raise RuntimeError("tray got stuck")
