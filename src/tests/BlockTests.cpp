
#include <SeExpr2/Expression.h>
#include <SeExpr2/VarBlock.h>

using namespace SeExpr2;

void printVec(const std::string& name, const std::vector<double>& v) {
    std::cerr << name << " data is at " << v.data() << " -- ";
    for (auto it : v) {
        std::cerr << " " << it;
    }
    std::cerr << std::endl;
}

bool compareVecs(const char* label, const std::vector<double>& a, const std::vector<double>& b) {
    bool isGood = true;
    for (size_t i = 0; i < a.size(); i++)
        if (a[i] != b[i]) {
            std::cerr << label << " index " << i << " no match a=" << a[i] << " b=" << b[i] << std::endl;
            isGood = false;
        }
    return isGood;
}
int main() {
    VarBlockCreator creator;
    int offI = creator.registerVariable("i", ExprType().FP(3).Varying());
    int offA = creator.registerVariable("A", ExprType().FP(3).Varying());
    int offB = creator.registerVariable("B", ExprType().FP(1).Varying());
    int offC = creator.registerVariable("C", ExprType().FP(3).Varying());
    int offI2 = creator.registerVariable("i2", ExprType().FP(3).Varying());
    auto eval = creator.create();
    std::vector<double> I(16 * 3);
    std::vector<double> I2(16 * 3);
    // for(int i=0;i<16*3;i+=3) I[i]=I[i+1]=I[i+2]=i+5;
    for (int i = 0; i < 16 * 3; i++) I[i] = i;
    for (int i = 0; i < 16 * 3; i++) I2[i] = i / 3;
    std::vector<double> A(16 * 3);
    std::vector<double> B(16);
    std::vector<double> C(16 * 3);
    eval.Pointer(offI) = I.data();
    eval.Pointer(offA) = A.data();
    eval.Pointer(offB) = B.data();
    eval.Pointer(offC) = C.data();
    eval.Pointer(offI2) = I2.data();

    auto buildAndRun = [&](
        Expression::EvaluationStrategy strategy, const std::string& expr, ExprType type, int output) {
        Expression e(strategy);
        e.setExpr(expr);
        e.setVarBlockCreator(&creator);
        e.setDesiredReturnType(type);
        if (!e.isValid()) {
            throw std::runtime_error(std::string("Expr '") + expr + "'" + " invalid because\n" + e.parseError() + "\n");
            return;
        } else {
            #if 1 // run multiple
            e.evalMultiple(&eval, output, 0, 16);
            #else // vs the old way
            for (int index = 0; index < 16; index++) {
                eval.indirectIndex = index;
                const double* data = e.evalFP(&eval);
                int dim = type.dim();
                for (int k = 0; k < dim; k++) eval.Pointer(output)[dim * index + k] = data[k];
            }
            #endif
        }
    };
    // buildAndRun("a=printf(\"Test %v\",i);[i,i*2,i*4]",TypeVec(3),offA);
    buildAndRun(Expression::UseLLVM, "i", TypeVec(3), offA);         // run A expr
    buildAndRun(Expression::UseLLVM, "i", TypeVec(1), offB);         // run B expr
    buildAndRun(Expression::UseLLVM, "A+B+i2*i", TypeVec(3), offC);  // run C expr
    std::vector<double> Abak = A;
    std::vector<double> Bbak = B;
    std::vector<double> Cbak = C;
    // reevaluate with interpreter
    A.resize(A.size(), 0);
    B.resize(B.size(), 0);
    C.resize(C.size(), 0);
    buildAndRun(Expression::UseInterpreter, "i", TypeVec(3), offA);         // run A expr
    buildAndRun(Expression::UseInterpreter, "i", TypeVec(1), offB);         // run B expr
    buildAndRun(Expression::UseInterpreter, "A+B+i2*i", TypeVec(3), offC);  // run C expr
    // compare two results
    bool good = true;
    good &= compareVecs("A", Abak, A);
    good &= compareVecs("B", Bbak, B);
    good &= compareVecs("C", Cbak, C);
    if (getenv("SE_EXPR_DEBUG")) {
        printVec("I", I);
        printVec("A", A);
        printVec("B", B);
        printVec("C", C);
    }
    return good ? 0 : 1;
}
