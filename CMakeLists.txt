set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(unused-funcs
                     src/unused-funcs.cpp
                     src/Finder.cpp
                     src/FuncInfo.cpp
                     src/RefInfo.cpp)
target_link_libraries(unused-funcs clangTooling clangBasic clangASTMatchers)
