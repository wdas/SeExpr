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
import subprocess
import re

# Assumes script is run from inside a code repo, at project root dir
imageDir = "./build/images/"

# Baseline images for paint3d and imageSynth examples
baselineRoot = "/disney/depts/tech/techweb_files/SeExpr/"
baselineUrl = "http://techweb/files/SeExpr/"
exampleDir = "images/"

# ln -s p3dDir /disney/depts/tech/techweb_files/SeExpr/expressions
p3dDir = "/disney/shows/default/rel/global/expressions/"
p3dUrl = "http://techweb/files/SeExpr/expressions/"

rowData = {} # expr filename, baseline image, test image, diff flag
diffImages = {}

# Diff images in current repo against baseline images
for imageFile in os.listdir(imageDir):
    if not imageFile.endswith('png'):
        continue
    exprFile = imageFile.replace('.png', '')
    exprFileUrl = ""
    foundPath = False
    for root, dirs, files in os.walk(p3dDir):
        if foundPath:
            continue
        for name in files:
            if foundPath:
                continue
            if re.compile(exprFile).match(name):
                exprFileUrl = os.path.join(root, name).replace(p3dDir, p3dUrl)
                foundPath = True
    currentImage = imageDir + imageFile
    baselineImage = baselineRoot + exampleDir + imageFile
    diffImage = imageDir + imageFile.replace('png','iff')

    child = subprocess.call(["imgdiff", "-t", "1", baselineImage, currentImage,
                             diffImage, '-v'])

    data = []
    data.append(exprFileUrl)
    data.append(baselineUrl + exampleDir + imageFile)
    data.append(exampleDir + imageFile)
    data.append(False)
    rowData[exprFile] = data


# Write HTML file
htmlFileName = "./build/seexpr-image-diff.html"
htmlFile = open(htmlFileName, 'w')
htmlFile.write("<html>\n<head>\n<title>SeExpr Image Tests</title>\n</head>\n")
htmlFile.write("<body>\n")
htmlFile.write("<h1>SeExpr Image Tests</h1>\n")

# Table header
htmlFile.write("<table>\n")
htmlFile.write("<thead bgcolor=\"silver\">\n")
htmlFile.write("<tr> <th>Filename</th>\n")
htmlFile.write("<th>Baseline</th>\n")
htmlFile.write("<th>Test Image</th>\n")
htmlFile.write("<th>Diff</th> </tr>\n <tbody>\n")

# Table rows w/ diff images
for diffFile in sorted(os.listdir(imageDir)):
    if not diffFile.endswith('iff'):
        continue
    exprFile = diffFile.replace('.iff', '')
    convertedFile = diffFile.replace('se.iff','diff.jpg')
    diffImage = imageDir + diffFile
    convertedImage = imageDir + convertedFile
    f = subprocess.Popen(["itconvert", diffImage, "-o", convertedImage])

    # Link to original expression file when diff image exists
    htmlFile.write("<tr><td><b><a href=\"" + rowData[exprFile][0] + "\">" +
                   exprFile + "</a></b></td>\n")
    htmlFile.write("<td><img src=\"" + rowData[exprFile][1] + "\"/></td>\n")
    htmlFile.write("<td><img src=\"" + rowData[exprFile][2] + "\"/></td>\n")
    htmlFile.write("<td><img src=\"" + exampleDir + convertedFile + "\"/>\n")
    htmlFile.write("</td></tr>\n")
    rowData[exprFile][3] = True

# Table rows w/ no diff images, ie, identical baseline and test images
for exprFile in sorted(rowData.keys()):
    if rowData[exprFile][3] == True:
        continue
    htmlFile.write("<tr><td><b>" + exprFile + "</b></td>\n")
    htmlFile.write("<td><img src=\"" + rowData[exprFile][1] + "\"/></td>\n")
    htmlFile.write("<td><img src=\"" + rowData[exprFile][2] + "\"/></td>\n")
    htmlFile.write("<td></td></tr>\n")

htmlFile.write("</tbody>\n")
htmlFile.write("</table>\n")
htmlFile.write("</body>")
htmlFile.close()

