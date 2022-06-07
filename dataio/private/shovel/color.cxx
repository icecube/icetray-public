/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 *  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 *  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 *  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 *  SUCH DAMAGE.
 *  
 *  SPDX-License-Identifier: BSD-2-Clause
 *  
 */
#include <ncurses.h>

#include <shovel/color.h>

using namespace std;

map<string, color_pair> color_map;

void 
settext(color_pair cp)
{
  standend();
  attron(cp.pair);
}

void 
settext(WINDOW* win, color_pair cp)
{
  standend();
  wattron(win, cp.pair);
}

color_pair ColorParse(string in_color) 
{
  string clr = in_color;
  color_pair ret;

  // First, find if theres a hi-
  if (clr.substr(0, 3) == "hi_") {
    ret.bold = true;
    clr = clr.substr(3, clr.length() - 3);
  } else {
    ret.bold = false;
  }

  // First, find if theres a hi-
  if (clr.substr(0, 4) == "rev_") {
    ret.reverse = true;
    clr = clr.substr(4, clr.length() - 4);
  } else {
    ret.reverse = false;
  }

  // First, find if theres a hi-
  if (clr.substr(0, 4) == "dim_") {
    ret.dim = true;
    clr = clr.substr(4, clr.length() - 4);
  } else {
    ret.dim = false;
  }

  // Then match all the colors
  if (clr == "black")
    ret.index = COLOR_BLACK;
  else if (clr == "red")
    ret.index = COLOR_RED;
  else if (clr == "green")
    ret.index = COLOR_GREEN;
  else if (clr == "yellow")
    ret.index = COLOR_YELLOW;
  else if (clr == "blue")
    ret.index = COLOR_BLUE;
  else if (clr == "magenta")
    ret.index = COLOR_MAGENTA;
  else if (clr == "cyan")
    ret.index = COLOR_CYAN;
  else if (clr == "white")
    ret.index = COLOR_WHITE;
  else // black by default
    ret.index = COLOR_BLACK;

  if (ret.index != -1) {
    ret.pair = COLOR_PAIR(ret.index);

    if (ret.bold)
      ret.pair |= A_BOLD;
    else
      ret.pair |= A_NORMAL;

    if (ret.reverse)
      ret.pair |= A_STANDOUT;

    if (ret.dim)
      ret.pair |= A_DIM;

  } else {
    ret.pair = 0;
  }

  return ret;
}
