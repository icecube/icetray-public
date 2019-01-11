#
# $Id: cdr.cmake 165886 2018-10-01 14:37:58Z nwhitehorn $
#

# This utility macro extracts all of the arguments given except the
# first, and places them into the variable named var.
#
#   car(var arg1 arg2 ...)
macro(cdr var junk)
  set(${var} ${ARGN})
endmacro(cdr)
