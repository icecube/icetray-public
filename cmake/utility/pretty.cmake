#
# The following functions/macros were originally taken from
# boost-cmake/tools/build/CMake/BoostUtils.cmake and had the following
# copyright/license notice
#

##########################################################################
# Boost Utilities                                                        #
##########################################################################
# Copyright (C) 2007 Douglas Gregor <doug.gregor@gmail.com>              #
# Copyright (C) 2007 Troy Straszheim                                     #
#                                                                        #
# Distributed under the Boost Software License, Version 1.0.             #
# See accompanying file LICENSE_1_0.txt or copy at                       #
#   http://www.boost.org/LICENSE_1_0.txt                                 #
##########################################################################

#
# A big shout out to the cmake gurus @ compiz
#

# colormsg("Colors:"  
#   WHITE "white" GRAY "gray" GREEN "green" 
#   RED "red" YELLOW "yellow" BLUE "blue" MAG "mag" CYAN "cyan" 
#   _WHITE_ "white" _GRAY_ "gray" _GREEN_ "green" 
#   _RED_ "red" _YELLOW_ "yellow" _BLUE_ "blue" _MAG_ "mag" _CYAN_ "cyan" 
#   _HIWHITE_ "white" _HIGRAY_ "gray" _HIGREEN_ "green" 
#   _HIRED_ "red" _HIYELLOW_ "yellow" _HIBLUE_ "blue" _HIMAG_ "mag" _HICYAN_ "cyan" 
#   HIWHITE "white" HIGRAY "gray" HIGREEN "green" 
#   HIRED "red" HIYELLOW "yellow" HIBLUE "blue" HIMAG "mag" HICYAN "cyan" 
#   "right?")

function (colormsg)
  if($ARGC EQUAL 1)
    message(STATUS $ARGV0)
  else($ARGC EQUAL 1)
  string (ASCII 27 _escape)
  set(WHITE "29")
  set(GRAY "30")
  set(RED "31")
  set(GREEN "32")
  set(YELLOW "33")
  set(BLUE "34")
  set(MAG "35")
  set(CYAN "36")

  foreach (color WHITE GRAY RED GREEN YELLOW BLUE MAG CYAN)
    set(HI${color} "1\;${${color}}")
    set(LO${color} "2\;${${color}}")
    set(_${color}_ "4\;${${color}}")
    set(_HI${color}_ "1\;4\;${${color}}")
    set(_LO${color}_ "2\;4\;${${color}}")
  endforeach()

  set(str "")
  set(coloron FALSE)
  foreach(arg ${ARGV})
    if (NOT ${${arg}} STREQUAL "")
      if (CMAKE_COLOR_MAKEFILE)
	set(str "${str}${_escape}[${${arg}}m")
	set(coloron TRUE)
      endif()
    else()
      set(str "${str}${arg}")
      if (coloron)
	set(str "${str}${_escape}[0m")
	set(coloron FALSE)
      endif()
      set(str "${str} ")
    endif()
  endforeach()
  message(STATUS ${str})
  endif($ARGC EQUAL 1)
endfunction()

#
#  pretty-prints the value of a variable so that the 
#  equals signs align
#
function(boost_report_value NAME)
  string(LENGTH "${NAME}" varlen)
  math(EXPR padding_len 30-${varlen})
  string(SUBSTRING "                                      " 
    0 ${padding_len} varpadding)
  colormsg("${NAME}${varpadding} = ${${NAME}}")
endfunction()

function(trace NAME)
  if(BOOST_CMAKE_TRACE)
    string(LENGTH "${NAME}" varlen)
    math(EXPR padding_len 40-${varlen})
    string(SUBSTRING "........................................"
      0 ${padding_len} varpadding)
    message("${NAME} ${varpadding} ${${NAME}}")
  endif()
endfunction()  

#
#  pretty-prints the value of a variable so that the 
#  equals signs align
#
function(boost_report_pretty PRETTYNAME VARNAME)
  string(LENGTH "${PRETTYNAME}" varlen)
  math(EXPR padding_len 30-${varlen})
  string(SUBSTRING "                                      " 
    0 ${padding_len} varpadding)
  message(STATUS "${PRETTYNAME}${varpadding} = ${${VARNAME}}")
endfunction()
