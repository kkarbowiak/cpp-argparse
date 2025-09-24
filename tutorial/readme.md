# C++ argparse Tutorial

This tutorial will get you familiar with this library and will show you, how you can easily get something useful from the `argc` `argv` duo that the runtime delivers to your `main` function.

This document is based on the tutorial for the Python's argparse library.

## The basics

Let's start with a very simple example that doesn't do much (`basic.cpp`):
```c++
#include "argparse.hpp"

auto main(int argc, char * argv[]) -> int
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

An example (`positional.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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

Note that while the help message looks nice, it is not as helpful as it could be. For example we see that we got `echo` as a positional argument, but we don't know what it does. Let's make it a bit more useful  (`positional1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
Now, how about doing some math  (`positional2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number");
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value("square");
    std::cout << value * value << '\n';
}
```
Well, this won't compile and your compiler will complain that the `std::string` type does not define a binary `*` operator. As I mentioned, the default type for all options is `std::string`. Let's tell the parser to treat the `square` option as an integer  (`positional3.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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

So far we have been playing with positional arguments. Let's see how to add optional ones (`optional.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
 * To show that the option is actually optional, there is no error when running the program without it. Note that by default, if an optional argument isn't used, the object holding the relevant value (the one retrieved using `parsed.get("verbosity")`) is empty and in a boolean context (in an `if` statement) yields `false`.
 * The help message is a bit different.
 * When using the `--verbosity` option, one must specify some arbitrary value.

The above example accepts arbitrary values for `--verbosity`, but for our simple program, only two values are actually useful, `true` and `false`. Let's modify the code accordingly (`optional1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
 * The option is now more of a flag than something that requires a value. We even changed the name of the option to match that idea. Note that we now used a new function, `action`, and passed it the value `argparse::store_true`. This means that, if the option is specified, assign the value `true` to the relevant object. Not specifying it implies `false`.
 * We now need to retrieve the value using `parsed.get_value<bool>()` as we now expect the value of type `bool` instead of the usual `std::string`.
 * The option now complains when you specify a value, in true spirit of what flags actually are.
 * Notice the different help text.

### Short options

