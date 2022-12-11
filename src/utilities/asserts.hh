#ifndef GM_ASSERTS_HH
#define GM_ASSERTS_HH


#include <type_traits>
#include <stdexcept>

template <typename ...Ts>
void assert_pointers(Ts...)
{
    static_assert(std::conjunction_v<std::is_pointer<Ts>...>, "Not all of the provided arguments are pointers.");
}

//TODO: NOW: Testala ancora un po'
template <typename ...Ts>
void assert_nonNullptrs(Ts... args)
{
    assert_pointers(args...);

    if ((... || !args)) { throw std::runtime_error("There is at least a nullptr between them."); }
}



#endif //GM_ASSERTS_HH