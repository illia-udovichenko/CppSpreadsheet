#pragma once
#include "expression.h"
#include "CExprNodes.h"
#include <string>
#include <vector>
#include <memory>
using namespace std;

/* Implements the abstract CExprBuilder interface for use with the provided parser.
 * Collects operands and operators during parsing and constructs an AST
 * (vector of unique_ptr<CExpr>) representing the expression in postfix order. */
class ExpressionBuilder : public CExprBuilder {
public:
    ExpressionBuilder() = default;

    // Arithmetic operators
    void opAdd() override { m_Expressions.push_back(make_unique<CAdd>()); }
    void opSub() override { m_Expressions.push_back(make_unique<CSub>()); }
    void opMul() override { m_Expressions.push_back(make_unique<CMul>()); }
    void opDiv() override { m_Expressions.push_back(make_unique<CDiv>()); }
    void opPow() override { m_Expressions.push_back(make_unique<CPow>()); }
    void opNeg() override { m_Expressions.push_back(make_unique<CNeg>()); }

    // Comparison operators
    void opEq() override { m_Expressions.push_back(make_unique<CEq>()); }
    void opNe() override { m_Expressions.push_back(make_unique<CNe>()); }
    void opLt() override { m_Expressions.push_back(make_unique<CLt>()); }
    void opLe() override { m_Expressions.push_back(make_unique<CLe>()); }
    void opGt() override { m_Expressions.push_back(make_unique<CGt>()); }
    void opGe() override { m_Expressions.push_back(make_unique<CGe>()); }

    // Literal values
    void valNumber(double val) override { m_Expressions.push_back(make_unique<CNumber>(val)); }
    void valString(string val) override { m_Expressions.push_back(make_unique<CString>(val)); }
    void valReference(string val) override { m_Expressions.push_back(make_unique<CReference>(val)); }

    // Returns a copy of the expression stack (AST)
    vector<AExpr> getExpressions() const {
        return copyExpressions(m_Expressions);
    }

    // Clears the current expression stack
    void clearExpressions() {
        m_Expressions.clear();
    }

    // Copy assignment
    ExpressionBuilder &operator =(const ExpressionBuilder &src) {
        if (this != &src) m_Expressions = src.getExpressions();
        return *this;
    };

    // Copy constructor
    ExpressionBuilder(const ExpressionBuilder &src) {
        m_Expressions = src.getExpressions();
    };

    // Move constructor
    ExpressionBuilder(ExpressionBuilder &&src) noexcept : m_Expressions(std::move(src.m_Expressions)) {
    }

private:
    vector<AExpr> m_Expressions; // Stores AST nodes in postfix order
};
