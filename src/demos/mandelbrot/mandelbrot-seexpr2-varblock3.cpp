#include <SeExpr2/Expression.h>
#include <SeExpr2/VarBlock.h>

#include "Vec-impl.h"
using SumFunc = void (*)(Vec*, Vec*, Vec*, const Vec*, const Vec&);

#include "mandelbrot-impl.h"

struct StaticExpression : public SeExpr2::Expression {
    StaticExpression(const SeExpr2::VarBlockCreator& creator, const std::string& expr) : SeExpr2::Expression()
    {
        setExpr(expr);
        setVarBlockCreator(&creator);
        setDesiredReturnType(SeExpr2::ExprType().FP(1).Varying());
    }
};

struct StaticVarBlockCreator : public SeExpr2::VarBlockCreator {
    StaticVarBlockCreator()
    {
        r = registerVariable("r", SeExpr2::ExprType().FP(1).Varying());
        i = registerVariable("i", SeExpr2::ExprType().FP(1).Varying());
        r0 = registerVariable("r0", SeExpr2::ExprType().FP(1).Varying());
        i0 = registerVariable("i0", SeExpr2::ExprType().FP(1).Varying());
    }

    int r;
    int i;
    int r0;
    int i0;
};

inline void calcSum(Vec* r, Vec* i, Vec* sum, const Vec* init_r, const Vec& init_i)
{
    static StaticVarBlockCreator creator;
    static StaticExpression rExpr(creator, "r*r - i*i + r0");
    static StaticExpression iExpr(creator, "2*r*i + i0");
    static StaticExpression sumExpr(creator, "r*r + i*i");
    thread_local double stack[4];
    thread_local SeExpr2::SymbolTable symtab(creator.create(), stack, 4);

    for (int vec = 0; vec < 8 / VEC_SIZE; vec++) {
        Vec r_ = r[vec];
        Vec i_ = i[vec];

        for (int c = 0; c < VEC_SIZE; ++c) {
            symtab.Scalar(creator.r) = r_[c];
            symtab.Scalar(creator.i) = i_[c];
            symtab.Scalar(creator.i0) = init_i[c];
            symtab.Scalar(creator.r0) = init_r[vec][c];

            sumExpr.evalFP(&sum[vec][c], &symtab);
            rExpr.evalFP(&r[vec][c], &symtab);
            iExpr.evalFP(&i[vec][c], &symtab);
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
