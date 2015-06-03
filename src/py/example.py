import SeExprPy
import sys

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
	expr=SeExprPy.AST(s)
	edits=SeExprPy.Edits(s)
	def editAllMapsToHavePrefix(node,childs):
		if node.type==SeExprPy.ASTType.Call: # check node is a call
			if node.value=="map": # check if the call is map
				if childs[0].type==SeExprPy.ASTType.String: # check if the first argument is a string
					edits.addEdit(childs[0],"testo/%s"%childs[0].value) # add to the edit structure
	SeExprPy.traverseCallback(expr.root(),editAllMapsToHavePrefix)
	SeExprPy.printTree(expr,s)
	print edits.makeNewString()
	
	print "--All Function calls----------------------------"
	def printAllFunctionsAndTheirArguments(node,childs):
		if node.type==SeExprPy.ASTType.Call:
			print "%s"%node.value
			idx=0
			for child in childs:
				if child.type==SeExprPy.ASTType.Num:
					edits.addEdit(child,child.value*4)
				print "  Argument %2d %-15s %r"%(idx,child.type,child.value)
				idx+=1
	SeExprPy.traverseCallback(expr.root(),printAllFunctionsAndTheirArguments)

	print "--All Variables----------------------------"
	def printAllVariableReferences(node,childs):
		if node.type==SeExprPy.ASTType.Var:
			print "reference %s"%node.value
		elif node.type==SeExprPy.ASTType.Assign:
			print "assign    %s"%node.value
	SeExprPy.traverseCallback(expr.root(),printAllVariableReferences)