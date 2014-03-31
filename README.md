Brief description
-----------------

Clang-based standalone tool that diagnoses the following cases of defining 
functions:

 - completely unused functions declared as `extern`;
 - functions declared as `extern` which are used only locally and thus can be 
 made `static`.

Building
--------

1. Clone the repository to `llvm/tools/clang/tools/extra`.
2. Add `add_subdirectory(unused-funcs)` to `CMakeLists.txt` in the same 
   directory.
3. Run `ninja unused-funcs` inside your LLVM's `build/` directory.
