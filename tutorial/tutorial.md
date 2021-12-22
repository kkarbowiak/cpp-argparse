# C++ argparse Tutorial

This tutorial will get you familiar with this library and will show you, how you can easily get something useful from the `argc` `argv` duo that the runtime delivers to your `main` function.

This document is based on the tutorial for the Python's argparse library.

## The basics

Let's start with a very simple example that doesn't do much:
```c++
#include "argparse.h"

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.parse_args(argc, argv);
}
```
Following is a result of running the code:
```
$ basic
$ basic --help
usage: basic [-h]

optional arguments:
  -h, --help            show this help message and exit
$ basic --option
unrecognised arguments: --option
usage: basic [-h]

optional arguments:
  -h, --help            show this help message and exit
$ basic foo
unrecognised arguments: foo
usage: basic [-h]

optional arguments:
  -h, --help            show this help message and exit
```
Here is what is happening:
 * Not passing any option does not trigger any action.
 * This shows how you get get handling of `--help` option and generating nice help message for free.
 * The `--help` option can be shortened to `-h` and is added automatically to the parser.
 * Specifying an unknown argument generates an error message (also for free).

 ## Introducing Positional arguments

An example:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("echo");
    auto parsed = parser.parse_args(argc, argv);
    std::cout << parsed.get_value("echo") << '\n';
}
```
And running the code:
```
$ positional
the following arguments are required: echo
usage: positional [-h] echo

positional arguments:
  echo

optional arguments:
  -h, --help            show this help message and exit
$ positional --help
usage: positional [-h] echo

positional arguments:
  echo

optional arguments:
  -h, --help            show this help message and exit
$ positional foo
foo
```
Here is what is happening:
 * We've used the `add_argument` function to specify which command-line options the program is willing to accept. In this case, it's named `echo`.
 * The program now requires us to specify an option.
 * The `parse_args` function actually returns the result of parsing. You can get the argument's value by calling `get_value` on the returned object and giving the option name.
 * By default, the returned value is of type `std::string`.

Note that while the help message looks nice, it is not as helpful as it could be. For example we see that we got `echo` as a positional argument, but we don't know what it does. Let's make it a bit more useful:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("echo").help("echo the string you use here");
    auto parsed = parser.parse_args(argc, argv);
    std::cout << parsed.get_value("echo") << '\n';
}
```
And we get:
```
$ positional1 --help
usage: positional1 [-h] echo

positional arguments:
  echo                  echo the string you use here

optional arguments:
  -h, --help            show this help message and exit
```
Now, how about doing some math:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number");
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value("square");
    std::cout << value * value << '\n';
}
```
Well, this won't compile and your compiler will complain that the `std::string` type does not define a binary `*` operator. As I mentioned, the default type for all options is `std::string`. Let's tell the parser to treat the `square` option as an integer:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    std::cout << value * value << '\n';
}
```
Follwoing is the result of running the code:
```
$ positional3 4
16
$ positional3 four
argument square: invalid value: 'four'
usage: positional3 [-h] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
```
That went well. The program now even detects an invalid value and quits with an informative error message.
