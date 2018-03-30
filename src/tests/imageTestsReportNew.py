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

import os
import sys
import subprocess
import re

seexpr1=os.environ.get("RP_SeExpr")
seexpr2=os.environ.get("RP_seexpr2")
if not seexpr1 or not seexpr2:
    print('%s: test versions not present, skipping tests' % sys.argv[0])
    sys.exit(0)

versionKeys="v1","v2-interp","v2-llvm"
versions={"v2-llvm": "SE_EXPR_EVAL=LLVM "+os.path.join(seexpr2,"share","test","SeExpr2","testmain2"),
        "v2-interp": "SE_EXPR_EVAL=INTERPRETER "+os.path.join(seexpr2,"share","test","SeExpr2","testmain2"),
        "v1": os.path.join(seexpr1,"share","test","SeExpr","testmain")}


def run(filterString):
    "Run every version listed above outputting xml files in tmp/"
    exitCode=0
    if not os.path.exists("tmp"): os.mkdir("tmp")
    fpLog=open("tmp/data.log","w")

    testCount=0

    with os.popen(versions["v2-llvm"]+" --gtest_list_tests --gtest_filter=%s"%filterString) as pipe:
        testCount=len([line for line in pipe if not line.endswith(".")])
    print "tests to run %d"%testCount
    for version in versions.keys():
        print " "
        count=0
        exe=versions[version]
        cmd=[exe,"--gtest_output=xml:tmp/%s"%(version+".xml"),"--gtest_filter=%s"%filterString]
        cmd=" ".join(cmd)
        print cmd
        process=subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,shell=True)
        pipe=iter(process.stdout.readline,b"")
        print " "
        for line in pipe:
            fpLog.write(line)
            line=line.strip()
            if re.search('OK', line):
                (prefix, time) = re.split('\(', line)
                count+=1
                if count%10==0: 
                    percent=100*count/testCount
                    print ("\r%30s %10d/%10d -- %d%% "%(version,count,testCount,percent)),
        errorCode=process.wait()
        if errorCode != 0:
            print "\n%s ERROR CODE %d\n"%(exe,errorCode)
            exitCode=1
    return exitCode

def iterateXML(f):
    import xml.etree.ElementTree
    for version in versions.keys():
        xmlfile="tmp/%s.xml"%version
        e = xml.etree.ElementTree.parse(xmlfile).getroot()
        for test in e.iter("testcase"):
            if test.get("status")=="notrun": continue
            fullname=test.get("classname")+"."+test.get("name")
            f(fullname,test)


def view(x):
    "Cat the expression corresponding to the test"
    def find(fullTest,elementData):
        if fullTest==x:
            path=elementData.get("path")
            if path:
                print path
                print "="*len(path)
                print open(path).read()
                sys.exit(0)
    iterateXML(find)

def process():
    "Process xml files generated from run, generate the csv file to be compatible with html (for now), also generate text table"
    import xml.etree.ElementTree
    table={}
    if not os.path.exists("tmp"): os.mkdir("tmp")
    fpData=open("tmp/data.csv","w")

    # Read everything into a nice dictionary
    attrs=["prepareTime","evalTime","totalTime"]
    attrsShort=["prep","eval","tot"]

    for version in versions.keys():
        xmlfile="tmp/%s.xml"%version
        e = xml.etree.ElementTree.parse(xmlfile).getroot()
        #for suite in e:
        #    for test in suite:
        #        print test
        for test in e.iter("testcase"):
            if test.get("status")=="notrun": continue
            fullname=test.get("classname")+"."+test.get("name")
            if not fullname in table:
                table[fullname]={}
            table[fullname][version]={}
            table[fullname][version]["prepareTime"]=test.get("prepareTime")
            table[fullname][version]["evalTime"]=test.get("evalTime")
            table[fullname][version]["totalTime"]=test.get("totalTime")
    #print table

    # headers
    headers=["Test Name"]
    for v in versionKeys:
        for c in attrsShort:
            headers.append("%s %s"%(v,c))
    fpData.write(",".join(headers)+"\n")
    # Build a table representation and the csv
    line="%-50s"%""
    for v in versionKeys:
        line+="|"
        line+="%18s"%v
    print line
    line="%-50s"%"Test Name"
    for v in versionKeys:
        line+="|"
        for c in attrsShort:
            line+=" %5s"%c
    print line
    print "-"*len(line)
    for entryName,entryData in table.items():
        csvLine=[entryName]
        line="%-50s"%entryName
        for v in versionKeys:
            line+="|"
            for c in attrs:
                data=None
                try:
                    data=entryData[v][c]
                except: pass
                if data:
                    if type(data)==str: data=float(data)
                    line+=" %5d"%data
                    csvLine.append(str(data))
                else:
                    line+=" -----"
                    csvLine.append("")
        fpData.write(",".join(csvLine)+"\n")
        print line




if __name__=="__main__":
    cmd="help"
    try:
        cmd=sys.argv[1]
    except:
        pass

    if cmd=="help":
        print "Usage:\n\t%s run [optional filter]"%sys.argv[0]
        print "\t%s process"%sys.argv[0]
    elif cmd=="view":
        view(sys.argv[2])
    elif cmd=="run":
        filterString="*"
        try: filterString=sys.argv[2]
        except:pass
        sys.exit(run(filterString))
    elif cmd=="process":
        process()
    elif cmd=="runall":
        run("*")
        process()
        os.system("python src/tests/htmlReport.py tmp/data.csv")

    
