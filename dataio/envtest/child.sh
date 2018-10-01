#!/bin/sh
#
#  $Id$
#  
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  and the IceCube Collaboration <http://www.icecube.wisc.edu>
#  
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#  

ERROR=0

if [ "$PATH" != "PATH_CHECK" ]
then
    echo "***\n*** Error: unable to prepend your PATH ***\n***"
    echo "PATH=$PATH"
    ERROR=1
fi

if [ "$PYTHONPATH" != "PYTHONPATH_CHECK" ]
then
    echo "***\n*** Error: unable to prepend your PYTHONPATH ***\n***"
    echo "PYTHONPATH=$PYTHONPATH"
    ERROR=1
fi

if [ "$LD_LIBRARY_PATH" != "LD_LIBRARY_PATH_CHECK" ]
then
    echo "***\n*** Error: unable to prepend your LD_LIBRARY_PATH ***\n***"
    echo "LD_LIBRARY_PATH=$LD_LIBRARY_PATH"
    ERROR=1
fi

if [ "$DYLD_LIBRARY_PATH" != "DYLD_LIBRARY_PATH_CHECK" ]
then
    echo "***\n*** Error: unable to prepend your DYLD_LIBRARY_PATH ***\n***"
    echo "DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH"
    ERROR=1
fi

if [ $ERROR -eq 1 ]
then
    echo "
Tests of your shell environment failed.  This is most probably due to
your shell initialization scripts (.bashrc, .tcshrc, or .zshenv)
settting PATH.  See 

  http://wiki.icecube.wisc.edu/index.php/Shell_Configuration_Tips

for detailed instructions.
"
fi

exit $ERROR