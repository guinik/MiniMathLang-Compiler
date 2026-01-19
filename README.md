# MiniMathLang Compiler

MiniMathLang is a small, statically-typed programming language designed for basic **scalar** and **vector** math operations. It compiles to **C++**, allowing efficient execution while keeping the syntax minimal and easy to read.

---

## Features

* **Statically typed variables**: `int`, `float`, `vec`
* **Basic arithmetic operations**: `+`, `-`, `*`, `/`
* **Vector operations**: element-wise addition, multiplication, and scalar-vector operations
* **Simple and minimal syntax**, inspired by modern statically typed languages

---

## Example Usage

```mini
// Scalar operations
let x: int = 42
let y: int = 8
let sum: int = x + y
print(sum)  // Output: 50

// Float operations
let pi: float = 3.14159
let radius: float = 5.0
let area: float = pi * radius * radius
print(area)  // Output: 78.53975

// Vector operations
let v1: vec = [1.0, 2.0, 3.0]
let v2: vec = [4.0, 5.0, 6.0]
let v_sum: vec = v1 + v2
let v_prod: vec = v1 * v2
print(v_sum)   // Output: [5.0, 7.0, 9.0]
print(v_prod)  // Output: [4.0, 10.0, 18.0]

// Scalar-vector operations
let scaled: vec = v1 * 2.0
print(scaled)  // Output: [2.0, 4.0, 6.0]
```

---

## Project Structure

```
C:\Compiler
│
├─ include/
│   ├─ ast.h
│   ├─ codegen.h
│   ├─ lexer.h
│   ├─ parser.h
│   └─ typechecker.h
│
├─ codegen.cpp
├─ lexer.cpp
├─ main.cpp
├─ parser.cpp
└─ typechecker.cpp
```

* `lexer.cpp/h` – Lexical analysis (tokenizer)
* `parser.cpp/h` – Parsing and AST generation
* `typechecker.cpp/h` – Enforces static type correctness
* `codegen.cpp/h` – Generates equivalent C++ code from AST
* `main.cpp` – Entry point of the compiler

---

## Building and Running

MiniMathLang comes with a **Dockerfile** for easy compilation and execution.

1. **Build the Docker image:**

```bash
docker build -t minimathlang .
```

2. **Run the container interactively:**

```bash
docker run -it minimathlang
```

3. **Inside the container: Build and run the compiler:**

```bash
mkdir build
cd build
cmake ..
make
./output path/to/source.minimath
```

Replace `path/to/source.mml` with the path to your MiniMathLang source file.

---

## License

This project is licensed under the MIT License.
