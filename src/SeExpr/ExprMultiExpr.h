#include "Expression.h"

namespace SeExpr2 {

class DExpression;
class GlobalVal;
class Expressions;

typedef std::set<GlobalVal*>::iterator VariableHandle;
typedef std::set<GlobalVal*>::iterator VariableSetHandle;
typedef std::set<DExpression*>::iterator ExprHandle;
typedef std::pair<ExprHandle, std::vector<DExpression*> > ExprEvalHandle;

class DExpression: public Expression {
    Expressions& context;

public:
    DExpression(const std::string& varName,
                Expressions &context,
                const std::string &e,
                const ExprType &type = ExprType().FP(3),
                EvaluationStrategy be = defaultEvaluationStrategy);

    mutable std::set<DExpression*> operandExprs;
    mutable std::set<GlobalVal*> operandVars;

    GlobalVal *val;
    const std::string& name() const;
    ExprVarRef* resolveVar(const std::string& name) const;
    void eval();
};

class Expressions {
    std::set<DExpression*> exprToEval;
    std::set<DExpression*> exprEvaled;
public:
    std::set<DExpression*> AllExprs;
    std::set<GlobalVal*> AllExternalVars;

   //Expressions(int numberOfEvals=1);
   Expressions() {}
   ~Expressions();

   VariableHandle addExternalVariable(const std::string& variableName, ExprType seTy);
   ExprHandle addExpression(const std::string& varName, ExprType seTy, const std::string& expr);

   VariableSetHandle getLoopVarSetHandle(VariableHandle vh);
   void setLoopVariable(VariableSetHandle handle, double *values, unsigned dim);
   void setLoopVariable(VariableSetHandle handle, double value) {
       setLoopVariable(handle, &value, 1);
   }
   void setLoopVariable(VariableSetHandle handle, const char *values);

   void setVariable(VariableHandle handle, double *values, unsigned dim);
   void setVariable(VariableHandle handle, double value) {
       setVariable(handle, &value, 1);
   }
   void setVariable(VariableHandle handle, const char *values);

   bool isValid() const;
   void getErrors(std::vector<std::string>& errors) const;
   //bool isVariableUsed(VariableHandle variableHandle) const;

   ExprEvalHandle getExprEvalHandle(ExprHandle eh);
   const std::vector<double> &evalFP(ExprEvalHandle eeh);
   const char* evalStr(ExprEvalHandle eeh);

   void resetEval() {
       exprToEval.clear();
       exprEvaled.clear();
   }

   void reset() {
       resetEval();
       AllExprs.clear();
       AllExternalVars.clear();
   }
};

}
