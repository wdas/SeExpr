import maya.OpenMaya as OpenMaya
import maya.OpenMayaAnim as OpenMayaAnim

infinityTypeToString={OpenMayaAnim.MFnAnimCurve.kConstant: "constant",
    OpenMayaAnim.MFnAnimCurve.kLinear:"linear",
    OpenMayaAnim.MFnAnimCurve.kCycle:"cycle",
    OpenMayaAnim.MFnAnimCurve.kCycleRelative:"cycleRelative",
    OpenMayaAnim.MFnAnimCurve.kOscillate:"oscillate"}
tangentTypeToString={
    OpenMayaAnim.MFnAnimCurve.kTangentGlobal: 'global',
    OpenMayaAnim.MFnAnimCurve.kTangentFixed: 'fixed',
    OpenMayaAnim.MFnAnimCurve.kTangentLinear: 'linear',
    OpenMayaAnim.MFnAnimCurve.kTangentFlat: 'flat',
    OpenMayaAnim.MFnAnimCurve.kTangentSmooth: 'smooth',
    OpenMayaAnim.MFnAnimCurve.kTangentStep: 'step',
    OpenMayaAnim.MFnAnimCurve.kTangentSlow: 'slow',
    OpenMayaAnim.MFnAnimCurve.kTangentFast: 'fast',
    OpenMayaAnim.MFnAnimCurve.kTangentClamped: 'clamped',
    OpenMayaAnim.MFnAnimCurve.kTangentPlateau: 'plateau',
    OpenMayaAnim.MFnAnimCurve.kTangentStepNext: 'stepNext',
    OpenMayaAnim.MFnAnimCurve.kTangentAuto: 'auto'
}

def getCurveForSeExpr(curveObj):
    #s=","

    selectionList = OpenMaya.MSelectionList()
    selectionList.add( curveObj )
    dependNode = OpenMaya.MObject()
    selectionList.getDependNode( 0, dependNode )
    curve=OpenMayaAnim.MFnAnimCurve(dependNode)
    weightedAsInt={False: "0", True: "1"}[curve.isWeighted()]
    #s+='"'+infinityTypeToString[curve.preInfinityType()]+'","'+infinityTypeToString[curve.postInfinityType()]+'",'+weightedAsInt+',"'+curveObj+'"'
    items=[]
    items.append(infinityTypeToString[curve.preInfinityType()])
    items.append(infinityTypeToString[curve.postInfinityType()])
    items.append(weightedAsInt)
    items.append(curveObj)
    for i in range(curve.numKeys()):
        def getAngleAndWeight(isIn):
            tangentAngle=OpenMaya.MAngle()
            scriptUtil=OpenMaya.MScriptUtil()
            curveWeightPtr=scriptUtil.asDoublePtr()
            curve.getTangent(i, tangentAngle, curveWeightPtr, isIn )
            return tangentAngle.asDegrees(),scriptUtil.getDouble(curveWeightPtr)
        t,val=curve.time(i).value(),curve.value(i)
        inAngle,inWeight=getAngleAndWeight(True)
        outAngle,outWeight=getAngleAndWeight(False)
        items.extend([t,val,inWeight,outWeight,inAngle,outAngle,tangentTypeToString[curve.inTangentType(i)],tangentTypeToString[curve.outTangentType(i)],1])
        #s+=',\n%f,%f,%f,%f,%f,%f,"%s","%s",1'%(t,val,inWeight,outWeight,inAngle,outAngle

    print items
    return tuple(items)

