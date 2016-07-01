#!/bin/env python
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
import SeExprPy
path="/disney/shows/SHOW/sa/shared/paint3d/expressions/"

success=0
failed=0
total=0

def parseFile(filename):
    global success,failed,total
    try:
        SeExprPy.AST(open(filename).read())
        success+=1
    except RuntimeError as e:
        print "Parse error in %s"%filename
        print "%s"%e
        failed+=1
    total+=1

if len(sys.argv)==2:
    parseFile(sys.argv[1])
else:
    for root,dirs,files in os.walk(path):
        for f in files:
            if f.endswith(".se"):
                #print f
                filename=os.path.join(root,f)
                parseFile(filename)


print "Parsed %d Success %d Failure %d"%(total,success,failed)