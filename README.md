Brief description
-----------------

Clang-based standalone tool that diagnoses the following cases of defining 
functions:

 - completely unused functions declared as `extern`;
 - functions declared as `extern` which are used only locally and thus can be 
 made `static`.

Example
-------

Input:

```c
static void firstStatic(void);
static void secondStatic(void);

void firstExtern(void);

extern void secondExtern(void);

static void firstStatic(void)  { }
static void secondStatic(void) { }

void firstExtern(void)  { }
void secondExtern(void) { }

int
main(void)
{
    firstExtern();
    secondStatic();
    return 0;
}
```

Run command:

```sh
bin/unused-funcs main.c --
```

Output (paths are truncated):

```
.../main.c:20:firstExtern:can be made static
.../main.c:25:secondExtern:unused
```

Building
--------

1. Clone the repository to `llvm/tools/clang/tools/extra`.
2. Add `add_subdirectory(unused-funcs)` to `CMakeLists.txt` in the same 
   directory.
3. Run `ninja unused-funcs` inside your LLVM's `build/` directory.
