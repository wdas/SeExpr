#include <gtest.h>
#include <gmock.h>
#include "ExprMultiExpr.h"

using ::testing::ElementsAre;
using namespace SeExpr2;

TEST(MultiTests, ParabolaCubicE1)
{
   Expressions ee;

   VariableHandle xHandle = ee.addExternalVariable("x", ExprType().FP(1).Varying());
   VariableHandle vHandle = ee.addExternalVariable("v", ExprType().FP(3).Varying());

   ExprHandle parabola = ee.addExpression("parabola", ExprType().FP(3).Varying(), "x*x");
   ExprHandle cubic = ee.addExpression("cubic", ExprType().FP(3).Varying(), "parabola*v");
   ExprHandle e1 = ee.addExpression("e1", ExprType().FP(3).Varying(), "cubic*parabola+x+1");

   ASSERT_TRUE(ee.isValid());

   //VariableSetHandle xSHandle = ee.getLoopVarSetHandle(xHandle);
   VariableSetHandle vSHandle = ee.getLoopVarSetHandle(vHandle);
   ExprEvalHandle Scubic = ee.getExprEvalHandle(cubic);
   EXPECT_EQ(Scubic.second.size(), (unsigned int)1);

   //ExprEvalHandle Se1 = ee.getExprEvalHandle(e1);
   //std::cout << "size of Se1 is " << Se1.second.size() << std::endl;

   ee.setVariable(xHandle, 8);

   double xmax = 100;
   double xmin = 1;
   double npoints = 1000;
   //double dx = (xmax-xmin) / (npoints-1);
   for(int i=0;i<npoints;i++){
       //ee.setVariable(xSHandle, i*dx+xmin);
       double myints[] =  {2.0, 3.0, 4.0};
       ee.setLoopVariable(vSHandle, myints, 3);
       std::vector<double> xCubed = ee.evalFP(Scubic);
       EXPECT_THAT(xCubed, ElementsAre(128,192,256));
       // EXPECT_EQ(xCubed[0], 128);
       // EXPECT_EQ(xCubed[1], 192);
       // EXPECT_EQ(xCubed[2], 256);

//       std::vector<double> xe1 = ee.evalFP(Se1);
//       std::cout << xe1[0] << ","
//                 << xe1[1] << ","
//                 << xe1[2] << std::endl;
   }

}
