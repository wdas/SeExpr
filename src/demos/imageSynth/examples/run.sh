#!/bin/sh

# (c) Disney Enterprises, Inc.  All rights reserved.
#
# This file is licensed under the terms of the Microsoft Public License (MS-PL)
# as defined at: http://opensource.org/licenses/MS-PL.
#
# A complete copy of this license is included in this distribution as the file
# LICENSE.

RES=256
for i in *.se; do
    FNAME=`basename $i .se`.png;
    echo Generating $FNAME;
    imageSynth $FNAME $RES $RES $i;
done
