#!/usr/bin/env python3

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
	# with the advent of the "spawn" process model in python3.8 it takes marginally
	# longer for processes to start, so we increase our timeout here to 2
	p.join(2)
	if p.is_alive():
		os.kill(p.pid, signal.SIGKILL)
		p.terminate()
		raise RuntimeError("tray got stuck")
