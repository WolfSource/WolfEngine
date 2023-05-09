/*
    Project: Wolf Engine. Copyright © 2014-2023 Pooya Eimandar
    https://github.com/WolfEngine/WolfEngine
*/

#pragma once

namespace wolf::system {

/**
 * @brief famous `overloaded` to combine callables (specially lambdas) into one overloaded callable.
 *
 * example:
 * ```cpp
 *  double res = std::visit(
 *      w_overloaded{
 *          [](long long int val) { return val * 2.0; },
 *          [](double val) { return val / 2.0; }
 *      },
 *      my_variant
 *  );
 * ```
 */
template <typename ...Fs>
struct w_overloaded : public Fs... { using Fs::operator()...; };

template <typename ...Fs>
w_overloaded(Fs&& ...) -> w_overloaded<Fs...>;

}  // namespace wolf::system
