/**
 *  $Id$
 *  
 *  Copyright (C) 2008
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
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <cxxabi.h>

/* Obtain a backtrace and print it to stdout. */
void
print_backtrace (FILE* stream, unsigned depth)
{
  void *array[depth];
  size_t size;
  size_t i;

  size = backtrace (array, depth);

  printf ("Obtained %zd stack frames.\n", size);

  Dl_info info[size];
  int status[size];

  const char* const notavailable = "N/A";

  for (int i=1; i < size && array[i] != 0; i++)
    {
      status[i] = dladdr(array[i], &info[i]);
      int demangle_status;
      char* realname = abi::__cxa_demangle(info[i].dli_sname, 0, 0, &demangle_status);

      fprintf(stream, "%s : %s\n", 
	      info[i].dli_fname,
	      demangle_status == 0 ? realname : info[i].dli_sname);
      if (realname)
	free(realname);
    }

  fprintf(stream, "\n\nEnd of backtrace.\nYou are welcome to hit .qqqqqqqq at this time.\n\n");
}

