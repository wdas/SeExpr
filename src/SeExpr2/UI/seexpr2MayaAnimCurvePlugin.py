import maya.OpenMaya as OpenMaya
import maya.OpenMayaAnim as OpenMayaAnim

from SeExpr2.seexpr2MayaAnimCurve import getCurveForSeExpr

def animCallback(x):
    #return ',"constant","constant",0,"%s"'%x
    return getCurveForSeExpr(x)


def initializePlugin(mobject):
    from SeExpr2 import expreditor2
    expreditor2.ExprControlCollection.setAnimCurveCallback(animCallback)


def uninitializePlugin(mobject):
    from SeExpr2 import expreditor2
    expreditor2.ExprControlCollection.setAnimCurveCallback(None)


def test():
    global ed
    from SeExpr2.expreditor2 import ExprDialog
    ed=ExprDialog()
    ed.setExpressionString('foo=animCurve(u,"constant","constant",0,"pSphere1_translateX");\nfoo')
    ed.show()
