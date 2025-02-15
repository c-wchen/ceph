// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:nil -*-
// vim: ts=8 sw=2 smarttab expandtab

#pragma once

#include <type_traits>

namespace _impl
{
template <class T> struct always_false : std::false_type {};
};

template <class T>
void assert_moveable(T &t)
{
    // It's fine
}
template <class T>
void assert_moveable(const T &t)
{
    static_assert(_impl::always_false<T>::value, "unable to move-out from T");
}

namespace internal
{

template <typename Obj, typename Method, typename ArgTuple, size_t... I>
static auto _apply_method_to_tuple(
    Obj &obj, Method method, ArgTuple &&tuple,
    std::index_sequence<I...>)
{
    return (obj.*method)(std::get<I>(std::forward<ArgTuple>(tuple))...);
}

}

template <typename Obj, typename Method, typename ArgTuple>
auto apply_method_to_tuple(Obj &obj, Method method, ArgTuple &&tuple)
{
    constexpr auto tuple_size = std::tuple_size_v<ArgTuple>;
    return internal::_apply_method_to_tuple(
               obj, method, std::forward<ArgTuple>(tuple),
               std::make_index_sequence<tuple_size>());
}
