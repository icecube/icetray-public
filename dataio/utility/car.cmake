#
# $Id$
#

# This utility macro extracts the first argument from the list of
# arguments given, and places it into the variable named var.
#
#   car(var arg1 arg2 ...)
macro(car var)
  set(${var} ${ARGV1})
endmacro(car)
