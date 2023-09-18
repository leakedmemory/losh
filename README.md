# Low Optmized Shell (Losh)

A simple shell for learning purposes.

## Building from Source

It is important to say that this project can only be compiled in a linux
enviroment, since it uses the linux syscalls direcly, instead of having a
preprocessor condition to determine which code has to be compiled.

That been said, make sure to have installed the following dependencies:

- [GNU Make](https://www.gnu.org/software/make/)
- [gcc](https://gcc.gnu.org/)

To compile the code, use the following commands:

```bash
$ make      # to build
$ make run  # to build and run
```

## Developing

### File Structure

Within the `src` directory is where the source code is located. When you
compile the code, a directory will be created on the root directory: `obj`.
It will contain the compiled objects and dependencies files.

In the `tests` directory are located the tests from the project. It's similarly
to the proccess described before, but with the root directory for it being `tests`.

Lastly, the `compile_flags.txt` file is destinated to put the include paths of
the project, been only a simple mechanism, in this project, to make the LSP
recognize those paths, and a simpler version of `compile_commands.json` and
`.clangd` [config file](https://clangd.llvm.org/config.html).

## Testing

Unfortunately, to run the tests of this project, it is necessary to have
installed on the machine the [GoogleTest](https://github.com/google/googletest)
framework, since it is not downloaded on the fly when trying to run the tests
for the first time. I know that [this is NOT the correct way of using it](
https://github.com/google/googletest/blob/36066cfecf79267bdf46ff82ca6c3b052f8f633c/googletest/docs/faq.md#why-is-it-not-recommended-to-install-a-pre-compiled-copy-of-google-test-for-example-into-usrlocal),
but this is just for learning anyway.

Since the GoogleTest framework is written in C++, the test files also happen
to be C++ files, but of course calling the source code in C ([FFI](
https://en.wikipedia.org/wiki/Foreign_function_interface)).

To run the tests, use the command below and make sure the object files for
testing are already compiled, otherwise the tests will fail.

```bash
make tests
```

## What have I learned with this?

- C properties
- How to create a makefile in projects with more than just a couple of files
- How to use GDB
- How to create and use clang-format
- How to make syscalls directly from C
- Pipes and communication between processes
- I/O redirection
- Tests in C
- Input parsing
