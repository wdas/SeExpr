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


import sys
import os

if len(sys.argv)==2:
    filenameIn=sys.argv[1]
else:
    print "    usage: htmlReport <CSV_filename>"
    sys.exit(0)

tbl="""<html>
<head>
<style>
body {font-family: Arial, sans-serif;}
table {border-spacing : 0px;}
td.spanned:nth-child(even) {text-align: center; padding: 5px; font-weight: bold; background: #222;}
td.spanned:nth-child(odd) {text-align: center;  padding: 5px; font-weight: bold; background: #444;}
td {width: 100pt;text-align:right;}
td.medium {background: orange;}
td.bad {background: red;}
td.good {background: green;}
td.broke {background: black;}
tr:nth-child(even) {background: #CCC}
tr:nth-child(odd) {background: #FFF}
tr.head {background:#333333;color:#eeeeee;}
td.neutral {} 
.sorty{
	float:right;
    display:block;
    width:0; height:0;
    border-left: 5px solid black;
    border-right: 5px solid black;
    border-bottom: 5px solid black;
    border-color: white;
    margin:5px;
}
.upArrow{
    float:right;
    display:block;
    width:0; height:0;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-bottom: 5px solid white;
    margin:5px;
}
.downArrow{
    display:block;
    float:right;
    width:0; height:0;
    border-left: 5px solid transparent;
    border-right: 5px solid transparent;
    border-top: 5px solid white;
    margin:5px;
}
</style>
</head>
<body>

<script>
	function sortTable(table,col,reverse){
		var body=table.tBodies[0];
		var tr=Array.prototype.slice.call(body.rows,0);
		if(reverse==0) reverse=-1;
		tr=tr.sort(function(a,b){
			if(col == 0) return -reverse*a.cells[col].textContent.trim().localeCompare(b.cells[col].textContent.trim());
			var anum=parseFloat(a.cells[col].textContent)
			var bnum=parseFloat(b.cells[col].textContent);
			if(!isFinite(anum)) anum=-1e7;
			if(!isFinite(bnum)) bnum=-1e7;
			return -reverse*(anum-bnum);
		});
		for(var i=0;i<tr.length;i++) body.appendChild(tr[i]);
	}
	function makeSortable(table) {
	    var th = table.tHead;
	    if(th){
	    	var thRows=th.rows[1];
	    	if(thRows){
	    		var thRowsCells=thRows.cells;
	    		if(thRowsCells){
		    		for(var i=0;i<thRowsCells.length;i++){
		    			var sorterClosure=function ( ii){
					        var dir = 1;
					        var spans=thRowsCells[i].getElementsByTagName("span")[0];
					        thRowsCells[ii].addEventListener('click', function () {
						        for(var j=0;j<thRowsCells.length;j++){
							        var spans2=thRowsCells[j].getElementsByTagName("span")[0];
						    		spans2.className="sorty";
						        }
					        	spans.className=dir ? "downArrow" : "upArrow";
					            sortTable(table, ii, (dir = 1 - dir))
					        });
					    };
					    sorterClosure(i);
				    }
				}
	    	}
	    }
	}

	var done;
	window.onload=function(){
		done=1;
		var tables=document.body.getElementsByTagName("table");
		for(var i=0;i<tables.length;i++){
			makeSortable(tables[i]);
		}
	}
</script>
"""

def getFloat(val):
	try:
		return float(val.split(" ")[0])
	except: return None
def percentToStyle(percent):
	if percent<-5:
		return "bad"
	elif percent <5:
		return "medium"
	else:
		return "good"

try:
    csvData=open(filenameIn)
except:
    print "File " + filenameIn + " doesn't exist"
    sys.exit(1)

lines=csvData.readlines()
tbl+="<table>\n"
#lines.insert(0,"Test,V1,V2 interpreter,V2 interpreter percent,V2 llvm,V2 llvm percent")

tbl+="<thead>"
items=lines[0].split(",")
spannedItems=[(1,"")]
curatedItems=[items[0]]
for idx in range(1,len(items),3):
	for j in range(3):
		spanname,subname=items[idx+j].split(" ")
		if j==0:
			spannedItems.append((4,spanname))
		curatedItems.append(subname)
	curatedItems.append("% diff")
tbl+="<tr class='head'>\n"+"".join(["    <td class='spanned' colspan='%d'>%s</td>\n"%(span,x) for span,x in spannedItems])+"</tr>"
tbl+="<tr class='head'>\n"+"".join(["    <td><span class='sorty'></span>"+x+"</td>\n" for x in curatedItems])+"</tr>"
tbl+="</thead>"
tbl+="<tbody>"

for line in lines[1:]:
	items=line.split(",")
	v1=getFloat(items[1])

	styles=["label"]
	curatedItems=[items[0]]
	for idx in range(1,len(items),3):
		for subidx in range(0,3):
			val=getFloat(items[idx+subidx])
			style="broke"
			if not val is None:
				style="neutral"
			styles.append(style)
			curatedItems.append(val)
		ref=getFloat(items[1+2])
		curr=getFloat(items[idx+2])
		if not (ref is None or curr is None):
			percent=(ref-curr)/ref*100.
			styles.append(percentToStyle(percent))
			curatedItems.append("%.0f%%"%percent)
		else:
			styles.append("broke")
			curatedItems.append("")

	s="<tr>"+"".join(["<td class='%s'>%s</td>\n"%xs for xs in zip(styles,curatedItems)])+"</tr>"
	tbl+=s
tbl+="</tbody>"
tbl+="</table>\n"
tbl+="</body></html>"
open("test.html","w").write(tbl)
