#pragma once
#include <string>
using namespace std;

class CExprBuilder {
public:
    virtual void opAdd() = 0;
    virtual void opSub() = 0;
    virtual void opMul() = 0;
    virtual void opDiv() = 0;
    virtual void opPow() = 0;
    virtual void opNeg() = 0;

    virtual void opEq() = 0;
    virtual void opNe() = 0;
    virtual void opLt() = 0;
    virtual void opLe() = 0;
    virtual void opGt() = 0;
    virtual void opGe() = 0;

    virtual void valNumber(double val) = 0;
    virtual void valString(string val) = 0;
    virtual void valReference(string val) = 0;
};

void parseExpression(string expr, CExprBuilder &builder);