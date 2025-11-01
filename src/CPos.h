#pragma once
#include <string_view>
#include <cctype>
#include <stdexcept>
#include <string>
#include <iostream>
using namespace std;

/* CPos - Cell Position
 * Represents a spreadsheet cell (e.g., "A7", "$B$2") with support for
 * absolute/relative references. Provides parsing, comparison, and
 * position adjustment. Throws invalid_argument for invalid input. */
class CPos {
public:
    /* Constructor: parses a string like "A1", "$B$2", etc. into a CPos object.
   Supports absolute column ($) and absolute row ($) references.  */
    explicit CPos(string_view str) : m_Col(0), m_Row(0) {
        // Find first digit which separates column and row
        size_t pos = str.find_first_of("0123456789");
        if (pos >= str.length() || pos == 0)
            throw invalid_argument("No integer or letter in cell identifier.");

        // Extract column and row parts
        string_view columnStr = str.substr(0, pos);
        string_view rowStr = str.substr(pos);

        // Absolute column
        if (!columnStr.empty() && columnStr.front() == '$') {
            m_AbsCol = true;
            columnStr.remove_prefix(1); // Remove the '$' character
        }

        // Absolute row
        if (!columnStr.empty() && columnStr.back() == '$') {
            m_AbsRow = true;
            columnStr.remove_suffix(1); // Remove the '$' character
        }

        // Validate column letters
        if (columnStr.empty())
            throw invalid_argument("Missing column letters in cell identifier.");

        // Convert column letters (A, B, ..., AA) to zero-based index
        for (char c: columnStr) {
            if (!isalpha(c))
                throw invalid_argument("Invalid column part in cell identifier.");
            m_Col *= 26; // (26 letters in the english alphabet)
            m_Col += tolower(c) - 'a' + 1;
        }
        m_Col--; // zero-based

        // Convert row string to integer
        for (char c: rowStr) {
            if (!isdigit(c))
                throw invalid_argument("Invalid row part in cell identifier.");
            m_Row = m_Row * 10 + (c - '0');
        }
    }

    // Lexicographical comparison (needed for using CPos as map key)
    bool operator <(const CPos &other) const {
        if (m_Col != other.m_Col)
            return m_Col < other.m_Col;
        return m_Row < other.m_Row;
    }

    // Change position by offset (respects absolute flags)
    void changePosition(int colOffset, int rowOffset) {
        if (!m_AbsCol) m_Col += colOffset;
        if (!m_AbsRow) m_Row += rowOffset;
    }

    // Stream output: prints a CPos in the standard format (e.g., "CPos $A$1")
    friend ostream &operator <<(ostream &os, const CPos &pos) {
        os << " CPos ";
        if (pos.m_AbsCol) os << "$";

        // Convert zero-based column to letters
        int col = pos.m_Col + 1;
        string result;
        while (col > 0) {
            int rem = (col - 1) % 26;
            result.insert(result.begin(), char('A' + rem));
            col = (col - rem - 1) / 26;
        }
        os << result;

        if (pos.m_AbsRow) os << "$";
        os << pos.m_Row;

        return os;
    }

    // Public getters and setters for column and row
    int getCol() const { return m_Col; }
    int getRow() const { return m_Row; }

    void setCol(int col) {
        if (col >= 0) m_Col = col;
        else throw invalid_argument("Invalid column");
    }

    void setRow(int row) {
        if (row >= 0) m_Row = row;
        else throw invalid_argument("Invalid row");
    }

private:
    int m_Col{0};
    int m_Row{0};
    bool m_AbsCol{false};
    bool m_AbsRow{false};
};
