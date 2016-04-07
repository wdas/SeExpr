#!/usr/bin/python

import sys
import os

filenameIn=sys.argv[1]

tbl="""<html>
<head>
<style>
body {font-family: Helvetica, sans-serif;}
td {width: 100pt;}
td.medium {background: orange;}
td.bad {background: red;}
td.good {background: green;}
td.broke {background: black;}
tr:nth-child(even) {background: #CCC}
tr:nth-child(odd) {background: #FFF}
tr.head {background:#333333;color:#eeeeee;}
td.neutral {} 
</style>
</head>
<body>
"""

def getFloat(val):
	return float(val.split(" ")[0])
def percentToStyle(percent):
	if percent<-5:
		return "bad"
	elif percent <5:
		return "medium"
	else:
		return "good"

lines=open(filenameIn).readlines()
tbl+="<table>\n"
lines.insert(0,"Test,V1,V2 interpreter,V2 llvm,V2 interpreter percent,V2 llvm percent")

tbl+="<thead>"
items=lines[0].split(",")
tbl+="<tr class='head'>\n"+"".join(["    <td>"+x+"</td>\n" for x in items])+"</tr>"
tbl+="</thead>"
tbl+="<tbody>"

for line in lines[1:]:
	items=line.split(",")
	v1=getFloat(items[1])
	v2interpreter=getFloat(items[2])
	v2llvm=getFloat(items[3])

	styles=["label","","broke","broke","broke","broke"]
	items[1]="%.1f ms"%v1
	if v2interpreter != -1:
		styles[2]=""
		items[2]="%.1f ms"%v2interpreter
		percent=(v1-v2interpreter)/v1*100
		items[3]="%.0f%%"%percent
		styles[3]=percentToStyle(percent)
	items.append("")
	items.append("")
	if v2llvm != -1:
		styles[4]=""
		items[4]="%.1f ms"%v2llvm
		percent=(v1-v2llvm)/v1*100
		items[5]="%.0f%%"%percent
		styles[5]=percentToStyle(percent)
	s="<tr>"+"".join(["<td class='%s'>%s</td>\n"%xs for xs in zip(styles,items)])+"</tr>"
	tbl+=s
tbl+="</tbody>"
tbl+="</table>\n"
tbl+="</body></html>"
open("test.html","w").write(tbl)