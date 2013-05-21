# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

foreach(item basic)
    ADD_EXECUTABLE(${item} "${item}.cpp")
    target_link_libraries(${item} ${SEEXPR_LIBRARIES})
    install(TARGETS ${item} DESTINATION test)
endforeach(item)
