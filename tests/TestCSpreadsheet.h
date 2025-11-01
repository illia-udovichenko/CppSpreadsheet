#pragma once
#include "../src/CSpreadsheet.h"
#include <sstream>
#include <cassert>
#include <cfloat>

class TestCSpreadsheet {
public:
    TestCSpreadsheet() {
        testBasicOperations();
        testNumberOperations();
        testAdvancedOperations();
        testComparisonOperators();
        testStringOperations();
        testReferencesAndCycles();
        testCopyRect();
        testSaveLoad();
        testFullWorkflow();
    }

private:
    // Basic set/get operations with numbers and strings
    static void testBasicOperations() {
        CSpreadsheet sheet;

        // Number
        assert(sheet.setCell(CPos("A1"), "123"));
        assert(get<double>(sheet.getValue(CPos("A1"))) == 123);

        // String
        assert(sheet.setCell(CPos("B1"), "Hello"));
        assert(get<string>(sheet.getValue(CPos("B1"))) == "Hello");

        // Arithmetic expression
        assert(sheet.setCell(CPos("C1"), "=1+2*3")); // postfix: 1 2 3 * +
        assert(get<double>(sheet.getValue(CPos("C1"))) == 7);
    }

    // Numeric operations: +, -, *, /, ^, unary -
    static void testNumberOperations() {
        CSpreadsheet sheet;

        assert(sheet.setCell(CPos("A1"), "=1+2"));
        assert(get<double>(sheet.getValue(CPos("A1"))) == 3);

        assert(sheet.setCell(CPos("B1"), "=5-3"));
        assert(get<double>(sheet.getValue(CPos("B1"))) == 2);

        assert(sheet.setCell(CPos("C1"), "=4*2"));
        assert(get<double>(sheet.getValue(CPos("C1"))) == 8);

        assert(sheet.setCell(CPos("D1"), "=8/2"));
        assert(get<double>(sheet.getValue(CPos("D1"))) == 4);

        assert(sheet.setCell(CPos("E1"), "=2^3"));
        assert(get<double>(sheet.getValue(CPos("E1"))) == 8);

        assert(sheet.setCell(CPos("F1"), "=-5"));
        assert(get<double>(sheet.getValue(CPos("F1"))) == -5);
    }

    // Exponentiation and unary minus
    static void testAdvancedOperations() {
        CSpreadsheet sheet;

        assert(sheet.setCell(CPos("A1"), "=2^3"));
        assert(get<double>(sheet.getValue(CPos("A1"))) == 8);

        assert(sheet.setCell(CPos("B1"), "=-A1"));
        assert(get<double>(sheet.getValue(CPos("B1"))) == -8);
    }

    // Comparison operators
    static void testComparisonOperators() {
        CSpreadsheet sheet;

        assert(sheet.setCell(CPos("A1"), "=1=1")); // true
        assert(get<double>(sheet.getValue(CPos("A1"))) == 1);

        assert(sheet.setCell(CPos("B1"), "=1<>2")); // true
        assert(get<double>(sheet.getValue(CPos("B1"))) == 1);

        assert(sheet.setCell(CPos("C1"), "=2<3")); // true
        assert(get<double>(sheet.getValue(CPos("C1"))) == 1);

        assert(sheet.setCell(CPos("D1"), "=3<=3")); // true
        assert(get<double>(sheet.getValue(CPos("D1"))) == 1);

        assert(sheet.setCell(CPos("E1"), "=4>3")); // true
        assert(get<double>(sheet.getValue(CPos("E1"))) == 1);

        assert(sheet.setCell(CPos("F1"), "=4>=4")); // true
        assert(get<double>(sheet.getValue(CPos("F1"))) == 1);
    }

