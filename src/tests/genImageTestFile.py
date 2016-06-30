#!/usr/bin/env python
# Copyright Disney Enterprises, Inc.  All rights reserved.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License
# and the following modification to it: Section 6 Trademarks.
# deleted and replaced with:
# 
# 6. Trademarks. This License does not grant permission to use the
# trade names, trademarks, service marks, or product names of the
# Licensor and its affiliates, except as required for reproducing
# the content of the NOTICE file.
# 
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0


import sys
import os
import re

###################################
def printUsage():
    print "Usage: genImageTestFile.py <dir> <outfile>"
    print "       traverse given dir for expr examples, write tests to outfile\n"
    print "       ex: genImageTestFile.py ./src/demos/imageSynth/examples src/tests/testSeExprExamples.cpp"
    sys.exit()
###################################

### MAIN ###

if len(sys.argv) < 3:
    printUsage()

# get args
rootdir = sys.argv[1]
outfile = sys.argv[2]

# open outfile
f = open(outfile, 'w')
print >> f, "#include <gtest/gtest.h>\n"
print >> f, "void evalExpressionFile(const char *filepath);\n"

for dir_name, sub_dirs, se_files in os.walk(rootdir):
    for se_file in se_files:
        fullpath = os.path.join(dir_name, se_file)
        # use parent_dir and gparent_dir for test name to avoid duplicates
        (head, parent_dir) = os.path.split(dir_name)
        (head, gparent_dir) = os.path.split(head)
        (filename,ext) = os.path.splitext(se_file)
        # strip out invalid chars
        filename = filename.translate(None, " &.#")
        if(re.match('\.se$', ext)):
            print >> f, "TEST(" + parent_dir.lstrip('.') +'_' + gparent_dir.lstrip('.') + ", " + filename + ")"
            print >> f, "{"
            print >> f, "    evalExpressionFile(\"" + fullpath + "\");"
            print >> f, "}\n"

f.close()
