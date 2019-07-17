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

"""
Helpful utilities that make using the SeExprPy syntax traversal class easier
"""

import sys

def traverseCallback(x,callback):
    """Traverse every node in the AST and run a callback(node,childrenNodes)

    For example: 
        ast=SeExprPy.AST(expr)
        allNodes=[]
        traverseCallback(ast.root(),lambda node,children: allNodes.append(node))
    """
    children=x.children()
    callback(x,children)
    for childNode in children: traverseCallback(childNode,callback)

def getComment(node,sorig):
    """Get the comment after the last character position of the given ast node"""
    charRange=node.range
    lineEnd=sorig.find("\n",node.range[1])
    line=""
    if lineEnd==-1: line=sorig[node.range[1]:]
    else: line=sorig[node.range[1]:lineEnd]
    pound=line.find("#",charRange[1])
    if pound == -1: return ""
    else: return line[pound+1:]


def traverseFilter(node,filterCallback):
    """Traverse every node and return a list of the nodes that matched the given expression

    For example:
        expr='a+3+map("test",f())'
        ast=SeExprPy.AST(expr)
        allCalls=SeExprPy.traverseFilter(ast.root(),lambda node,children: node.type==SeExprPy.ASTType.Call)
        allMapNodes=SeExprPy.traverseFilter(ast.root(),lambda node,children: node.type==SeExprPy.ASTType.Call and node.value=="map")
        allVarRefs=SeExprPy.traverseFilter(ast.root(),lambda node,children: node.type==SeExprPy.ASTType.Var)
    """
    ret=[]
    children=node.children()
    if filterCallback(node,children): ret.append(node)
    for childNode in children: ret.extend(traverseFilter(childNode,filterCallback))
    return ret

class Edits:
    """Allows editing of an expression.

    For example:
        originalExpression="3+4"
        ast=SeExprPy.AST(originalExpression)
        edits=SeExprPy.Edits(originalExpression)
        literals=[]
        def doubleAllNumberLiterals(node,children):
            if node.type==SeExprPy.ASTType.Num: edits.addEdit(node,node.value*2)
        SeExprPy.traverseCallback(ast.root(),doubleAllNumberLiterals)
        editedString=edits.makeNewString()
        print(editedString)  # will be 6.0+8.0
    """
    def __init__(self,str):
        "The string to be edited is stored"
        self.changes=[]
        self.str=str
    def addEdit(self,node,xNew):
        "Add an edit"
        self.changes.append((node.range,xNew))
    def formatNewValue(self,newVal):
        "Format the value to be SeExpr literal format"
        if type(newVal)==str: return "\"%s\""%newVal
        return str(newVal)
    def makeNewString(self):
        "Apply the edits to the string and give a new expression"
        # reverse sort
        self.changes.sort(lambda x,y: -cmp(x[0][0],y[0][0]))
        s=self.str
        for r,newVal in self.changes:
            s=s[:r[0]]+self.formatNewValue(newVal)+s[r[1]:]
        return s


def printTreeHelper(x,indent,mask,sorig):
    "Helper for printTree() (internal function)"
    children=x.children()
    if indent != 1:
        for i in range(indent-1):
            if (mask&(1<<i))!=0: sys.stdout.write("    ")
            else:sys.stdout.write("   \xe2\x94\x82")
        sys.stdout.write("   ")
        if (mask&(1<<(indent-1))) != 0:  sys.stdout.write("\xe2\x94\x94")
        else:  sys.stdout.write("\xe2\x94\x9c")
        sys.stdout.write("\xe2\x94\x80 ")
    else:
        sys.stdout.write("   ")
    sys.stdout.write("%s %s\n"%(x.type,x.value))
    
    for i in range(len(children)):
        child=children[i]
        isEnd=(i==len(children)-1)
        newMask=mask | (isEnd << indent)
        printTreeHelper(child,indent+1,newMask,sorig)

def printTree(x,sorig):
    "Print the parse tree of an ASTHandle"
    printTreeHelper(x.root(),1,1,sorig)


if __name__=="__main__":
    s="""
    # a cool expression
    a=3;
    if(a>5){
        b=10;
    }else if(a>10){
        b=20;
    }
    c+=10+P;
        map("lame.png")
        +map("foo.png",
            map("blah.png"))+f(g(3,"test"),h("lame"))
    """
    expr=AST(s)
    edits=Edits(s)
    def editAllMapsToHavePrefix(node,childs):
        if node.type==ASTType.Call: # check node is a call
            if node.value=="map": # check if the call is map
                if childs[0].type==ASTType.String: # check if the first argument is a string
                    edits.addEdit(childs[0],"testo/%s"%childs[0].value) # add to the edit structure
    traverseCallback(expr.root(),editAllMapsToHavePrefix)
    printTree(expr,s)
    print(edits.makeNewString())
    
    print("--All Function calls----------------------------")
    def printAllFunctionsAndTheirArguments(node,childs):
        if node.type==ASTType.Call:
            print("%s"%node.value)
            for child in childs:
                if child.type==ASTType.Num:
                    edits.addEdit(child,child.value*4)
                print("  %30r %r"%(child.type,child.value))
    traverseCallback(expr.root(),printAllFunctionsAndTheirArguments)

    print("--All Variables----------------------------")
    def printAllVariableReferences(node,childs):
        if node.type==ASTType.Var:
            print("%s"%node.value)
        elif node.type==ASTType.Assign:
            print("assign of %s"%node.value)
    traverseCallback(expr.root(),printAllVariableReferences)
