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

## Introducing Optional arguments

So far we have been playing with positional arguments. Let's see how to add optional ones:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--verbosity").help("increase output verbosity");
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get("verbosity"))
    {
        std::cout << "verbosity turned on\n";
    }
}
```
And the output:
```
$ optional --verbosity 1
verbosity turned on
$ optional
$ optional --help
usage: optional [-h] [--verbosity VERBOSITY]

optional arguments:
  -h, --help            show this help message and exit
  --verbosity VERBOSITY
                        increase output verbosity
$ optional --verbosity
argument --verbosity: expected one argument
usage: optional [-h] [--verbosity VERBOSITY]

optional arguments:
  -h, --help            show this help message and exit
  --verbosity VERBOSITY
                        increase output verbosity
```
Here is what is happening:
 * The program is written so as to display something when `--verbosity` is specified and display nothing when not.
 * To show that the option is actually optional, there is no error when running the program without it. Note that by default, it an optional argument isn't used, the object holding the relevant value (the one retrieved using `parsed.get("verbosity")`) is empty and in a boolean context (in an `if` statement) yields `false`.
 * The help message is a bit different.
 * When using the `--verbosity` option, one must specify some arbitrary value.

The above example accepts arbitrary values for `--verbosity`, but for our simple program, only two values are actually useful, `true` and `false`. Let's modify the code accordingly:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--verbose").help("increase output verbosity").action(argparse::store_true);
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get_value<bool>("verbose"))
    {
        std::cout << "verbosity turned on\n";
    }
}
```
And the output:
```
$ optional1 --verbose
verbosity turned on
$ optional1 --verbose 1
unrecognised arguments: 1
usage: optional1 [-h] [--verbose]

optional arguments:
  -h, --help            show this help message and exit
  --verbose             increase output verbosity
$ optional1 --help
usage: optional1 [-h] [--verbose]

optional arguments:
  -h, --help            show this help message and exit
  --verbose             increase output verbosity
```
Here is what is happening:
 * The option is now more of a flag than something that requires a value. We even changed the name of the option to match that idea. Note that we now used a new function, `action`, and passed it the value `argparse::store_true`. This means that, if the option is specified, assing the value `true` to the relevant object. Not speficying it implies `false`.
 * We now need to retireve the value using `parsed.get_value<bool>()` as we now expect the value of type `bool` instead of the usual `std::string`.
 * The option now complains when you specify a value, in true spirit of what flags actually are.
 * Notice the different help text.

### Short options

If you are familiar with command line usage, you will notice that I haven't touched on the topic of short versions of the options. It's quite simple:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("-v", "--verbose").help("increase output verbosity").action(argparse::store_true);
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get_value<bool>("verbose"))
    {
        std::cout << "verbosity turned on\n";
    }
}
```
And the output:
```
$ optional2 -v
verbosity turned on
$ optional2 --help
usage: optional2 [-h] [-v]

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         increase output verbosity
```
Note that the new ability is also reflected in the help text.

## Combining Positional and Optional arguments

Our program keeps growing in complexity:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbose").help("increase output verbosity").action(argparse::store_true);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    if (parsed.get_value<bool>("verbose"))
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
And now the output:
```
$ complex
the following arguments are required: square
usage: complex [-h] [-v] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         increase output verbosity
$ complex 4
16
$ complex 4 --verbose
the square of 4 equals 16
$ complex --verbose 4
the square of 4 equals 16
```
 * We have brought back a positional argument, hence the complaint.
 * Note that the order does not matter.

How about we give this program of ours back the ability to have multiple verbosity values, and actually get to use them:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").type<int>();
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get("verbosity");
    if (verbosity && verbosity.get<int>() == 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity && verbosity.get<int>() == 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
One new thing to note here is the new way of extracting a value for the argument that may be missing. We cannot simply call `parsed.get_value<int>("verbosity")`, because the underlying object may be empty. Instead, we can extract the object itself (`auto verbosity = parsed.get("verbosity")`) and use it in a boolean context to check whether it contains a value. Then, we can extract the value itself (`verbosity.get<int>()`).

And the output:
```
$ complex1 4
16
$ complex 4 -v
argument -v/--verbosity: expected one argument
usage: complex1 [-h] [-v VERBOSITY] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v VERBOSITY, --verbosity VERBOSITY
                        increase output verbosity
$ complex1 4 -v 1
4^2 == 16
$ complex1 4 -v 2
the square of 4 equals 16
$ complex1 4 -v 3
16
```
The above outputs all look good except the last one, which exposes a bug in our program. Let's fix it by restricting the values the `--verbosity` option can accept:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").type<int>().choices({0, 1, 2});
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get("verbosity");
    if (verbosity && verbosity.get<int>() == 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity && verbosity.get<int>() == 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
And the output:
```
$ complex2 -v 3
argument -v/--verbosity: invalid choice: 3 (choose from 0, 1, 2)
usage: complex2 [-h] [-v {0,1,2}] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v {0,1,2}, --verbosity {0,1,2}
                        increase output verbosity
