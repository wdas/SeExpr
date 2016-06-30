#include <SeExpr2/Expression.h>
#include <SeExpr2/VarBlock.h>
#include <SeExpr2/Platform.h>
#include <iostream>

struct Expr : public SeExpr2::Expression {
    Expr(const std::string& s) : Expression(s) {}

    struct SimpleVar : public SeExpr2::ExprVarRef {
        SimpleVar() : ExprVarRef(SeExpr2::ExprType().FP(3).Varying()) { v[0] = v[1] = v[2] = 0; }
        double v[3];
        void eval(const char**) {}
        void eval(double* result) override {
            for (int k = 0; k < 3; k++) result[k] = v[k];
        }
    };

    mutable SimpleVar singleII, singleII2;

    mutable SeExpr2::VarBlockCreator creator;
    SeExpr2::ExprVarRef* resolveVar(const std::string& name) const override {
        if (name == "singleII") return &singleII;
        if (name == "singleII2") return &singleII2;
        return creator.resolveVar(name);
    }
};

// More concise type thing i.e. predefined SeExpr2::Vec3Varying
// Direct varying use enum type instead of bool.
// For loop auto unroll.
// Variable access using varRef!!! for paint3d u,v in map and rand()

void run(int way) {
    std::string exprStr;
    SeExpr2::PrintTiming timer("way " + std::to_string(way));
    switch (way) {
        case 0:
            exprStr = "singleII+singleII2";
            break;
        case 2:
            exprStr = "singleI+singleI2";
            break;
        default:
            throw std::runtime_error("Invalid way");
    }
    Expr e(exprStr);
    e.singleII.v[0] = 1.;
    e.singleII.v[1] = 10.;
    e.singleII.v[2] = 100.;
    e.singleII2.v[0] = 5.5;
    e.singleII2.v[1] = 210.;
    e.singleII2.v[2] = 2100.;
    int singleI = e.creator.registerVariable("singleI", SeExpr2::ExprType().FP(3).Uniform());
    int singleI2 = e.creator.registerVariable("singleI2", SeExpr2::ExprType().FP(3).Uniform());
    int P = e.creator.registerVariable("P", SeExpr2::ExprType().FP(3).Varying());
    int Cd = e.creator.registerVariable("Cd", SeExpr2::ExprType().FP(3).Varying());
    int faceId = e.creator.registerVariable("faceId", SeExpr2::ExprType().FP(1).Varying());

    if (!e.isValid()) {
        std::cerr << "Parse Error:\n" << e.parseError() << std::endl;
    }
    auto evaluator = e.creator.create();
    std::vector<double> PArray(10 * 3);
    for (size_t i = 0; i < PArray.size(); i++) PArray[i] = i;
    std::vector<double> CdArray(10 * 3);
    for (size_t i = 0; i < CdArray.size(); i++) CdArray[i] = 2 * i;
    std::vector<double> faceIdArray(10);
    for (size_t i = 0; i < faceIdArray.size(); i++) faceIdArray[i] = 3 * i;
    evaluator.Pointer(P) = PArray.data();
    evaluator.Pointer(Cd) = CdArray.data();
    evaluator.Pointer(faceId) = faceIdArray.data();
    double singleV[] = {1, 10, 100};
    double singleV2[] = {5.5, 210, 2100};
    // tbb::parallel_for(...){
    //    auto evaluator=evaluator.clone();
    evaluator.Pointer(singleI) = &singleV[0];
    evaluator.Pointer(singleI2) = &singleV2[0];
    double sum = 0.;
    for (int iii = 0; iii < 20; iii++) {
        for (int ii = 0; ii < 1000000; ii++) {
            for (size_t i = 0; i < faceIdArray.size(); i++) {
                // evaluator.FP<3>(singleD)[0]=sum;
                // singleV2[0]=sum;
                evaluator.indirectIndex = i;
                const double* data = e.evalFP(&evaluator);
                sum += data[0];
                if (data[0] != 6.5 || data[1] != 220 || data[2] != 2200) {
                    std::cerr << "Mismatch of expected value 6.5,220,2200 got " << data[0] << " " << data[1] << " "
                              << data[2] << std::endl;
                    throw std::runtime_error("Mismatch");
                }
                // std::cout<<"we have "<<data[0]<<" "<<data[1]<<" "<<data[2]<<std::endl;
            }
        }
    }
}

int main() {
    run(0);
    run(2);
    run(0);
    run(2);
    run(0);
    run(2);
    return 0;
}
