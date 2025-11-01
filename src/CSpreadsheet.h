#pragma once
#include "ExpressionBuilder.h"
#include <map>
#include <set>
#include <vector>
#include <memory>
using namespace std;

constexpr unsigned SPREADSHEET_CYCLIC_DEPS = 1;

// Represents a spreadsheet storing expressions per cell and supporting evaluation
class CSpreadsheet {
public:
    CSpreadsheet() = default;

    static unsigned capabilities() { return SPREADSHEET_CYCLIC_DEPS; }

    // Copy constructor / assignment: deep copy of all cells and their expressions
    CSpreadsheet(const CSpreadsheet &src) {
        // Copy excel map
        for (const auto &pair: src.m_Excel)
            m_Excel[pair.first] = copyExpressions(pair.second);
    }

    CSpreadsheet &operator =(const CSpreadsheet &src) {
        if (this != &src) {
            // Copy excel map
            m_Excel.clear();
            for (const auto &pair: src.m_Excel)
                m_Excel[pair.first] = copyExpressions(pair.second);
        }
        return *this;
    }

    CSpreadsheet(CSpreadsheet &&src) noexcept : m_Excel(std::move(src.m_Excel)) {
    }

    // Load spreadsheet from stream; returns false if input is invalid
    bool load(istream &is) {
        m_Excel.clear();

        // Read until istream finished || caught error
        string position, tmp;
        int vectorLen, token;
        bool success = true; // flag to know if reading finished
        while (is >> tmp) {
            // Get position
            if (tmp != "CPos")
                return false;
            is >> position;
            CPos pos((string_view(position)));

            // Read length of vector
            is >> tmp;
            if (tmp != "VectorLen")
                return false;
            is >> vectorLen;

            // Read and save vector of expressions
            for (int i = 0; i < vectorLen; i++) {
                is >> token;
                switch (token) {
                    case 0: {
                        m_Excel[pos].push_back(make_unique<CAdd>());
                        break;
                    }
                    case 1: {
                        m_Excel[pos].push_back(make_unique<CSub>());
                        break;
                    }
                    case 2: {
                        m_Excel[pos].push_back(make_unique<CMul>());
                        break;
                    }
                    case 3: {
                        m_Excel[pos].push_back(make_unique<CDiv>());
                        break;
                    }
                    case 4: {
                        m_Excel[pos].push_back(make_unique<CPow>());
                        break;
                    }
                    case 5: {
                        m_Excel[pos].push_back(make_unique<CNeg>());
                        break;
                    }
                    case 6: {
                        m_Excel[pos].push_back(make_unique<CEq>());
                        break;
                    }
                    case 7: {
                        m_Excel[pos].push_back(make_unique<CNe>());
                        break;
                    }
                    case 8: {
                        m_Excel[pos].push_back(make_unique<CLt>());
                        break;
                    }
                    case 9: {
                        m_Excel[pos].push_back(make_unique<CLe>());
                        break;
                    }
                    case 10: {
                        m_Excel[pos].push_back(make_unique<CGt>());
                        break;
                    }
                    case 11: {
                        m_Excel[pos].push_back(make_unique<CGe>());
                        break;
                    }
                    case 12: {
                        double number;
                        is >> number;
                        m_Excel[pos].push_back(make_unique<CNumber>(number));
                        break;
                    }
                    case 13: {
                        // Read string until met "endOfString"
                        tmp = "";
                        string result;
                        while (is >> tmp && tmp != "endOfString")
                            result += tmp;

                        m_Excel[pos].push_back(make_unique<CString>(result));
                        break;
                    }
                    case 14: {
                        is >> tmp; // "CPos"
                        is >> tmp; // "real" CPos
                        m_Excel[pos].push_back(make_unique<CReference>(tmp));
                        break;
                    }
                    default:
                        return false;
                }
            }

            // Set reading flag
            success = !is.fail();
        }

        // Check if reading finished successfully
        return success;
    }

