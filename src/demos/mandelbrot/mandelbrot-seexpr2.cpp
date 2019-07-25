#include <mutex>

#include <SeExpr2/Expression.h>

#include "Vec-impl.h"
using SumFunc = void (*)(Vec*, Vec*, Vec*, const Vec*, const Vec&);

#include "mandelbrot-impl.h"

struct DoubleVar : public SeExpr2::ExprVarRef {
    DoubleVar() : SeExpr2::ExprVarRef(SeExpr2::ExprType().FP(1).Varying()), val(0.0)
    {
    }

    void eval(double* result)
    {
        result[0] = val;
    }

    void eval(const char**)
    {
    }

    double val;
};

struct MyVars {
    mutable DoubleVar r;
    mutable DoubleVar i;
    mutable DoubleVar r0;
    mutable DoubleVar i0;

    SeExpr2::ExprVarRef* resolveVar(const std::string& name) const
    {
        if (name == "r")
            return &r;
        if (name == "i")
            return &i;
        if (name == "r0")
            return &r0;
        if (name == "i0")
            return &i0;
        return 0;
    }
};

struct MyExpression : public SeExpr2::Expression {
    MyExpression(const std::string& expr, MyVars& vars_) : SeExpr2::Expression(), vars(vars_)
    {
        setExpr(expr);
        setDesiredReturnType(SeExpr2::ExprType().FP(1).Uniform());
    }

  private:
    SeExpr2::ExprVarRef* resolveVar(const std::string& name) const override
    {
        return vars.resolveVar(name);
    }

    MyVars& vars;
};

inline void calcSum(Vec* r, Vec* i, Vec* sum, const Vec* init_r, const Vec& init_i)
{
    thread_local MyVars vars;
    thread_local MyExpression rExpr("r*r - i*i + r0", vars);
    thread_local MyExpression iExpr("2*r*i + i0", vars);
    thread_local MyExpression sumExpr("r*r + i*i", vars);

    for (int vec = 0; vec < 8 / VEC_SIZE; vec++) {
        for (size_t c = 0; c < VEC_SIZE; ++c) {
            vars.r.val = r[vec][c];
            vars.i.val = i[vec][c];
            vars.r0.val = init_r[vec][c];
            vars.i0.val = init_i[c];

            sumExpr.evalFP(&sum[vec][c]);
            rExpr.evalFP(&r[vec][c]);
            iExpr.evalFP(&i[vec][c]);
        }
    }
}

int main(int argc, char** argv)
{
    int res = 16000;
    if (argc >= 2) {
        res = atoi(argv[1]);
    }

    mandelbrot(res, calcSum);
}
