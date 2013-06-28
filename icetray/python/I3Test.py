import sys

def ENSURE(cond, msg):
	if not cond :
		print(msg)
		print("FAIL")
		sys.exit(1)

def ENSURE_EQUAL(x,y, msg):
	if not x == y :
		print(msg)
		print("FAIL")
		sys.exit(1)

def ENSURE_DISTANCE(x,y, d, msg):
	if abs(x - y) > d :
		print(msg)
		print("FAIL")
		sys.exit(1)

