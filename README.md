# Low Optmized Shell (Losh)

A simple shell for learning purposes.

## Building from Source

It is important to say that this project can only be compiled in a linux
enviroment, since it uses the linux syscalls direcly, instead of having a
preprocessor condition to determine which code has to be compiled.

That been said, make sure to have installed the following dependencies:

- [GNU Make](https://www.gnu.org/software/make/)
- [gcc](https://gcc.gnu.org/)

You can try changing the compiler in the [Makefile](Makefile) to something that
you prefer, but it is NOT garanteed that it will work by only doing the this,
and maybe you will have to change other things aswell.

Finally, to compile the code you use the following commands:

```bash
$ make      # to build
$ make run  # to build and run
```

The binary output, by default, is the debug version. You can change the binary
to the release version by using the previous commands and adding `RELEASE=1`
to it:

```bash
$ RELEASE=1 make      # or
$ make run RELEASE=1
```

## Developing

### File Structure

Within the `src` directory is where the source code is located. When you
compile the code, two more directories will be created on the root directory:
`bin` and `obj`, both having the binaries executables and the object files,
respectively.

In the `tests` directory are located the tests from the project. Similarly as
the proccess described before, when the tests are compiled, two other directories
will be created, them been `tests/bin` and `tests/obj` and, respectively, storing
the binary executable to run the tests and the object files of them.

Lastly, the `compile_flags.txt` file is destinated to put the include paths of
the project, been only a simple mechanism, in this project, to make the LSP
recognize those paths, and a simpler version of `compile_commands.json` and
`.clangd` [config file](https://clangd.llvm.org/config.html).

## Testing

Unfortunately, to run the tests of this project, it is necessary to have
installed on the machine the [GoogleTest](https://github.com/google/googletest)
framework, since it is not downloaded on the fly when trying to run the tests
for the first time.

Since the GoogleTest framework is written in C++, the test files also happen
to be C++ files, but of course calling the source code in C ([FFI](
https://en.wikipedia.org/wiki/Foreign_function_interface)).

To run the tests, use the command below. Make sure to use the `RELEASE=1` flag,
otherwise it will NOT compile.

```bash
$ RELEASE=1 make test  # or
$ make test RELEASE=1
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

