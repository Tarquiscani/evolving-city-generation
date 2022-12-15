Style Guide
=====

### Naming convention 
* Class and struct -> *CamelCase*:

        class MyClass;

* Functions and methods -> *snake_case*. This changed during the development, I initially used a messy mixture of snake case and camel case. Then I switched to snake_case only.

        auto my_func(int const a) -> int;

* Local variables -> *snake_case*. This changed during the development, I initially used a messy mixture of snake case and camel case. Then I switched to snake_case only.

        auto my_var = 3;

* Enums -> *CamelCase*:

        enum class MyState
        {
            Idle,
            Busy,
        };

* Global variables -> *snake_case*, but prepend *g_*:

        auto constexpr g_my_global_var = 1;

### Const style
I follow the David Vandevoorde's "East const" style. For example:

    int const var;
    int const& ref = var;
    int const* ptr = &var;
    int const*const ptr2 = ptr;

### Auto
I follow Scott Meyer's proposed practice ["almost always auto"](https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/):

    auto my_var = 0;
    auto my_instance = MyClass{ 52 };

### Functions
I always use the trailing return, but I specify return type explicitly. This allows to have all function names aligned to the left:

    auto my_func(int const a) -> int;
    auto my_func2(int const a) -> MyAbnormallyLargeNamespaceName::MyAbnormallyLargeClassName;

If I have a long function I align the second line to the start of the parenthesis:

    auto my_func(int const my_long_argument_1, int const my_long_argument_2
                 int const my_long_argument_3)
        -> MyReturnType

### Classes
I indent *public* and *private* keywords:

    class MyClass
    {
        private:
            int m_member{ 0 };
        public:
            auto const member() { return m_member; }
    }

### Brace placement
I follow the Allman convention:

    while (x == y)
    {
        do_something();
    }

### Indentation
I use 4 spaces for indentation. I prefer spaces because they are rendered consistently across applications (including Github).
Also they makes the alignment of function arguments easier.

### Single statement if block
I agree with the [general opinion](https://softwareengineering.stackexchange.com/questions/16528/single-statement-if-block-braces-or-no) that is a good practice to always add curly braces. Either:

    if (condition)
    {
        my_func();
    }

or:

    if (condition) { my_func(); }