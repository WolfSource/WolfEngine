#pragma once

#include <utility>

namespace wolf::gst::internal {

/**
 * @brief The famous overloaded pattern to overload several callables, e.g. lambdas.
 *
 * reference: https://www.cppstories.com/2019/02/2lines3featuresoverload.html/
 */
template <typename ...Fs>
struct w_overloaded : Fs... { using Fs::operator()...; };

template <typename ...Fs>
w_overloaded(Fs...) -> w_overloaded<Fs...>;

/**
 * @brief a helper tool used with `overloaded` and `std::visit` to
 *        behave similar to `default` clause in switch/case statements.
 *
 * it is implicitly convertible from anything, but discards them,
 * thus in an overload set, it will be the least match, thus if any other
 * callable wouldn't match, this one will be used.
 *
 * example:
 * ```cpp
 * std::visit(
 *   w_overloaded{
 *     [](int num) { ... case int ... },
 *     [](char* str) { ... case c-string ... },
 *     [](w_blackhole) { ... default ... }
 *   },
 *   my_varaint
 * );
 * ```
 */
struct w_blackhole
{
    template <typename ...Ts>
    constexpr w_blackhole(Ts&& ...) noexcept {}
};

/**
 * @brief make an instantiable callable type out of
 *        a compile-time function pointer.
 *
 * @tparam FV a function symbole/pointer.
 */
template <auto FV>
class w_functor {
public:
    template <typename ...Args>
    auto operator()(Args&& ...p_args) const
    {
        return FV(std::forward<Args>(p_args)...);
    }
};

}  // namespace wolf::gst::internal
