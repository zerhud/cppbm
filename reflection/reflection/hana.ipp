/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */

template<auto> struct _object_c{};
template<auto obj> constexpr const auto object_c = _object_c<obj>{};
template<typename t> struct _type_c{ using type = t; };
template<typename t> constexpr auto type_c = _type_c<t>{};

template<typename...> struct type_list {};
template<typename... left, typename... right>
constexpr bool operator == (type_list<left...>, type_list<right...>) { return false; }
template<typename... list>
constexpr bool operator == (type_list<list...>, type_list<list...>) { return true; }
template<typename... left, typename... right>
constexpr bool operator != (type_list<left...> l, type_list<right...> r) { return !(l==r); }
template<typename t> constexpr bool operator == (_type_c<t>, _type_c<t>) { return true; }
template<typename t, typename r> constexpr bool operator == (_type_c<t>, _type_c<r>) { return false; }
template<typename t, typename r> constexpr bool operator != (_type_c<t> ll, _type_c<r> rr) { return !(ll==rr); }
template<typename... left, typename... right>
constexpr auto operator + (type_list<left...>, type_list<right...>) { return type_list<left..., right...>{}; }

template<typename type, typename... list>
constexpr auto push_back(type_list<list...>) { return type_list<list..., type>{}; }
template<typename type, typename... list>
constexpr auto push_back(type_list<list...>, _type_c<type>) { return type_list<list..., type>{}; }
template<typename... left, typename... right>
constexpr auto push_back(type_list<left...>, type_list<right...>) { return type_list<left..., right...>{}; }
template<typename type, typename... list>
constexpr auto push_front(type_list<list...>) { return type_list<type, list...>{}; }
template<typename type, typename... list>
constexpr auto push_front(type_list<list...>, _type_c<type>) { return type_list<type, list...>{}; }

template<typename type, typename... list>
constexpr bool contains(type_list<list...>) { return ((type_c<type> == type_c<list>) + ... ); }
template<typename type, typename... list>
constexpr bool contains(_type_c<type> t, type_list<list...>) { return ((t == type_c<list>) + ...);}

template<typename type, typename... list>
constexpr auto push_front_if_not_contains(type_list<list...> l) {
	if constexpr (contains<type>(type_list<list...>{})) return l;
	else return push_front<type>(l);
}
template<typename type, typename... list>
constexpr auto push_front_if_not_contains(_type_c<type>, type_list<list...> l) {
	return  push_front_if_not_contains<type>(l);
}
template<typename type, typename... list>
constexpr auto push_back_if_not_contains(type_list<list...> l) {
	if constexpr (contains<type>(type_list<list...>{})) return l;
	else return push_back<type>(l);
}
template<typename type, typename... list>
constexpr auto push_back_if_not_contains(_type_c<type>, type_list<list...> l) {
	return push_back_if_not_contains<type>(l);
}

constexpr auto transform_uniq(type_list<> l) { return l; }
template<typename first,typename... list>
constexpr auto transform_uniq(type_list<first,list...> l) {
	if constexpr(sizeof...(list)==0) return l;
	else return push_front_if_not_contains(type_c<first>, transform_uniq(type_list<list...>{}));
}

template<typename initial>
constexpr auto fold(type_list<> v, initial, auto&&) { return v; }
template<typename first, typename... list, typename initial>
constexpr auto fold(type_list<first, list...>, initial init, auto&& fnc) {
	if constexpr (sizeof...(list)==0) return fnc(init, type_c<first>);
	else return fold(type_list<list...>{}, fnc(init, type_c<first>), fnc);
}

template<auto ind> constexpr auto nth(type_list<>) {
	static_assert( false, "the index is out of range or try to get element from empty list" );
}
template<auto ind, typename first, typename... types> constexpr auto nth(type_list<first, types...>) {
	if constexpr (ind==0) return type_c<first>;
	else return nth<ind-1>(type_list<types...>{});
}
template<auto ind, typename... types> constexpr auto nth() { return nth<ind>(type_list<types...>{}); }

template<typename type, typename... types> constexpr auto index_of(type_list<types...> list) {
	if constexpr (!contains<type>(list)) return -1;
	else {
		auto ret = 0;
		(void)( (++ret,type_c<type> == type_c<types>) || ... );
		return ret-1;
	}
}
template<typename type, typename... types> constexpr auto index_of() { return index_of<type>(type_list<types...>{}); }

template<template<typename...>class holder, typename... types>
constexpr auto create_list(holder<types...>&&) { return type_list<types...>{}; }
template<template<typename...>class holder, typename... types>
constexpr bool operator==(holder<types...>&& l, type_list<types...> r) { return true; }
template<template<typename...>class holder, typename... types, typename... rtypes>
constexpr bool operator==(holder<types...>&& l, type_list<rtypes...> r) { return false; }