    // String concatenation using +
    static void testStringOperations() {
        CSpreadsheet sheet;

        assert(sheet.setCell(CPos("A1"), "=\"Hello\"+\"World\""));
        assert(get<string>(sheet.getValue(CPos("A1"))) == "HelloWorld");

        // Concatenate string + number
        assert(sheet.setCell(CPos("B1"), "=\"Val\"+123"));
        assert(get<string>(sheet.getValue(CPos("B1"))) == "Val123");

        // Concatenate number + string
        assert(sheet.setCell(CPos("C1"), "=456+\"Test\""));
        assert(get<string>(sheet.getValue(CPos("C1"))) == "456Test");
    }

    // References and cycle detection
    static void testReferencesAndCycles() {
        CSpreadsheet sheet;

        // Reference to another cell
        assert(sheet.setCell(CPos("A1"), "100"));
        assert(sheet.setCell(CPos("B1"), "=A1+50"));
        assert(get<double>(sheet.getValue(CPos("B1"))) == 150);

        // Cyclic reference detection
        assert(sheet.setCell(CPos("C1"), "=D1+1"));
        assert(sheet.setCell(CPos("D1"), "=C1+1"));
        CValue valC1 = sheet.getValue(CPos("C1"));
        CValue valD1 = sheet.getValue(CPos("D1"));
        assert(holds_alternative<monostate>(valC1));
        assert(holds_alternative<monostate>(valD1));
    }

    // Copying ranges of cells
    static void testCopyRect() {
        CSpreadsheet sheet;

        sheet.setCell(CPos("A1"), "5");
        sheet.setCell(CPos("B1"), "=A1+1"); // formula referencing A1

        // Copy single cell
        sheet.copyRect(CPos("C1"), CPos("A1"));
        assert(get<double>(sheet.getValue(CPos("C1"))) == 5);

        // Copy formula with adjustment
        sheet.copyRect(CPos("D1"), CPos("B1"));
        assert(get<double>(sheet.getValue(CPos("D1"))) == 6); // references C1
    }

    // Saving and loading the spreadsheet
    static void testSaveLoad() {
        CSpreadsheet sheet;

        sheet.setCell(CPos("A1"), "123");
        sheet.setCell(CPos("B1"), "Hello");
        sheet.setCell(CPos("C1"), "=1+2");

        stringstream ss;
        assert(sheet.save(ss));

        CSpreadsheet sheet2;
        assert(sheet2.load(ss));

        assert(get<double>(sheet2.getValue(CPos("A1"))) == 123);
        assert(get<string>(sheet2.getValue(CPos("B1"))) == "Hello");
        assert(get<double>(sheet2.getValue(CPos("C1"))) == 3);
    }

    // Compare two CValue variants (numbers, strings, or empty)
    static bool valueMatch(const CValue &r, const CValue &s) {
        if (r.index() != s.index())
            return false;
        if (r.index() == 0)
            return true;
        if (r.index() == 2)
            return get<string>(r) == get<string>(s);
        if (isnan(get<double>(r)) && isnan(get<double>(s)))
            return true;
        if (isinf(get<double>(r)) && isinf(get<double>(s)))
            return    (get<double>(r) < 0 && get<double>(s) < 0)
                   || (get<double>(r) > 0 && get<double>(s) > 0);
        return fabs(get<double>(r) - get<double>(s)) <= 1e8 * DBL_EPSILON * fabs(get<double>(r));
    }

