#ifndef GM_ASSERTS_HH
#define GM_ASSERTS_HH


#include <cassert>

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


#endif GM_ASSERTS_HH