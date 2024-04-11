#!/bin/sh
#
#  $Id$
#
#  Copyright (C) 2007   Troy D. Straszheim  <troy@icecube.umd.edu>
#  Copyright (C) 2007   the IceCube Collaboration <http://www.icecube.wisc.edu>
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.
#
#  SPDX-License-Identifier: BSD-2-Clause
#
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
setting PATH.  See:

  http://wiki.icecube.wisc.edu/index.php/Shell_Configuration_Tips

for detailed instructions.
"
fi

exit $ERROR
