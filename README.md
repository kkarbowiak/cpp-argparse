# C++ argparse

[![Build argparse](https://github.com/kkarbowiak/cpp-argparse/actions/workflows/ci.yml/badge.svg)](https://github.com/kkarbowiak/cpp-argparse/actions/workflows/ci.yml)
[![Version](https://img.shields.io/github/v/tag/kkarbowiak/cpp-argparse?sort=semver)](https://github.com/kkarbowiak/cpp-argparse/releases)
[![Codecov](https://codecov.io/gh/kkarbowiak/cpp-argparse/branch/master/graph/badge.svg)](https://codecov.io/gh/kkarbowiak/cpp-argparse)
![Standard](https://img.shields.io/badge/C%2B%2B-20-blue)
[![License](https://img.shields.io/github/license/kkarbowiak/cpp-argparse)](https://github.com/kkarbowiak/cpp-argparse/blob/master/LICENSE.md)

## Overview

This is a C++ implementation of Python's `argparse` module. The aim is to cover as much functionality as possible and retain `argparse`'s familiarity and ease of use. Everyone who already used Python should feel at home using this library. At the same time, the library is implemented using modern C++ features.

## Dependencies

C++ argparse is a header-only library, so its setup is minimal. It has no external dependencies and only uses STL. Since it uses `std::ranges` and some other features, it requires C++20 compiler and standard library.

C++ argparse uses CMake internally, but you don't have to. Just put the [argparse.hpp](https://github.com/kkarbowiak/cpp-argparse/releases/latest/download/argparse.hpp) header somewhere and point your build system to it.

Unit tests use [doctest](https://github.com/onqtam/doctest) unit testing framework.

## Design considerations

This library strives to provide Python-like arguments parsing experience for C++ developers. However, where Python and C++ clash, this implementation goes the C++ way using its idioms.

### Named parameters

One obvious example is the lack of C++ support for named function parameters. While in Python you can do

```python
parser = argparse.ArgumentParser(prog='app', usage='%(prog)s', description='Showcase app')
```

in C++ you need something else. C++ argparse uses Named Parameter Idiom to achieve similar functionality:

```c++
auto parser = argparse::ArgumentParser().prog("app").usage("{prog}").description("Showcase app");
```

It is a bit more verbose and a bit less convenient, but I believe it is still usable.

### Dynamic attribute creation

The `parse_args()` function adds attributes representing parsed arguments to an instance of a `Namespace` class at runtime. In Python creating custom types at runtime is not a problem. Not so in C++. Instead of returning a bespoke class instance, C++ argparse returns a mapping object that uses argument names as keys. This is not as convenient, but not too bad either.

### Type-safety

Python does not have any static type-safety built in and instead depends on runtime checks. Therefore it is fine to pass some things as strings:

```python
parser.add_argument('--option', action='store_true')
```

In C++ we can ask the compiler to help us detect bugs at compile-time, and this is what C++ argparse does. Instead of passing actions as strings, we use `enum`s here:

```c++
parser.add_argument("--option").action(argparse::store_true);
```

If you make a typo, your compiler will let you know.

### Typing

Python is a dynamically typed language while C++ is statically typed. This means that while in Python a function can return any type

```python
def fun(arg):
   if not arg:
       return None
   elif arg == 10:
       return 'ten'
   else:
       return 42
```

in C++ we need a workaround. The workaroud used here is template-based. To get a parsed value, you need to specify which type you expect:

```c++
auto args = parser.parse_args(argc, argv);
auto force = args.get_value<bool>("force");
```

The only exception is `std::string` which is returned by default by the non-template function overload:

```c++
auto args = parser.parse_args(argc, argv);
auto file_name = args.get_value("filename");
```

## Usage

You can follow the [tutorial](tutorial/readme.md) in the project's [`tutorial`](tutorial) subdirectory to learn how to use this library. A short example follows below:

```c++
#include "argparse.hpp"


int move_file(std::string const & src, std::string const & dst, bool force, bool verbose);

int main(int argc, char* argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("source").help("source file path");
    parser.add_argument("destination").help("destination file path");
    parser.add_argument("-f", "--force").help("force copying").action(argparse::store_true);
    parser.add_argument("-v", "--verbose").help("prints additional information").action(argparse::store_true);

    auto args = parser.parse_args(argc, argv);

    auto result = move_file(
        args.get_value("source"),
        args.get_value("destination"),
        args.get_value<bool>("force"),
        args.get_value<bool>("verbose"));

    return result;
}
```

## Supported features

The below lists features of the `argparse` module that this implementation supports:
* `ArgumentParser` objects
   * [x] `prog`
   * [x] `usage`
   * [x] `description`
   * [x] `epilog`
   * [ ] `parents`
   * [ ] `formatter_class`
   * [ ] `prefix_chars`
   * [ ] `fromfile_prefix_chars`
   * [ ] `argument_default`
   * [ ] `allow_abbrev`
   * [ ] `conflict_handler`
   * [x] `add_help`

* The `add_argument()` method
   * [x] name or flags
   * [x] `action`
      * [x] `store`
      * [x] `store_true`
      * [x] `store_false`
      * [x] `store_const`
      * [x] `count`
      * [x] `append`
      * [x] `help`
      * [x] `version`
   * [x] `nargs`
      * [x] `'N'`
      * [x] `'?'` (as `zero_or_one`)
      * [x] `'*'` (as `zero_or_more`)
      * [x] `'+'` (as `one_or_more`)
   * [x] `const` (renamed to `const_` due to keyword clash)
   * [x] `default` (renamed to `default_` due to keyword clash; only for optional arguments and with no string parsing)
   * [x] `type` (built-in (except for `bool`) and user-defined types (via overloading `from_string` function))
   * [x] `choices`
   * [x] `required`
   * [x] `help`
   * [x] `metavar` (only for single nargs)
   * [x] `dest`

* The `parse_args()` method
   * no defaults, you need to pass `argc` and `argv` explicitly (normally, forward what you got in `main`)
   * option value syntax
      * [x] passing long option and value as a single command-line argument (`--foo=FOO`)
      * [x] passing short option and value concatenated (`-xX`)
      * [x] joining together several short options (`-xyz`)
      * [x] double-dash pseudo-argument (`--`)
      * [ ] argument abbreviations (prefix matching)

* Mutual exclusion
   * you can put optional arguments in a mutually exclusive group to have only one of them accepted by the parser
   * the group itself (together with its arguments) is optional

## License

This project is released under **MIT** license.
