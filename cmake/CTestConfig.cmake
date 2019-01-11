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
SET(CTEST_PROJECT_NAME ${META_PROJECT})
SET(CTEST_DROP_METHOD "xmlrpc")
SET(CTEST_DROP_SITE "http://code.icecube.wisc.edu:8081")
SET(CTEST_DROP_LOCATION ${META_PROJECT})
SET(CTEST_COMPRESS_SUBMISSION ON)

SET(CTEST_CONTINUOUS_DURATION_INTERVAL 600)
SET(CTEST_CONTINUOUS_MINIMUM_INTERVAL 1)

SET(CTEST_CUSTOM_MAXIMUM_NUMER_OF_ERRORS 1000)
SET(CTEST_CUSTOM_MAXIMUM_NUMER_OF_WARNINGS 1000)
SET(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 49152)
SET(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 49152)

