#ifndef CSTRING_ARRAY_H__DDK
#define CSTRING_ARRAY_H__DDK

#include <array>


template<std::size_t N, class... Ts>
struct cstr_arr
{
    explicit cstr_arr(Ts... args)
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
cstr_arr(Ts...) -> cstr_arr<sizeof...(Ts), Ts...>;

#endif /* CSTRING_ARRAY_H__DDK */
