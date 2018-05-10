#pragma once

#include "ExprType.h"

namespace SeExpr2 {    
struct TypeConversion {
    enum Category {
        Undefined,
        NoConversion,
        VectorToScalar,
        ScalarToVector
    };

    TypeConversion() : category(Undefined) {}
    TypeConversion(Category category_) : category(category_) {}
    TypeConversion(const ExprType& natural_, const ExprType& final_) : category(Undefined), natural(natural_), final(final_) {
        if (natural.type() == final.type()) {
            if (natural.dim() == final.dim()) {
                category = NoConversion;
            } else if (natural.dim() > final.dim()) {
                category = VectorToScalar;
            } else {
                category = ScalarToVector;
            }
        }

        std::cout << natural.toString() << " ==> " << final.toString() << " :: " << category << std::endl;
    }

    operator Category() const { return category; }

    Category category;
    ExprType natural;
    ExprType final;
};
}