    // Save spreadsheet to stream
    bool save(ostream &os) const {
        for (const auto &pair: m_Excel) {
            // Save cell if it is not empty
            int size = (int) pair.second.size();
            if (size) {
                // Print position
                os << pair.first;

                // Print all expressions
                os << " VectorLen " << size << " "; // to know how much to read
                for (const auto &expr: pair.second)
                    os << expr;
            }
        }

        return true;
    }

    // Set contents of a cell (number, string, or expression)
    bool setCell(CPos pos, const string &contents) {
        m_ExprBuilder.clearExpressions(); // clear from previous expressions

        // Create and save vector of all elements (operations, constants, references...)
        try {
            parseExpression(contents, m_ExprBuilder);
        } catch (const exception &e) {
            cerr << "Error while parsing input: " << e.what();
            return false;
        }

        m_Excel[pos] = m_ExprBuilder.getExpressions();
        return true;
    }

    // Evaluate and return value of a cell; returns empty CValue if undefined or cyclic
    CValue getValue(CPos pos) {
        // Check if it is not cycle
        if (calledPositions.find(pos) != calledPositions.end())
            return {};
        calledPositions.insert(pos);

        // Perform all evaluations (result always saved to stack)
        stack<CValue> values;
        for (const auto &expr: m_Excel[pos]) {
            if (!expr->getValue(*this, values)) {
                calledPositions.clear();
                return {};
            }
        }
        // Delete itself position from called set
        calledPositions.erase(pos);

        // Final value
        if (!values.empty())
            return values.top();

        // Empty cell
        return {};
    }

    // Copy a rectangle of cells to a new position (adjusting references)
    void copyRect(CPos dst, CPos src, int w = 1, int h = 1) {
        // Copy selected cells to temporary storage
        CPos dstCopy = dst;
        CPos srcCopy = src;
        map<CPos, vector<AExpr> > newExcel;

        for (int i = 0; i < w; i++) {
            // Reset row for each column
            srcCopy.setRow(src.getRow());
            dstCopy.setRow(dst.getRow());

            for (int j = 0; j < h; j++) {
                // Copy cell
                newExcel[dstCopy] = copyExpressions(m_Excel[srcCopy]);
                srcCopy.setRow(srcCopy.getRow() + 1);
                dstCopy.setRow(dstCopy.getRow() + 1);
            }

            srcCopy.setCol(srcCopy.getCol() + 1);
            dstCopy.setCol(dstCopy.getCol() + 1);
        }

        // Reset dst column
        dstCopy.setCol(dst.getCol());

        // Evaluate offset
        int colOffset = dst.getCol() - src.getCol();
        int rowOffset = dst.getRow() - src.getRow();

        // Write new cells to dst position
        for (int i = 0; i < w; i++) {
            // Reset row
            dstCopy.setRow(dst.getRow());

            for (int j = 0; j < h; j++) {
                // Change expressions position inside the cell
                m_Excel[dstCopy] = changePositions(newExcel[dstCopy], colOffset, rowOffset);
                dstCopy.setRow(dstCopy.getRow() + 1);
            }

            dstCopy.setCol(dstCopy.getCol() + 1);
        }
    }

private:
    map<CPos, vector<AExpr> > m_Excel; // maps cell positions to their expressions
    ExpressionBuilder m_ExprBuilder;   // temporary builder for parsing cell contents
    set<CPos> calledPositions;         // tracks cells during evaluation to detect cycles

    // Return a copy of expressions with updated positions
    static vector<AExpr> changePositions(const vector<AExpr> &expressions, int colOffset, int rowOffset) {
        vector<AExpr> changedExpressions;
        changedExpressions.reserve(expressions.size());

        for (const auto &expr: expressions) {
            changedExpressions.push_back(expr->clone());
            changedExpressions.back()->changePosition(colOffset, rowOffset);
        }

        return changedExpressions;
    }
};
