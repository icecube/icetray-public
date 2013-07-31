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
TOOLDEF (mysql
    include/mysql-4.1.20
    mysql/mysql.h
    lib/mysql-4.1.20/mysql
    NONE
    mysqlclient
    )

if (NOT MYSQL_FOUND)
  tooldef (mysql
    /usr/include
    mysql/mysql.h
    NONE
    NONE
    mysqlclient
    )
endif (NOT MYSQL_FOUND)