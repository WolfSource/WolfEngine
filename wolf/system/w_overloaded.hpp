#pragma once

namespace wolf {

/**
 * @brief famous `overloaded` to combine callables (specially lambdas)
 *        into one overloaded callable.
 *
 * example:
 * ```cpp
 *  double res = std::visit(
 *      overloaded{
 *          [](long long int val) { return val * 2.0; },
 *          [](double val) { return val / 2.0; },
 *          [](const auto&) { return 0.0; }
 *      },
 *      my_variant
 *  );
 * ```
 */
template <typename ...Fs>
struct w_overloaded : public Fs... { using Fs::operator()...; };

template <typename ...Fs>
w_overloaded(Fs&& ...) -> w_overloaded<Fs...>;

/**
 * @brief discards instance of any given argument on
 *        construction as implicit conversion;
 *        useful for unhandled cases in a visit.
 *
 * @note use this type only if `const auto&` doesn't suffice;
 *       for example when you want strict matches instead of
 *       implicit conversions or mistakenly falling down into
 *       default `const auto&` case.
 *
 * @note using `[](auto&&) { ... }` wouldn't be the same as
 *       it would be the best match in an overload set,
 *       unlike `[](blackhole_t) { ... }` which is triggered due implicit conversion.
 *
 * example:
 * ```cpp
 *  double res = std::visit(
 *      overloaded{
 *          [](long long int val) { return val * 2.0; },
 *          [](double val) { return val / 2.0; },
 *          [](w_blackhole) { return 0.0; }  // any other type
 *      },
 *      my_variant
 *  );
 * ```
 */
struct w_blackhole {
    template <typename ...Ts>
    constexpr w_blackhole(Ts&& ...) noexcept { /* nothing */ }
};

}  // namespace wolf
