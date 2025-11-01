#include "CExprNodes.h"
#include "CSpreadsheet.h"
using namespace std;

/* This file is separate because the implementation of CReference::getValue
 * requires the full definition of CSpreadsheet. Including CSpreadsheet.h
 * here ensures that getValue can access sheet.getValue(). */
bool CReference::getValue(CSpreadsheet &sheet, stack<CValue> &values) const {
    CValue value = sheet.getValue(m_Pos);

    if (!holds_alternative<monostate>(value)) {
        values.emplace(value);
        return true;
    }
    return false;
}
