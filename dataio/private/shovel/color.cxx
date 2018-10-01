/**
 *  $Id$
 *  
 *  Copyright (C) 2007
 *  Troy D. Straszheim  <troy@icecube.umd.edu>
 *  and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *  
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
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
