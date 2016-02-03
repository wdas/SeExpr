#!/usr/bin/env python

import sys
import os
import re

###################################
def printUsage():
    print "Usage: genImageTestFile.py <dir> <outfile>"
    print "       traverse given dir for expr examples, write tests to outfile\n"
    print "       ex: genImageTestFile.py ./src/demos/imageSynth2/examples2 src/tests/testSeExprExamples.cpp"
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
print >> f, "#include <gtest.h>\n"
print >> f, "extern void evalExpressionFile(const char *filepath);\n"

for dir_name, sub_dirs, se_files in os.walk(rootdir):
    for se_file in se_files:
        fullpath = os.path.join(dir_name, se_file)
        parent_dir = os.path.basename(dir_name).lstrip('.')
        (filename,ext) = os.path.splitext(se_file)
        filename = filename.replace(" ", "_").replace("&", "_").replace('.', '_')
        if(re.match('\.se$', ext)):
            print >> f, "TEST(" + parent_dir + ", " + filename + ")"
            print >> f, "{"
            print >> f, "    evalExpressionFile(\"" + fullpath + "\");"
            print >> f, "}\n"

f.close()
