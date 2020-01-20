#ifndef PARSE_ARGS_HELPER_H__DDK
#define PARSE_ARGS_HELPER_H__DDK

#include "argparse.h"


template<int N, class... Ts>
struct c_str_arr
{
    c_str_arr(Ts... args)
        : array{args...}
    {
    }

    operator char const * const * () const
    {
        return array.data();
    }

    std::array<char const *, N> array;
};

template<class... Ts>
c_str_arr(Ts...) -> c_str_arr<sizeof...(Ts), Ts...>;

#endif /* PARSE_ARGS_HELPER_H__DDK */
