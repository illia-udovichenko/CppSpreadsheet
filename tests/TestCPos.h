#pragma once
#include "../src/CPos.h"
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <random>

using namespace std;

class TestCPos {
public:
    TestCPos() {
        testSuccessCases();
        testEdgeCases();
        testErrorCases();
        testRandomCases();
    }

private:
    static void testSuccessCases() {
        // Basic single-letter columns
        {
            CPos p("A1");
            assert(p.getCol() == 0);
            assert(p.getRow() == 1);
            assert(!(p < CPos("A1"))); // same pos

            ostringstream os;
            os << p;
            string out = os.str();
            assert(out.find("A1") != string::npos);
        }

        // Multi-letter columns
        {
            CPos p("AA10");
            assert(p.getCol() == 26); // A=0..Z=25 → AA = 26
            assert(p.getRow() == 10);

            CPos q("AB10");
            assert(p < q);
        }

        // Absolute column & row
        {
            CPos p("$B$2");
            assert(p.getCol() == 1);
            assert(p.getRow() == 2);

            ostringstream os;
            os << p;
            string out = os.str();
            assert(out.find("$B$2") != string::npos);
        }

        // Mixed absolute and relative
        {
            CPos p("$C3");
            assert(p.getCol() == 2);
            assert(p.getRow() == 3);

            ostringstream os;
            os << p;
            string out = os.str();
            assert(out.find("$C3") != string::npos);
        }
    }

    static void testEdgeCases() {
        // Very large column (e.g., "ZZZ1")
        {
            CPos p("ZZZ1");
            int expected = 26 * 26 * 26 + 26 * 26 + 25;
            assert(p.getCol() == expected);
        }

        // Change position respects absolute flags
        {
            CPos p("A1");
            p.changePosition(2, 3);
            assert(p.getCol() == 2);
            assert(p.getRow() == 4);

            CPos q("$B$2");
            q.changePosition(10, 10);
            assert(q.getCol() == 1); // unchanged because of $
            assert(q.getRow() == 2);
        }

        // Operator <
        {
            CPos a("A1"), b("A2"), c("B1");
            assert(a < b);
            assert(a < c);
            assert(!(c < a));
        }
    }

    static void testErrorCases() {
        auto expectThrow = [](const string &input) {
            try {
                CPos p(input);
                assert(false && "Expected exception not thrown");
            } catch (const invalid_argument &) {
                // expected
            }
        };

        expectThrow(""); // empty
        expectThrow("1A"); // number before letter
        expectThrow("$$A1"); // double $
        expectThrow("A"); // missing row
        expectThrow("A1$"); // misplaced $
        expectThrow("A1B"); // garbage at end
        expectThrow("$1"); // missing column
        expectThrow("AB$"); // trailing $
        expectThrow("A!1"); // invalid character
        expectThrow("A-1"); // negative row
    }

    static void testRandomCases() {
        // Test random columns and rows
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> rowDist(0, 1000);
        uniform_int_distribution<int> colDist(1, 100); // number of letters

        for (int i = 0; i < 50; i++) {
            int colLen = colDist(gen);
            string colStr;
            int n = colLen;
            while (n > 0) {
                n--;
                colStr.insert(colStr.begin(), 'A' + (n % 26));
                n /= 26;
            }

            int row = rowDist(gen);
            CPos p(colStr + to_string(row));
            assert(p.getRow() == row);
            // Column index should be >= 0
            assert(p.getCol() >= 0);
        }
    }
};
