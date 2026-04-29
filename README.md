# Swift-Load Logistics DSA Final Assignment

This repository contains the C++ source code and report materials for Unit 19: Data Structures & Algorithms, Final Assignment.

## Contents

- `src/main.cpp` - executable C++17 program demonstrating:
  - Goods ADT encapsulation and validation
  - FIFO queue for trucks entering the loading bay
  - LIFO stack behavior for function-call/process control
  - Bubble Sort and QuickSort comparison on 12 cargo weights
  - AVL tree inventory implementation for high-speed retrieval
  - Robust error handling for invalid or missing cargo entries
  - AVL lookup benchmark compared with linear search
- `tests/run_results.txt` - sample output captured from a successful program run
- `Swift_Load_DSA_Final_Report.docx` - assignment report covering P, M, and D criteria

## Build and Run

Use a C++17 compiler such as `clang++` or `g++`.

```bash
mkdir -p build
clang++ -std=c++17 -O2 -Wall -Wextra -pedantic src/main.cpp -o build/swift_load_inventory
./build/swift_load_inventory
```

To capture a fresh test output:

```bash
./build/swift_load_inventory > tests/run_results.txt
```


