#pragma once
#include "CExpr.h"
#include "CPos.h"
#include <string>
#include <stack>
#include <cmath>

/* Helper function: Pop two top values from the stack for binary operations.
 * Returns them as a pair {lhs, rhs}. */
inline pair<CValue, CValue> getTwoTopValues(stack<CValue> &values) {
    CValue rhs = values.top();
    values.pop();
    CValue lhs = values.top();
    values.pop();
    return make_pair(lhs, rhs);
}

/* Binary arithmetic expressions
 * Implements standard arithmetic: +, -, *, /, ^ (power)
 * Operands must be double; + also supports string concatenation. */

// Addition / concatenation
class CAdd : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CAdd(*this);
        return unique_ptr<CAdd>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (values.size() < 2) return false;

        // Pop the two top values from the stack
        auto [lhs, rhs] = getTwoTopValues(values);

        // Case 1: both are numbers → perform addition
        if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
            double left = get<double>(lhs);
            double right = get<double>(rhs);
            values.emplace(left + right);
            return true;
        }

        // Case 2: concatenate if both are non-empty (string + string, string + number, number + string)
        if (!holds_alternative<monostate>(lhs) && !holds_alternative<monostate>(rhs)) {
            auto doubleToStr = [](double val) -> string {
                string s = to_string(val);
                // Remove trailing zeros and dot
                s.erase(s.find_last_not_of('0') + 1, string::npos);
                if (!s.empty() && s.back() == '.') s.pop_back();
                return s;
            };

            string left = holds_alternative<double>(lhs) ? doubleToStr(get<double>(lhs)) : get<string>(lhs);
            string right = holds_alternative<double>(rhs) ? doubleToStr(get<double>(rhs)) : get<string>(rhs);

            // Concatenate strings
            values.emplace(left + right);
            return true;
        }

        return false; // Not enough operands or cannot concatenate
    }

    void print(ostream &os) const override {
        os << " 0 ";
    }
};

// Subtraction
class CSub : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CSub(*this);
        return unique_ptr<CSub>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (values.size() >= 2) {
            // Get two values from top of the stack
            auto [lhs, rhs] = getTwoTopValues(values);

            // Check if they are double
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double left = get<double>(lhs);
                double right = get<double>(rhs);
                values.emplace(left - right);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 1 ";
    }
};

// Multiplication
class CMul : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CMul(*this);
        return unique_ptr<CMul>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (values.size() >= 2) {
            // Get two values from top of the stack
            auto [lhs, rhs] = getTwoTopValues(values);

            // Check if they are double
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double left = get<double>(lhs);
                double right = get<double>(rhs);
                values.emplace(left * right);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 2 ";
    }
};

// Division (ignores division by zero)
class CDiv : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CDiv(*this);
        return unique_ptr<CDiv>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (values.size() >= 2) {
            // Get two values from top of the stack
            auto [lhs, rhs] = getTwoTopValues(values);

            // Check if they are double
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double left = get<double>(lhs);
                double right = get<double>(rhs);

                // Check if right isn't zero
                if (right != 0) {
                    values.emplace(left / right);
                    return true;
                }
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 3 ";
    }
};

// Exponentiation
class CPow : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CPow(*this);
        return unique_ptr<CPow>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (values.size() >= 2) {
            // Get two values from top of the stack
            auto [lhs, rhs] = getTwoTopValues(values);

            // Check if the operands are double
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double left = get<double>(lhs);
                double right = get<double>(rhs);
                values.emplace(pow(left, right));
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 4 ";
    }
};

// Unary minus
class CNeg : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CNeg(*this);
        return unique_ptr<CNeg>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        if (!values.empty()) {
            CValue operand = values.top();
            values.pop();

            // Check if the operand is double
            if (holds_alternative<double>(operand)) {
                double val = get<double>(operand);
                values.emplace(-val);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 5 ";
    }
};

/* Relational / comparison expressions
 * Result: numeric 1 (true) or 0 (false). Operands must be both numbers or both strings. */

// Equality ==
class CEq : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CEq(*this);
        return unique_ptr<CEq>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) == get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) == get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 6 ";
    }
};

// Inequality !=
class CNe : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CNe(*this);
        return unique_ptr<CNe>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) != get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) != get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 7 ";
    }
};

// Less than <
class CLt : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CLt(*this);
        return unique_ptr<CLt>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) < get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) < get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 8 ";
    }
};

// Less or equal <=
class CLe : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CLe(*this);
        return unique_ptr<CLe>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) <= get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) <= get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 9 ";
    }
};

// Greater than >
class CGt : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CGt(*this);
        return unique_ptr<CGt>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) > get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) > get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 10 ";
    }
};

// Greater or equal >=
class CGe : public CExpr {
public:
    AExpr clone() const override {
        auto newExpr = new CGe(*this);
        return unique_ptr<CGe>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        // Check if there are at least two values on the stack
        if (values.size() >= 2) {
            auto [lhs, rhs] = getTwoTopValues(values);

            // Two doubles
            if (holds_alternative<double>(lhs) && holds_alternative<double>(rhs)) {
                double res = get<double>(lhs) >= get<double>(rhs);
                values.emplace(res);
                return true;
            }

            // Two strings
            if (holds_alternative<string>(lhs) && holds_alternative<string>(rhs)) {
                double res = get<string>(lhs) >= get<string>(rhs);
                values.emplace(res);
                return true;
            }
        }

        return false;
    }

    void print(ostream &os) const override {
        os << " 11 ";
    }
};

// Literal expressions

class CNumber : public CExpr {
public:
    explicit CNumber(double number) : m_Number(number) {
    }

    AExpr clone() const override {
        auto newExpr = new CNumber(*this);
        return unique_ptr<CNumber>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        values.emplace(m_Number);
        return true;
    }

    void print(ostream &os) const override {
        os << " 12 " << m_Number << " ";
    }

private:
    double m_Number;
};

class CString : public CExpr {
public:
    explicit CString(string str) : m_String(std::move(str)) {
    }

    AExpr clone() const override {
        auto newExpr = new CString(*this);
        return unique_ptr<CString>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override {
        values.emplace(m_String);
        return true;
    }

    void print(ostream &os) const override {
        os << " 13 " << m_String << " endOfString ";
    }

private:
    string m_String;
};

/* Cell reference
 * Supports relative and absolute references.
 * When copied, can adjust its position (changePosition). */
class CReference : public CExpr {
public:
    explicit CReference(const string &str) : m_Pos(str) {
    }

    AExpr clone() const override {
        auto newExpr = new CReference(*this);
        return unique_ptr<CReference>(newExpr);
    }

    bool getValue(CSpreadsheet &sheet, stack<CValue> &values) const override;

    void changePosition(int colOffset, int rowOffset) override {
        m_Pos.changePosition(colOffset, rowOffset);
    }

    void print(ostream &os) const override {
        os << " 14 " << m_Pos;
    }

private:
    CPos m_Pos;
};
