# Spreadsheet Processor (CSpreadsheet)

## Overview

This project implements a **spreadsheet processor** in C++, inspired by standard spreadsheet applications. The main
functionality includes:

* Creating and manipulating cells in a spreadsheet.
* Calculating cell values based on formulas.
* Detecting and preventing cyclic dependencies between cells.
* Saving and loading spreadsheet content.
* Handling different value types: numbers, strings, or undefined.

The implementation emphasizes **object-oriented design**, **polymorphism**, and **clean modular code**.

---

## Key Classes

### `CSpreadsheet`

* Core spreadsheet class that stores cell contents and evaluates formulas.
* Supports:

    * Setting cell values (numbers, strings, or formulas).
    * Copying rectangular ranges of cells.
    * Automatic recalculation of dependent cells.
    * Cycle detection to prevent circular references.
    * Saving to and loading from streams.

### `CPos`

* Represents a spreadsheet cell position, e.g., `A1`, `B2`, `AA10`.
* Supports:

    * Absolute and relative references (`$A$1` syntax).
    * Conversion between string identifiers and zero-based internal representation.
    * Adjusting positions during copy operations.

### `CExpressionBuilder` (and subclass `ExpressionBuilder`)

* Abstract interface used by the provided parser.
* Collects operands and operators during parsing and constructs an **abstract syntax tree** (AST) of expressions.
* Supports arithmetic, comparison, and string operations.
* Provides methods for evaluating formulas in a postfix-like order.

### `CValue`

* Represents the value of a cell.
* Implemented as `std::variant<std::monostate, double, std::string>`.
* Holds:

    * `monostate` for undefined cells.
    * `double` for numeric values.
    * `string` for text values.

---

## Features

1. **Formulas and Calculations**

    * Supports basic arithmetic: `+`, `-`, `*`, `/`, `^`.
    * Supports comparison operators: `==`, `!=`, `<`, `<=`, `>`, `>=`.
    * Supports string concatenation of numbers and text.
    * Automatically recalculates dependent cells when a referenced cell changes.

2. **Cell References**

    * Handles both relative and absolute references.
    * Automatically updates references when copying ranges of cells.

3. **Cycle Detection**

    * Prevents infinite loops caused by circular dependencies between cells.

4. **Persistence**

    * Save and load spreadsheet content to/from streams (files or memory).

---

## Testing

* Unit tests are implemented for all major components:

    * `CPos` (cell positions)
    * `CExprNodes` (expression evaluation)
    * `CSpreadsheet` (spreadsheet operations)
* Tests cover:

    * Basic success cases
    * Edge cases
    * Randomized value checks
    * Formula evaluation, copy operations, and persistence

---

## Usage Example

```cpp
#include "CSpreadsheet.h"

int main() {
    CSpreadsheet sheet;
    
    // Set values
    sheet.setCell(CPos("A1"), "10");
    sheet.setCell(CPos("A2"), "20");
    
    // Set formula
    sheet.setCell(CPos("B1"), "=A1+A2*2");
    
    // Get computed value
    auto value = sheet.getValue(CPos("B1")); // 50
    
    return EXIT_SUCCESS;
}
```

---

## Notes on Implementation

* Parsing of formulas is handled via a **provided parser library**; the project only implements the builder to create
  the AST.
* Proper **object-oriented design** is used:

    * Base classes for expressions.
    * Use of `std::unique_ptr` for memory-safe AST management.
    * Polymorphism for evaluating different expression types.
* Designed with **modularity and testability** in mind.