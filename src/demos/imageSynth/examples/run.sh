#!/bin/sh

RES=256
for i in *.se; do
    FNAME=`basename $i .se`.png;
    echo Generating $FNAME;
    imageSynth $FNAME $RES $RES $i;
done
