#pragma once
#include <stack>
#include <memory>
#include <variant>
#include <string>
#include <vector>

using namespace std;

// Cell value type: empty, number, or string
using CValue = variant<monostate, double, string>;

// Pointer to an expression node
using AExpr = unique_ptr<class CExpr>;

class CSpreadsheet;

/* CExpr - abstract base class for all spreadsheet expressions (numbers, strings, operators, references).
 * Supports evaluation, cloning, and printing. */
class CExpr {
public:
    virtual ~CExpr() = default;

    virtual AExpr clone() const = 0;                            // Deep copy
    virtual bool getValue(CSpreadsheet & sheet, stack<CValue> & values) const = 0;
    virtual void changePosition(int colOffset, int rowOffset) {} // only for references
    virtual void print(ostream & os) const = 0;

    friend ostream & operator << (ostream & os, const AExpr & expression) {
        expression->print(os);
        return os;
    }
};

// Deep copy a vector of expressions
inline vector<AExpr> copyExpressions (const vector<AExpr> & expressions) {
    vector<AExpr> copiedExpressions;
    copiedExpressions.reserve(expressions.size()); // Reserve memory for efficiency

    for (const auto& expr : expressions)
        copiedExpressions.push_back(expr->clone());

    return copiedExpressions;
}