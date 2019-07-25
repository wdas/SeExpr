# Overview
This demo contains several implementations of a test program that renders a mandelbrot pattern. The different versions are:

1. `mandelbrot-control.cpp` - An implementation with no SeExpr2 dependencies
2. `mandelbrot-control2.cpp` - A modified version of #1 that only uses SeExpr2::Vec
3. `mandelbrot-seexpr2.cpp` - A naive-styled port of the control program to use SeExpr2
4. `mandelbrot-seexpr2-varblock.cpp` - A modified version of #3 that refactors for performance
5. `mandelbrot-seexpr2-varblock2.cpp` - A modified version of #4 that refactors for demonstrating deferred variable declaration

Although these programs are mostly aimed at API demonstrations of SeExpr2 integration, there are some useful performance insights as well:

# Impact of Alignment
At the time of writing, #1 runs 2x faster than #2. The main difference is #1 uses a custom vector implementation, whereas #2 uses SeExpr2::Vec4d. Alignment has a strong effect on performance of this demo program. The code generated with default alignment and `gcc version 4.8.5 20150623 (Red Hat 4.8.5-16) (GCC)` for SeExpr2::Vec4d produces vectorized instructions that are suboptimal for this application.

# Benefits of Eager Variable Definition
\#4 is significantly faster than #3 and #5 by an order of magnitude. SeExpr2 generates different LLVM IR depending on if variables are lazily defined (via subclassing ExprVarRef or assigning a SymbolTable::DeferredVar lambda) or eagerly defined (via assigning a VarBlock pointer). Lazily defined variabes require SeExpr2 to generate LLVM IR that treats variables as non-inlined function calls, whereas eagerly defined variables allow SeExpr2 to generate LLVM IR that treats variables as pointer dereferences (or GEP instructions, in LLVM-speak).