```
Note that the change reflects in both the error message and the help string.

### Default values

One way to remove the need of getting the value object and doing a boolean test before extracting the value is to give the option a default value:
```c++
#include "argparse.h"
#include <iostream>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").type<int>().default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity == 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity == 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
Note that the function for specifying a default value is spelled with an underscore as suffix to prevent clash with language keyword.

The output:
```
$ complex3 4
16
$ complex3 4 --verbosity 1
4^2 == 16
$ complex3 4 --verbosity 2
the square of 4 equals 16
```
As expected, if the argument is not specified, it assumes its default value.

## Getting a little more advanced

What if we wanted to expand our tiny program to perform other powers, not just squares:
```c++
#include "argparse.h"
#include <iostream>
#include <cmath>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    parser.add_argument("-v", "--verbosity").type<int>().default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << base << " to the power " << exp << " equals " << answer << '\n';
    }
    else if (verbosity >= 1)
    {
        std::cout << base << "^" << exp << " == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
Output:
```
$ advanced
the following arguments are required: x y
usage: advanced [-h] [-v VERBOSITY] x y

positional arguments:
  x                     the base
  y                     the exponent

optional arguments:
  -h, --help            show this help message and exit
  -v VERBOSITY, --verbosity VERBOSITY
$ advanced 4 2 -v 1
4^2 == 16
```
Notice that so far we've been using verbosity to *change* the text that gets displayed. The following example instead uses verbosity to display *more* text instead:
```c++
#include "argparse.h"
#include <iostream>
#include <cmath>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    parser.add_argument("-v", "--verbosity").type<int>().default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << "Running '" << argv[0] << "'\n";
    }
    if (verbosity >= 1)
    {
        std::cout << base << "^" << exp << " == ";
    }

    std::cout << answer << '\n';
}
```
Output:
```
$ advanced1 4 2
16
$ advanced1 4 2 -v 1
4^2 == 16
$ advanced1 4 2 -v 2
Running 'advanced1'
4^2 == 16
```

### Conflicting options

So far we have been using two member functions of `argparse::ArgumentParser` class. Let's introduce a third one, `add_mutually_exclusive_group()`. It allows us to specify options that conflict with each other. Let's also change the rest of the program so that the new functionality makes more sense: we'll introduce the `--quiet` option, which will be the opposite of the `--verbose` one:
```c++
#include "argparse.h"
#include <iostream>
#include <cmath>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser();
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-v", "--verbose").action(argparse::store_true);
    group.add_argument("-q", "--quiet").action(argparse::store_true);
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    if (parsed.get_value<bool>("quiet"))
    {
        std::cout << answer << '\n';
    }
    else if (parsed.get_value<bool>("verbose"))
    {
        std::cout << base << " to the power " << exp << " equals " << answer << '\n';
    }
    else
    {
        std::cout << base << "^" << exp << " == " << answer << '\n';
    }
}
```
Our program is now simpler, and we've lost some functionality for the sake of demonstration. Anyways, here's the output:
```
$ conflicting 4 2
4^2 == 16
$ conflicting 4 2 -q
16
$ conflicting 4 2 -v
4 to the power 2 equals 16
$ conflicting 4 2 -q -v
argument -q/--quiet: not allowed with argument -v/--verbose
usage: conflicting [-h] [-v | -q] x y

positional arguments:
  x                     the base
  y                     the exponent

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose
  -q, --quiet
```
That should be easy to follow. Note that slight difference in the usage text. Note the `[-v | -q]`, which tells us that we can either use `-v` or `-q`, but not both at the same time.

Before we end this part, you probably want to tell your users the main purpose of your program, just in case they don't know:
```c++
#include "argparse.h"
#include <iostream>
#include <cmath>

int main(int argc, char * argv[])
{
    auto parser = argparse::ArgumentParser().description("calculate X to the power of Y");
    auto group = parser.add_mutually_exclusive_group();
    group.add_argument("-v", "--verbose").action(argparse::store_true);
    group.add_argument("-q", "--quiet").action(argparse::store_true);
    parser.add_argument("x").type<int>().help("the base");
    parser.add_argument("y").type<int>().help("the exponent");
    auto parsed = parser.parse_args(argc, argv);
    auto base = parsed.get_value<int>("x");
    auto exp = parsed.get_value<int>("y");
    auto answer = std::pow(base, exp);
    if (parsed.get_value<bool>("quiet"))
    {
        std::cout << answer << '\n';
    }
    else if (parsed.get_value<bool>("verbose"))
    {
        std::cout << base << " to the power " << exp << " equals " << answer << '\n';
    }
    else
    {
        std::cout << base << "^" << exp << " == " << answer << '\n';
    }
}
```
