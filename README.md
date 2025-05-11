# Lexical Analyzer and Interpreter

This repository implements a basic **Lexical Analyzer** and **Interpreter** written in C++. It provides functionality to tokenize input, parse it, process logical and arithmetic expressions, and execute simple interpreted statements.

## Features

- **Tokenization**: Converts input into tokens for further processing.
- **Expression Parsing**: Supports arithmetic and logical expressions.
- **Control Statements**: Includes support for `if` statements and similar control structures.
- **Error Handling**: Detects and reports syntax and semantic errors.
- **Input File Support**: Reads input programs from a file for processing.

## Repository Structure

The repository includes the following key components:

- **`lex.cpp`**: Handles lexical analysis, converting raw input into tokens.
- **`parseInt.cpp` and `parseInt.h`**: Implements parsing and interpretation logic, including syntax and semantic checks.
- **`val.cpp` and `val.h`**: Defines the `Value` class for representing data types and performing operations (e.g., addition, subtraction).
- **`prog3.cpp`**: The main entry point for program execution.

## Usage

To use the lexical analyzer and interpreter:

1. Clone this repository:
   ```bash
   git clone https://github.com/MohamedAkash/Lexical-Analyzer-Interpreter.git
   cd Lexical-Analyzer-Interpreter
   ```

2. Compile the source code:
   ```bash
   g++ prog3.cpp lex.cpp parseInt.cpp val.cpp -o interpreter
   ```

3. Run the program with an input file:
   ```bash
   ./interpreter <input_file>
   ```

   Replace `<input_file>` with the path to the file containing your program.

## Example Input Program

Below is an example of a simple program that can be interpreted:

```
PROGRAM Example
BEGIN
    x = 5;
    y = x + 10;
    IF y > 10 THEN
        WRITE y;
    END
END PROGRAM
```

## Limitations

- The interpreter supports only basic control structures and expressions.
- Floating-point arithmetic and type conversion between integers and floats are implemented but limited.
- Error messages are basic and aimed at highlighting syntax or semantic issues.