If you are familiar with command line usage, you will notice that I haven't touched on the topic of short versions of the options. It's quite simple (`optional3.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
$ optional3 -v
verbosity turned on
$ optional3 --help
usage: optional3 [-h] [-v]

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         increase output verbosity
```
Note that the new ability is also reflected in the help text.

## Combining Positional and Optional arguments

Our program keeps growing in complexity (`complex1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
$ complex1
the following arguments are required: square
usage: complex1 [-h] [-v] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose         increase output verbosity
$ complex1 4
16
$ complex1 4 --verbose
the square of 4 equals 16
$ complex1 --verbose 4
the square of 4 equals 16
```
 * We have brought back a positional argument, hence the complaint.
 * Note that the order does not matter.

How about we give this program of ours back the ability to have multiple verbosity values, and actually get to use them (`complex2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
$ complex2 4
16
$ complex 4 -v
argument -v/--verbosity: expected one argument
usage: complex2 [-h] [-v VERBOSITY] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v VERBOSITY, --verbosity VERBOSITY
                        increase output verbosity
$ complex2 4 -v 1
4^2 == 16
$ complex2 4 -v 2
the square of 4 equals 16
$ complex2 4 -v 3
16
```
The above outputs all look good except the last one, which exposes a bug in our program. Let's fix it by restricting the values the `--verbosity` option can accept (`complex3.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
$ complex3 -v 3
argument -v/--verbosity: invalid choice: 3 (choose from 0, 1, 2)
usage: complex3 [-h] [-v {0,1,2}] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v {0,1,2}, --verbosity {0,1,2}
                        increase output verbosity
```
Note that the change reflects in both the error message and the help string.

Let us now try out a different approach of playing with verbosity, which is pretty common (`count1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").action(argparse::count);
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
We have introduced another action, “count”, to count the number of occurrences of specific options.

And the output:
```
$ count1 4
16
$ count1 4 -v
4^2 == 16
$ count1 4 -vv
the square of 4 equals 16
$ count1 4 --verbosity --verbosity
the square of 4 equals 16
$ count1 4 -v 1
unrecognised arguments: 1
usage: count1 [-h] [-v] square

positional arguments:
  square                display a square of a given number

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbosity       increase output verbosity
$ count1 4 -vvv
16
```
 * The option is more of a flag now (similar to `action(argparse::store_true)` in the previous version of the program). This is where the complaint comes from.
 * It also behaves similar to `store_true` action.
 * I think you know what the `count` action does and have seen this sort of usage before.
 * If you don't specify the `-v` flag, the corresponding argument has no value.
 * As should be expected, specifying the long form of the flag, we should get the same output.
 * Sadly, the help output isn't very informative on the new ability of the program, but this can always be fixed by improving the documentation of the program or option's help message.
 * The last output exposes a bug in our program.

Let's fix it (`count2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").action(argparse::count);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity >= 1)
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
$ count2 4 -vvv
the square of 4 equals 16
$ count2 4 -vvvv
the square of 4 equals 16
$ count2 4
terminate called after throwing an instance of 'argparse::type_error'
```
 * First output went well. The second illustrates we fixed the bug we had before. Now, any value >= 2 is as verbose as possible.
 * The third execution crashed. Yikes!

The crash is due to an unhandled exception. Let's add a `try/catch` block to see what happens (`count3.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    try
    {
        auto parser = argparse::ArgumentParser();
        parser.add_argument("square").help("display a square of a given number").type<int>();
        parser.add_argument("-v", "--verbosity").help("increase output verbosity").action(argparse::count);
        auto parsed = parser.parse_args(argc, argv);
        auto value = parsed.get_value<int>("square");
        auto answer = value * value;
        auto verbosity = parsed.get_value<int>("verbosity");
        if (verbosity >= 2)
        {
            std::cout << "the square of " << value << " equals " << answer << '\n';
        }
        else if (verbosity >= 1)
        {
            std::cout << value << "^2 == " << answer << '\n';
        }
        else
        {
            std::cout << answer << '\n';
        }
    }
    catch (argparse::type_error const & e)
    {
        std::cout << "error: " << e.what() << '\n';
    }
}
```
The output:
```
$ count3 4
error: wrong type: requested 'i' for argument of type 'v'
```
Let me first note that the exact message depends on the standard library implementation. The above is generated by libstdc++ which is shipped with GCC. If you are using Visual Studio, you will likely see this:
`error: wrong type: requested 'int' for argument of type 'void'`.

But what does it really mean? It means that by calling `verbosity.get<int>()` we request to get a value of `int` type, but we did not add the `-v` flag, so there is no value stored.

In case you request an incorrect type, or mistype the name of argument you want to get the value of (e.g. `parsed.get_value("bat")` instead of `parsed.get_value("cat")`), the library will throw `argparse::type_error` or `argparse::name_error`, respectively. Both classes derive from `std::logic_error` to indicate this is likely a programmer error that should be fixed, rather than a runtime condition.

Ok, let's fix it (`count4.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("square").help("display a square of a given number").type<int>();
    parser.add_argument("-v", "--verbosity").help("increase output verbosity").action(argparse::count).default_(0);
    auto parsed = parser.parse_args(argc, argv);
    auto value = parsed.get_value<int>("square");
    auto answer = value * value;
    auto verbosity = parsed.get_value<int>("verbosity");
    if (verbosity >= 2)
    {
        std::cout << "the square of " << value << " equals " << answer << '\n';
    }
    else if (verbosity >= 1)
    {
        std::cout << value << "^2 == " << answer << '\n';
    }
    else
    {
        std::cout << answer << '\n';
    }
}
```
We have introduced default value of 0. When an option is not specified, it gets its default value, if set. This way we can do the comparison safely even in the absence of the flag.

And the output:
```
$ count4 4 -vvvv
the square of 4 equals 16
$ count4 4 -vvv
the square of 4 equals 16
$ count4 4 -vv
the square of 4 equals 16
$ count4 4 -v
4^2 == 16
$ count4 4
16
```

### Default values

One way to remove the need of getting the value object and doing a boolean test before extracting the value is to give the option a default value (`complex4.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
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
$ complex4 4
16
$ complex4 4 --verbosity 1
4^2 == 16
$ complex4 4 --verbosity 2
the square of 4 equals 16
```
As expected, if the argument is not specified, it assumes its default value.

## Getting a little more advanced

What if we wanted to expand our tiny program to perform other powers, not just squares (`advanced1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
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
$ advanced1
the following arguments are required: x y
usage: advanced1 [-h] [-v VERBOSITY] x y

positional arguments:
  x                     the base
  y                     the exponent

optional arguments:
  -h, --help            show this help message and exit
  -v VERBOSITY, --verbosity VERBOSITY
$ advanced1 4 2 -v 1
4^2 == 16
```
Notice that so far we've been using verbosity to *change* the text that gets displayed. The following example instead uses verbosity to display *more* text instead (`advanced2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
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
$ advanced2 4 2
16
$ advanced2 4 2 -v 1
4^2 == 16
$ advanced2 4 2 -v 2
Running 'advanced2'
4^2 == 16
```

### Conflicting options

So far we have been using two member functions of `argparse::ArgumentParser` class. Let's introduce a third one, `add_mutually_exclusive_group()`. It allows us to specify options that conflict with each other. Let's also change the rest of the program so that the new functionality makes more sense: we'll introduce the `--quiet` option, which will be the opposite of the `--verbose` one (`conflicting1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
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
$ conflicting1 4 2
4^2 == 16
$ conflicting1 4 2 -q
16
$ conflicting1 4 2 -v
4 to the power 2 equals 16
$ conflicting1 4 2 -q -v
argument -q/--quiet: not allowed with argument -v/--verbose
usage: conflicting1 [-h] [-v | -q] x y

positional arguments:
  x                     the base
  y                     the exponent

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose
  -q, --quiet
```
That should be easy to follow. Note that slight difference in the usage text. Note the `[-v | -q]`, which tells us that we can either use `-v` or `-q`, but not both at the same time.

Before we end this part, you probably want to tell your users the main purpose of your program, just in case they don't know (`conflicting2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>
#include <cmath>

auto main(int argc, char * argv[]) -> int
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

This results in additional information in help message:
```
$ conflicting2 --help
usage: conflicting2 [-h] [-v | -q] x y

calculate X to the power of Y

positional arguments:
  x                     the base
  y                     the exponent

optional arguments:
  -h, --help            show this help message and exit
  -v, --verbose
  -q, --quiet
```

## Other important considerations

Before we end this tutorial, I would like to mention some more topics more related to this implementation rather than to the Python's version.

### Pay attention to types

When you want to use default values, choices, or const values in conjunction with types other that `std::string`, please remember to also specify the argument type and then retrieve this same exact type (`types.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("pos").type<int>().choices({1, 2, 3});
    parser.add_argument("-c").type<double>().const_(3.14).action(argparse::store_const);
    parser.add_argument("-d").type<int>().default_(7);
    auto parsed = parser.parse_args(argc, argv);
    std::cout << "pos:\t" << parsed.get_value<int>("pos") << '\n';
    std::cout << "c:\t" << (parsed.get("c") ? parsed.get_value<double>("c") : 0) << '\n';
    std::cout << "d:\t" << parsed.get_value<int>("d") << '\n';
}
```
Which outputs:
```
$ types
the following arguments are required: pos
usage: types [-h] [-c] [-d D] {1,2,3}

positional arguments:
  {1,2,3}

optional arguments:
  -h, --help            show this help message and exit
  -c
  -d D
$ types 2
pos:    2
c:      0
d:      7
$ types 2 -c
pos:    2
c:      3.14
d:      7
$ types 2 -d 21
pos:    2
c:      0
d:      21
```

When using `std::string`, **beware not to use** `const char*` by accident. The easiest and most convenient way to avoid this is to use the `""s` string literal (`string.cpp`):
```c++
#include "argparse.hpp"
#include <string>
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    using namespace std::string_literals;

    auto parser = argparse::ArgumentParser();
    parser.add_argument("name").type<std::string>().choices({"John"s, "Lukas"s, "Gregory"s});
    auto parsed = parser.parse_args(argc, argv);
    std::cout << "Hello, " << parsed.get_value("name") << "!\n";
}
```
And the output:
```
$ string
the following arguments are required: name
usage: string [-h] {"John","Lukas","Gregory"}

positional arguments:
  {"John","Lukas","Gregory"}

optional arguments:
  -h, --help            show this help message and exit
$ string Lukas
Hello, Lukas!
```
The library could act smart here and automatically convert the `const char*` types to `std::string`s, but (at least for now) I decided not to.

### Using custom types

You may wonder whether this library allows using types other than the built-in ones (`int`, `float`, `double`, etc.) or `std::string`. Actually, yes, it does!

You can parse directly to any custom type, provided that this type is default-constructible and you provide a way to do string-type and type-string conversion as well as equality comparison. This may sound complicated, but basically boils down to specialising three template functions: `argparse::from_string`, `argparse::to_string`, and `argparse::are_equal`, which are the library's customisation points. Let's have a look at an example (`custom1.cpp`):
```c++
#include "argparse.hpp"
#include <string>
#include <sstream>
#include <cmath>

namespace geometry
{
    struct Point
    {
        Point() = default;

        Point(int xx, int yy) : x(xx), y(yy)
        {
        }

        int x = 0;
        int y = 0;
    };
}

namespace argparse
{
template<>
inline auto from_string(std::string const & s, geometry::Point & p) -> bool
{
    std::istringstream iss(s);
    char comma;
    iss >> p.x >> comma >> p.y;
    return true;
}

template<>
inline auto to_string(geometry::Point const & p) -> std::string
{
    return std::to_string(p.x) + "," + std::to_string(p.y);
}

template<>
inline auto are_equal(geometry::Point const & l, geometry::Point const & r) -> bool
{
    return l.x == r.x && l.y == r.y;
}
}

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("start").type<geometry::Point>();
    parser.add_argument("end").type<geometry::Point>();
    auto parsed = parser.parse_args(argc, argv);
    auto start = parsed.get_value<geometry::Point>("start");
    auto end = parsed.get_value<geometry::Point>("end");
    auto distance = std::hypot(end.x - start.x, end.y - start.y);
    std::cout << "The distance is " << distance << '\n';
}
```
And the output:
```
$ custom1
the following arguments are required: start end
usage: custom1 [-h] start end

positional arguments:
  start
  end

optional arguments:
  -h, --help            show this help message and exit
$ custom1 0,0 1,1
The distance is 1.41421
```
The return value of `argparse::from_string` indicates whether the conversion succeeded. You can use it to your advantage (`custom2.cpp`):
```c++
#include "argparse.hpp"
#include <string>
#include <sstream>
#include <cmath>

namespace geometry
{
    struct Point
    {
        Point() = default;

        Point(int xx, int yy) : x(xx), y(yy)
        {
        }

        int x = 0;
        int y = 0;
    };
}

namespace argparse
{
template<>
inline auto from_string(std::string const & s, geometry::Point & p) -> bool
{
    std::istringstream iss(s);
    char comma;
    iss >> p.x >> comma >> p.y;
    return !iss.fail();
}

template<>
inline auto to_string(geometry::Point const & p) -> std::string
{
    return std::to_string(p.x) + "," + std::to_string(p.y);
}

template<>
inline auto are_equal(geometry::Point const & l, geometry::Point const & r) -> bool
{
    return l.x == r.x && l.y == r.y;
}
}

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("start").type<geometry::Point>();
    parser.add_argument("end").type<geometry::Point>();
    auto parsed = parser.parse_args(argc, argv);
    auto start = parsed.get_value<geometry::Point>("start");
    auto end = parsed.get_value<geometry::Point>("end");
    auto distance = std::hypot(end.x - start.x, end.y - start.y);
    std::cout << "The distance is " << distance << '\n';
}
```
Let's see how it works:
```
$ custom2 0,0 -1,-1
The distance is 1.41421
$ custom2 foo bar
argument start: invalid value: 'foo'
usage: custom2 [-h] start end

positional arguments:
  start
  end

optional arguments:
  -h, --help            show this help message and exit
```
Of course it would be even better to let the user know what is the proper format of the arguments instead of having them guess. This can be done via arguments' help messages, but is left here as an excercise for the readers.

At this point you may wonder what the type-string conversion and comparison are needed for. They become important when you want to use choices (`custom3.cpp`):
```c++
#include "argparse.hpp"
#include <string>
#include <sstream>
#include <cmath>

namespace geometry
{
    struct Point
    {
        Point() = default;

        Point(int xx, int yy) : x(xx), y(yy)
        {
        }

        int x = 0;
        int y = 0;
    };
}

namespace argparse
{
template<>
inline auto from_string(std::string const & s, geometry::Point & p) -> bool
{
    std::istringstream iss(s);
    char comma;
    iss >> p.x >> comma >> p.y;
    return !iss.fail();
}

template<>
inline auto to_string(geometry::Point const & p) -> std::string
{
    return std::to_string(p.x) + "," + std::to_string(p.y);
}

template<>
inline auto are_equal(geometry::Point const & l, geometry::Point const & r) -> bool
{
    return l.x == r.x && l.y == r.y;
}
}

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("start").type<geometry::Point>().choices({geometry::Point(0, 0), geometry::Point(1, 1), geometry::Point(2, 2)});
    parser.add_argument("end").type<geometry::Point>();
    auto parsed = parser.parse_args(argc, argv);
    auto start = parsed.get_value<geometry::Point>("start");
    auto end = parsed.get_value<geometry::Point>("end");
    auto distance = std::hypot(end.x - start.x, end.y - start.y);
    std::cout << "The distance is " << distance << '\n';
}
```
And the output:
```
$ custom3 0,0 9,9
The distance is 12.7279
$ custom3 1,0 9,9
argument start: invalid choice: 1,0 (choose from 0,0, 1,1, 2,2)
usage: custom3 [-h] {0,0,1,1,2,2} end

positional arguments:
  {0,0,1,1,2,2}
  end

optional arguments:
  -h, --help            show this help message and exit
```

### Help, version, and error handling

As you have noticed, the parser automatically adds the `-h/--help` optional argument and handles help requests and parsing errors. There are some cases, when this may be undesirable.

You can disable adding the `--help` option like this (`nohelp.cpp`):
```c++
#include "argparse.hpp"

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser().add_help(false);
    parser.parse_args(argc, argv);
}
```
Output:
```
$ nohelp --help
unrecognised arguments: --help
usage: nohelp
```
The automatic help, version, and error handling consists of printing a relevant message and exiting the program by a call to `std::exit`. The reason this may be undesirable is that `std::exit` does not ensure cleanup of local variables (`undesired.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

class Logger
{
    public:
        Logger()
        {
            std::cout << "Log started\n";
        }

        ~Logger()
        {
            std::cout << "Log ended\n";
        }
};

auto main(int argc, char * argv[]) -> int
{
    Logger logger;
    auto parser = argparse::ArgumentParser();
    parser.parse_args(argc, argv);
}
```
As you will notice, the logger's destructor is not called:
```
$ undesired
Log started
Log ended
$ undesired --help
Log started
usage: undesired [-h]

optional arguments:
  -h, --help            show this help message and exit
$ undesired foo
Log started
unrecognised arguments: foo
usage: undesired [-h]

optional arguments:
  -h, --help            show this help message and exit
```
To improve this situation, you may tell the logger not to handle help, version, and errors (`nohandling1.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

class Logger
{
    public:
        Logger()
        {
            std::cout << "Log started\n";
        }

        ~Logger()
        {
            std::cout << "Log ended\n";
        }
};

auto main(int argc, char * argv[]) -> int
{
    Logger logger;
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    parser.parse_args(argc, argv);
}
```
In such a case, you'll most likely want to handle help and version requests yourself. Also, you will now **need** to handle errors, otherwise you will encounter unhandled exceptions. Let's extend the program (`nohandling2.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

class Logger
{
    public:
        Logger()
        {
            std::cout << "Log started\n";
        }

        ~Logger()
        {
            std::cout << "Log ended\n";
        }
};

auto main(int argc, char * argv[]) -> int
{
    Logger logger;
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::none);
    try
    {
        auto parsed = parser.parse_args(argc, argv);
        if (parsed.get_value<bool>("help"))
        {
            std::cout << parser.format_help() << '\n';
            return 0;
        }
    }
    catch (argparse::parsing_error const & e)
    {
        std::cout << e.what() << '\n';
        return 1;
    }
}
```
This version outputs:
```
$ nohandling2
Log started
Log ended
$ nohandling2 --help
Log started
usage: nohandling2 [-h]

optional arguments:
  -h, --help            show this help message and exit
Log ended
$ nohandling2 foo
Log started
unrecognised arguments: foo
Log ended
```
The following example illustrates handling version requests (`nohandling3.cpp`):
```c++
#include "argparse.hpp"
#include <iostream>

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser().handle(argparse::Handle::errors_and_help);
    parser.add_argument("--version").action(argparse::version);
    auto parsed = parser.parse_args(argc, argv);
    if (parsed.get_value<bool>("version"))
    {
        std::cout << "This is program version 1.0.0\n";
    }
    return 0;
}
```
Its automatically generated help message looks like this:
```
$ nohandling3 --help
usage: nohandling3 [-h] [--version]

optional arguments:
  -h, --help            show this help message and exit
  --version             show program's version number and exit
```
Let's see how it prints version:
```
$ nohandling3 --version
This is program version 1.0.0
```
Of course, handling version requests can be fully automatic (`version.cpp`):
```c++
#include "argparse.hpp"

auto main(int argc, char * argv[]) -> int
{
    auto parser = argparse::ArgumentParser();
    parser.add_argument("--version").action(argparse::version).version("1.0.0-rc1");
    parser.parse_args(argc, argv);
}
```
Let's see how it works:
```
$ version --version
1.0.0-rc1
```

Some things to note here:
 * You can use parser's `format_help`, `format_usage`, and `format_version` functions to generate the messages for you.
 * Before using `format_version` be sure to add an argument with `version` action and set the version.
 * In case of error, parser throws an exception of type `argparse::parsing_error` (or type derived from it), which derives from `std::runtime_error`.
 * The control is more granular; you can tell the parser to handle:
   * only help (`argparse::Handle::help`),
   * only version (`argparse::Handle::version`),
   * only errors (`argparse::Handle::errors`),
   * both help and errors (`argparse::Handle::errors_and_help`),
   * any combination just like with bit masks (`argparse::Handle::errors | argparse::Handle::help`),
   * help, version, and errors (`argparse::Handle::errors_help_version`) which is the default.

## Conclusion

This ends the tutorial. I hope the information here is useful and can get you started using the library!