    static void testFullWorkflow() {
        CSpreadsheet x0, x1;
        ostringstream oss;
        istringstream iss;
        string data;

        // Basic cell assignment and retrieval
        assert(x0.setCell(CPos("A1"), "10"));
        assert(x0.setCell(CPos("A2"), "20.5"));
        assert(x0.setCell(CPos("A3"), "3e1"));
        assert(x0.setCell(CPos("A4"), "=40"));
        assert(x0.setCell(CPos("A5"), "=5e+1"));
        assert(x0.setCell(CPos("A6"), "raw text with any characters, including a quote \" or a newline\n"));
        assert(x0.setCell(CPos("A7"),
                          "=\"quoted string, quotes must be doubled: \"\". Moreover, backslashes are needed for C++.\""));
        assert(valueMatch(x0.getValue(CPos("A1")), CValue(10.0)));
        assert(valueMatch(x0.getValue(CPos("A2")), CValue(20.5)));
        assert(valueMatch(x0.getValue(CPos("A3")), CValue(30.0)));
        assert(valueMatch(x0.getValue(CPos("A4")), CValue(40.0)));
        assert(valueMatch(x0.getValue(CPos("A5")), CValue(50.0)));
        assert(valueMatch(x0.getValue(CPos("A6")),
                          CValue("raw text with any characters, including a quote \" or a newline\n")));
        assert(valueMatch(x0.getValue(CPos("A7")),CValue("quoted string, quotes must be doubled: \". Moreover, backslashes are needed for C++.")));
        assert(valueMatch(x0.getValue(CPos("A8")), CValue())); // empty
        assert(valueMatch(x0.getValue(CPos("AAAA9999")), CValue())); // out of bounds

        // Expressions and formula evaluation
        assert(x0.setCell(CPos("B1"), "=A1+A2*A3"));
        assert(x0.setCell(CPos("B2"), "= -A1 ^ 2 - A2 / 2   "));
        assert(x0.setCell(CPos("B3"), "= 2 ^ $A$1"));
        assert(x0.setCell(CPos("B4"), "=($A1+A$2)^2"));
        assert(x0.setCell(CPos("B5"), "=B1+B2+B3+B4"));
        assert(x0.setCell(CPos("B6"), "=B1+B2+B3+B4+B5"));
        assert(valueMatch(x0.getValue(CPos("B1")), CValue(625.0)));
        assert(valueMatch(x0.getValue(CPos("B2")), CValue(-110.25)));
        assert(valueMatch(x0.getValue(CPos("B3")), CValue(1024.0)));
        assert(valueMatch(x0.getValue(CPos("B4")), CValue(930.25)));
        assert(valueMatch(x0.getValue(CPos("B5")), CValue(2469.0)));
        assert(valueMatch(x0.getValue(CPos("B6")), CValue(4938.0)));

        // Update a cell and check dependent calculations
        assert(x0.setCell(CPos("A1"), "12"));
        assert(valueMatch(x0.getValue(CPos("B1")), CValue(627.0)));
        assert(valueMatch(x0.getValue(CPos("B2")), CValue(-154.25)));
        assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
        assert(valueMatch(x0.getValue(CPos("B4")), CValue(1056.25)));
        assert(valueMatch(x0.getValue(CPos("B5")), CValue(5625.0)));
        assert(valueMatch(x0.getValue(CPos("B6")), CValue(11250.0)));

        // Copy spreadsheet and test independence
        x1 = x0;
        assert(x0.setCell(CPos("A2"), "100"));
        assert(x1.setCell(CPos("A2"), "=A3+A5+A4"));
        assert(valueMatch(x0.getValue(CPos("B1")), CValue(3012.0)));
        assert(valueMatch(x0.getValue(CPos("B2")), CValue(-194.0)));
        assert(valueMatch(x0.getValue(CPos("B3")), CValue(4096.0)));
        assert(valueMatch(x0.getValue(CPos("B4")), CValue(12544.0)));
        assert(valueMatch(x0.getValue(CPos("B5")), CValue(19458.0)));
        assert(valueMatch(x0.getValue(CPos("B6")), CValue(38916.0)));
        assert(valueMatch(x1.getValue(CPos("B1")), CValue(3612.0)));
        assert(valueMatch(x1.getValue(CPos("B2")), CValue(-204.0)));
        assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
        assert(valueMatch(x1.getValue(CPos("B4")), CValue(17424.0)));
        assert(valueMatch(x1.getValue(CPos("B5")), CValue(24928.0)));
        assert(valueMatch(x1.getValue(CPos("B6")), CValue(49856.0)));

        // Save and load spreadsheet
        oss.clear();
        oss.str("");
        assert(x0.save(oss));
        data = oss.str();
        iss.clear();
        iss.str(data);
        assert(x1.load(iss));
        assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
        assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
        assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
        assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
        assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
        assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));

        assert(x0.setCell(CPos("A3"), "4e1"));
        assert(valueMatch(x1.getValue(CPos("B1")), CValue(3012.0)));
        assert(valueMatch(x1.getValue(CPos("B2")), CValue(-194.0)));
        assert(valueMatch(x1.getValue(CPos("B3")), CValue(4096.0)));
        assert(valueMatch(x1.getValue(CPos("B4")), CValue(12544.0)));
        assert(valueMatch(x1.getValue(CPos("B5")), CValue(19458.0)));
        assert(valueMatch(x1.getValue(CPos("B6")), CValue(38916.0)));

        // Load corrupted data should fail
        oss.clear();
        oss.str("");
        assert(x0.save(oss));
        data = oss.str();
        for (size_t i = 0; i <min <size_t>(data.length(), 10); i++)
            data[i] ^= 0x5a;
        iss.clear();
        iss.str(data);

        assert(!x1.load(iss));
        assert(x0.setCell(CPos("D0"), "10"));
        assert(x0.setCell(CPos("D1"), "20"));
        assert(x0.setCell(CPos("D2"), "30"));
        assert(x0.setCell(CPos("D3"), "40"));
        assert(x0.setCell(CPos("D4"), "50"));
        assert(x0.setCell(CPos("E0"), "60"));
        assert(x0.setCell(CPos("E1"), "70"));
        assert(x0.setCell(CPos("E2"), "80"));
        assert(x0.setCell(CPos("E3"), "90"));
        assert(x0.setCell(CPos("E4"), "100"));
        assert(x0.setCell(CPos("F10"), "=D0+5"));
        assert(x0.setCell(CPos("F11"), "=$D0+5"));
        assert(x0.setCell(CPos("F12"), "=D$0+5"));
        assert(x0.setCell(CPos("F13"), "=$D$0+5"));

        // Copy cell ranges and test offsets
        x0.copyRect(CPos("G11"), CPos("F10"), 1, 4);
        assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
        assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
        assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
        assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
        assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
        assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));

        x0.copyRect(CPos("G11"), CPos("F10"), 2, 4);
        assert(valueMatch(x0.getValue(CPos("F10")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F11")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F12")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F13")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("F14")), CValue()));
        assert(valueMatch(x0.getValue(CPos("G10")), CValue()));
        assert(valueMatch(x0.getValue(CPos("G11")), CValue(75.0)));
        assert(valueMatch(x0.getValue(CPos("G12")), CValue(25.0)));
        assert(valueMatch(x0.getValue(CPos("G13")), CValue(65.0)));
        assert(valueMatch(x0.getValue(CPos("G14")), CValue(15.0)));
        assert(valueMatch(x0.getValue(CPos("H10")), CValue()));
        assert(valueMatch(x0.getValue(CPos("H11")), CValue()));
        assert(valueMatch(x0.getValue(CPos("H12")), CValue()));
        assert(valueMatch(x0.getValue(CPos("H13")), CValue(35.0)));
        assert(valueMatch(x0.getValue(CPos("H14")), CValue()));
        assert(x0.setCell(CPos("F0"), "-27"));
        assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));

        // More complex copy with overlapping ranges
        x0.copyRect(CPos("H12"), CPos("H13"), 1, 2);
        assert(valueMatch(x0.getValue(CPos("H12")), CValue(25.0)));
        assert(valueMatch(x0.getValue(CPos("H13")), CValue(-22.0)));
        assert(valueMatch(x0.getValue(CPos("H14")), CValue(-22.0)));
    }
};
