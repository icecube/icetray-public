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
#ifndef DATAIO_SHOVEL_COLOR_H_INCLUDED
#define DATAIO_SHOVEL_COLOR_H_INCLUDED

#include <boost/preprocessor.hpp>
#include <ncurses.h>
#include <map>
#include <string>

using namespace std;

struct color_pair 
{
  int index, pair;
  bool bold;
  bool reverse;
  bool dim;
};

extern std::map<std::string, color_pair> color_map;

color_pair ColorParse(std::string in_color);

void settext(color_pair cp);
void settext(WINDOW* win, color_pair cp);

#define ALL_COLORS (black)(red)(green)(yellow)(blue)(magenta)(cyan)(white)

#define MAKE_COLOR(r,data,color)					\
  const static color_pair color = ColorParse(BOOST_PP_STRINGIZE(color)); \
  const static color_pair BOOST_PP_CAT(hi_, color) = ColorParse("hi_" BOOST_PP_STRINGIZE(color)); \
  const static color_pair BOOST_PP_CAT(rev_, color) = ColorParse("rev_" BOOST_PP_STRINGIZE(color));\
  const static color_pair BOOST_PP_CAT(dim_, color) = ColorParse("dim_" BOOST_PP_STRINGIZE(color));

BOOST_PP_SEQ_FOR_EACH(MAKE_COLOR, ~, ALL_COLORS);

#endif
