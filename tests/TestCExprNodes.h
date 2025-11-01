#pragma once
#include "../src/CExprNodes.h"
#include <cassert>
#include <stack>
#include <string>

using namespace std;

class TestCExprNodes {
public:
    TestCExprNodes() {
        testNumbers();
        testStrings();
        testArithmetic();
        testComparisons();
    }

private:
    static void testNumbers() {
        stack<CValue> values;
        CNumber n1(5);
        CNumber n2(3.5);

        n1.getValue(*(CSpreadsheet *) nullptr, values);
        n2.getValue(*(CSpreadsheet *) nullptr, values);

        assert(values.size() == 2);
        assert(get<double>(values.top()) == 3.5);
        values.pop();
        assert(get<double>(values.top()) == 5);
        values.pop();
    }

    static void testStrings() {
        stack<CValue> values;
        CString s1("Hello");
        CString s2("World");

        s1.getValue(*(CSpreadsheet *) nullptr, values);
        s2.getValue(*(CSpreadsheet *) nullptr, values);

        assert(values.size() == 2);
        assert(get<string>(values.top()) == "World");
        values.pop();
        assert(get<string>(values.top()) == "Hello");
        values.pop();
    }

    static void testArithmetic() {
        stack<CValue> values;

        values.emplace(2.0);
        values.emplace(3.0);

        CAdd add;
        assert(add.getValue(*(CSpreadsheet*)nullptr, values));
        assert(values.size() == 1);
        assert(get<double>(values.top()) == 5.0);
        values.pop();

        values.emplace(10.0);
        values.emplace(4.0);

        CSub sub;
        assert(sub.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 6.0);
        values.pop();

        values.emplace(2.0);
        values.emplace(3.0);

        CMul mul;
        assert(mul.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 6.0);
        values.pop();

        values.emplace(10.0);
        values.emplace(2.0);

        CDiv div;
        assert(div.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 5.0);
        values.pop();

        values.emplace(2.0);
        values.emplace(3.0);

        CPow pow;
        assert(pow.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 8.0);
        values.pop();

        values.emplace(5.0);

        CNeg neg;
        assert(neg.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == -5.0);
        values.pop();
    }

    static void testComparisons() {
        stack<CValue> values;

        // Equality
        values.emplace(2.0);
        values.emplace(2.0);
        CEq eq;
        assert(eq.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 1);
        values.pop();

        // Inequality
        values.emplace(2.0);
        values.emplace(3.0);
        CNe ne;
        assert(ne.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 1);
        values.pop();

        // Less than
        values.emplace(2.0);
        values.emplace(3.0);
        CLt lt;
        assert(lt.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 1);
        values.pop();

        // Greater or equal
        values.emplace(3.0);
        values.emplace(3.0);
        CGe ge;
        assert(ge.getValue(*(CSpreadsheet*)nullptr, values));
        assert(get<double>(values.top()) == 1);
        values.pop();
    }
};
