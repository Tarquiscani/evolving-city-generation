#ifndef GM_DEBUG_ASSERTS_HH
#define GM_DEBUG_ASSERTS_HH


#include <cassert>
#include <stdexcept>
#include <type_traits>

#include "settings/debug/debug_settings.hh"


#define check(expr) \
{ \
    assert(expr); \
}

#if DYNAMIC_ASSERTS
    // Execute the expression
    #define exec_check(expr) \
    { \
        expr; \
    }
#elif
    // Do nothing
    #define exec_check(expr) do {} while(0)
#endif


namespace tgm
{
    


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



} // namespace tgm


#endif //GM_DEBUG_ASSERTS_HH