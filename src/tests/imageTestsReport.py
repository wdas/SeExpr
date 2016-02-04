#!/usr/bin/env python
# generate timing report for image tests against 3 versions of seexpr
# imageTestsReport.py 2> /tmp/imageTests.csv

import sys
import os
import re

# test_case[test_name] = [v1 time, v2 time, llvm time]
test_case={}

# Get list of all tests to loop over
group = ""
with os.popen("./build/src/tests/testmain2 --gtest_list_tests") as pipe:
    for line in pipe:
        line = (line).strip()
        if(line.endswith('.')):
            group = line
        else:
            test_case[group+line] = [-1,-1,-1]

# Execute each test case using v1, v2, v2-LLVM binaries
v1 = os.environ.get('RP_SeExpr')
if not v1:
    sys.exit()

for t in test_case:
    # test time in v1
    with os.popen(v1+"/share/test/SeExpr/testmain --gtest_filter="+t) as pipe:
        for line in pipe:
            line = (line).strip()
            if re.search('OK', line):
                (prefix, time) = re.split('\(', line)
                test_case[t][0]=time.strip('\)')

    # test time in v2 with interpreter
    with os.popen("./build/src/tests/testmain2 --gtest_filter="+t) as pipe:
        for line in pipe:
            line = (line).strip()
            if re.search('OK', line):
                (prefix, time) = re.split('\(', line)
                test_case[t][1]=time.strip('\)')

    # test time in v2 with llvm
    with os.popen("./build/src/tests/testmain2LLVM --gtest_filter="+t) as pipe:
        for line in pipe:
            line = (line).strip()
            if re.search('OK', line):
                (prefix, time) = re.split('\(', line)
                test_case[t][2]=time.strip('\)')

    print t+","+str(test_case[t][0])+","+str(test_case[t][1])+','+str(test_case[t][2])
