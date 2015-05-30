import SeExprPy
import sys

def traverseCallback(x,callback):
	"Traverse every node in the AST and run a callback"
	children=x.children()
	callback(x,children)
	for c in children: traverseCallback(c,callback)

class Edits:
	"Help edit expression"
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

def printTreeHelper(x,indent,mask):
	"print an expression tree"
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
		printTreeHelper(child,indent+1,newMask)
def printTree(x):
	printTreeHelper(x.root(),1,1)

if __name__=="__main__":
	s="""
	# a cool expression
	a=3;
	if(a>5){
		b=10;
	}else if(a>10){
		b=20;
	}
	c+=10;
		map("lame.png")
		+map("foo.png",
			map("blah.png"))+f(g(3,"test"),h("lame"))

	"""
	expr=SeExprPy.AST(s)
	edits=Edits(s)
	def editAllMapsToHavePrefix(x,childs):
		if x.value!="map": return
		if x.type==SeExprPy.ASTType.Call:
			if childs[0].type==SeExprPy.ASTType.String:
				edits.addEdit(childs[0],"testo/%s"%x.value)
	traverseCallback(expr.root(),editAllMapsToHavePrefix)
	printTree(expr)
	print edits.makeNewString()

