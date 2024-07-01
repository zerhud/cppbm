/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */

namespace details {

struct true_type { constexpr static bool value = true; };
struct fail_type {};

template<template<typename...>typename variant, typename... types>
consteval auto is_variant(const variant<types...>& v) {
	constexpr bool contains_types = sizeof...(types)>0;
	constexpr bool has_visit = requires{ visit([](const auto&){}, v); };
	if constexpr(contains_types && has_visit) return true_type{};
	else if constexpr (!contains_types && has_visit && requires{ get<0>(v); }) return true_type{};
	else return fail_type{};
}

template<typename t1, typename t2> struct _is_same { constexpr static bool val=false; };
template<typename t> struct _is_same<t,t>{ constexpr static bool val=true; };
template<typename t> struct _is_ptr { constexpr static bool val=false; };
template<typename t> struct _is_ptr<t*>{ constexpr static bool val=true; };
template<typename t> struct _rm_ptr { using type = t; };
template<typename t> struct _rm_ptr<t*>{ using type = t;};


template<typename type> consteval bool is_ptr() { return _is_ptr<type>::val; }
template<typename type1, typename type2> consteval bool is_same() { return _is_same<type1,type2>::val; }

} // namespace details

template<typename type> concept variant = requires(const type& o){ details::is_variant(o).value; };

template<typename type> concept outer_begin_end = requires(const type& o) { begin(o); end(o); };
template<typename type> concept inner_begin_end = requires(const type& o) { o.begin(); o.end(); };

template<typename type>
concept iterable = outer_begin_end<type> || inner_begin_end<type> ;

template<typename type> concept list = iterable<type> || requires(const type& o){ o.size(); o[0]; };
template<typename type>
concept vector = iterable<type> && requires(const type& o){ o.data(); typename type::value_type; };
template<typename type>
concept smart_ptr = requires(const type& o) {
	typename type::element_type;
	o.get();
	*o;
	!!o;
};
template<typename type> concept any_ptr = smart_ptr<type> || details::is_ptr<type>();
template<typename type, typename to>
concept smart_ptr_to = smart_ptr<type> && details::is_same<typename type::element_type, to>();
template<typename type, typename to>
concept any_ptr_to = smart_ptr_to<type, to> || (details::is_ptr<type>() && details::is_same<typename details::_rm_ptr<type>::type, to>());

