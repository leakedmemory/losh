# Low Optmized Shell (Losh)

A simple shell for learning purposes.

## Building from Source

This project can only be compiled on a Linux environment, as it uses Linux
syscalls directly without preprocessor conditions to determine platform-specific
code.

Ensure you have the following dependencies installed:

- [GNU Make](https://www.gnu.org/software/make/)
- [gcc](https://gcc.gnu.org/)

To compile the code, use the following commands:

```bash
$ make      # to build
$ make run  # to build and run
```

To remove generated files, use:

```bash
$ make clean      # to remove the `.o`, `.d`, and binary files
$ make clean_all  # to remove these files from both the main application and the tests
```

## Developing

### File Structure

- `src/`: Contains the source code.
- `obj/`: Created during compilation, contains compiled objects and
  dependencies.
- `tests/`: Contains the project's test files, with a similar structure to the
  main source code.
- `compile_flags.txt`: Specifies include paths for the project to help the LSP
  in recognizing those paths, acting as a simpler version of
  `compile_commands.json` and the
  [`.clangd` config file](https://clangd.llvm.org/config.html).

### Testing

This project uses the [GoogleTest](https://github.com/google/googletest)
framework for testing. Note that GoogleTest is not downloaded automatically when
running tests for the first time, which is not the
[recommended usage](https://github.com/google/googletest/blob/36066cfecf79267bdf46ff82ca6c3b052f8f633c/googletest/docs/faq.md#why-is-it-not-recommended-to-install-a-pre-compiled-copy-of-google-test-for-example-into-usrlocal)
for it, but this project is just for learning.

Since GoogleTest is written in C++, the test files are also C++ files but call
the C code using
[FFI](https://en.wikipedia.org/wiki/Foreign_function_interface).

To run the tests, use:

```bash
make tests
```

## What have I learned from this?

- C properties
- Creating Makefiles for projects with multiple files
- Using GDB
- Creating and using `clang-format`
- Making syscalls directly from C
- Pipes and communication between processes
- IO redirection
- Writing tests in C
- Input parsing
- Basic FFI use
