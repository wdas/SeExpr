#!/usr/bin/env python
# generate timing report for image tests against 3 versions of seexpr
# imageTestsReport.py 2> /tmp/imageTests.csv

import sys
import os
import re
import subprocess
# test_case[test_name] = [v1 time, v2 time, llvm time]
test_case={}

# Get list of all tests to loop over

fpData=open("data.csv","w")
fpLog=open("data.log","w")


v2=os.path.join(os.environ["RP_seexpr2"],"share/test/SeExpr2/")
v1 = os.path.join(os.environ["RP_SeExpr"],"share/test/SeExpr/")
print v1
filterExpr=""
if len(sys.argv)==2:
    filterExpr=sys.argv[1]


group = ""
with os.popen(v2+"/testmain2 --gtest_list_tests") as pipe:
    for line in pipe:
        line = (line).strip()
        if(line.endswith('.')):
            group = line
        else:
            if filterExpr == "" or (group+line).find(filterExpr)!=-1:
                test_case[group+line] = [-1,-1,-1]

# Execute each test case using v1, v2, v2-LLVM binaries
if not v1:
    sys.exit()

def runAndParseTest(cmd):
    fpLog.write("==================================================================\n")
    fpLog.write(cmd+"\n")
    fpLog.write("==================================================================\n")
    #print cmd
    process=subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
    pipe=iter(process.stdout.readline,b"")
    for line in pipe:
        fpLog.write(line)
        line = (line).strip()
        if re.search('OK', line):
            (prefix, time) = re.split('\(', line)
            return (time.strip('\)').strip('ms'))

    return -1

idx=0
N=len(test_case)
for test in test_case:
    # test time in v
    test_case[test][0]=runAndParseTest(v1+"/testmain --gtest_filter="+test)
    test_case[test][1]=runAndParseTest(v2+"/testmain2 --gtest_filter="+test)
    test_case[test][2]=runAndParseTest(v2+"/testmain2LLVM --gtest_filter="+test)

    fpData.write(test+","+str(test_case[test][0])+","+str(test_case[test][1])+','+str(test_case[test][2])+"\n")
    idx+=1
    data=test_case[test]
    print "%5d/%5d (%10s,%10s,%10s) %s "%(idx,N,data[0],data[1],data[2],test)

