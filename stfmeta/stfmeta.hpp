/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */
#pragma once

#ifdef __clang__
#define STFMETA_ADD_TMPL_PARAM
#define STFMETA_USE_TMPL_PARAM
#else
#define STFMETA_ADD_TMPL_PARAM , auto
#define STFMETA_USE_TMPL_PARAM , 1
#endif

namespace stfmeta {

struct not_exists_tag{};

template<typename... types> struct type_list{};
template<typename t> struct _type_c {using type = t;};
template<typename t> constexpr auto type_c = _type_c<t>{};

template<typename l, typename r>
constexpr bool operator==(const _type_c<l>, const _type_c<r>) { return false; }
template<typename s>
constexpr bool operator==(const _type_c<s>, const _type_c<s>) { return true; }

template<typename... l, typename... r>
constexpr bool operator==(const type_list<l...>, const type_list<r...>) { return false; }
template<typename... s>
constexpr bool operator==(const type_list<s...>, const type_list<s...>) { return true; }

template<typename... types>
constexpr void for_each(type_list<types...>, auto&& fnc) {
	[[maybe_unused]] auto i=(0 + ... + (fnc(type_c<types>), 1));
}

template<typename... list, typename item>
constexpr auto append(type_list<list...>, _type_c<item>) {
	return type_list<list..., item>{};
}

consteval auto filter(type_list<> result, auto&& fnc) {
	return result;
}
template<typename fnc_type>
consteval auto filter(auto result, type_list<>, const fnc_type& fnc) {
	return result;
}
template<typename first, typename... tail, typename fnc_type>
consteval auto filter(auto result, type_list<first, tail...>, const fnc_type& fnc) {
	if constexpr (fnc_type{}(type_c<first>))
		return filter(append(result, type_c<first>), type_list<tail...>{}, fnc);
	else return filter(result, type_list<tail...>{}, fnc);
}

template<typename first, typename... tail, typename fnc_type>
consteval auto filter(type_list<first, tail...>, const fnc_type& fnc) {
	return filter(type_list<>{}, type_list<tail...>{}, fnc);
}


template<typename type, template<typename...>class tmpl> constexpr const bool is_specialization_of = false;
template<template<typename...>class type, typename... args> constexpr const bool is_specialization_of<type<args...>, type> = true;

using size_type = decltype(sizeof(int));

template<typename tag, size_type n> struct mark {
	friend constexpr auto ctx_check(mark<tag, n> STFMETA_ADD_TMPL_PARAM);
};
template<typename tag, typename payload, size_type n> struct create {
	friend constexpr auto ctx_check(mark<tag, n> STFMETA_ADD_TMPL_PARAM){return type_c<payload>;}
};

template<typename tag, auto utag, auto cur_val = 0>
consteval auto counter_impl(auto v) {
	constexpr bool already_has = requires(mark<tag, cur_val> r) { ctx_check(r STFMETA_USE_TMPL_PARAM); };
	if constexpr (already_has) return counter_impl<tag, utag, cur_val+1>(v);
	else {
		[[maybe_unused]] auto c = create<tag, typename decltype(v)::type, cur_val>{};
		return cur_val;
	}
}
template<typename tag, typename cnt = void, auto u=[]{}, auto val = counter_impl<tag, u>(type_c<cnt>)>
constexpr auto counter = val;

template<typename tag, auto utag, auto cur_val = 0>
consteval auto counter_val_impl(auto v) {
	constexpr bool already_has = requires(mark<tag, cur_val> m) { ctx_check(m STFMETA_USE_TMPL_PARAM); };
	if constexpr (already_has) return counter_val_impl<tag, utag, cur_val+1>(v);
	else return cur_val-1;
}

template<typename tag, auto u=[]{}, auto val = counter_val_impl<tag, u>(2)>
constexpr auto counter_val = val;

template<typename tag, auto pos=counter_val<tag>>
consteval auto counter_content(auto v) {
	constexpr bool exists = requires(mark<tag, pos> m){ctx_check(m STFMETA_USE_TMPL_PARAM);};
	static_assert( exists, "the required value dosen't exists" );
	if constexpr (!exists) return type_c<not_exists_tag>;
	else return ctx_check(mark<tag, pos>{} STFMETA_USE_TMPL_PARAM);
}
template<typename tag, auto pos=counter_val<tag>, auto u=[]{}>
constexpr auto counter_cnt = counter_content<tag, pos>(1);

template<typename tag, template<typename>class type_holder, typename type>
constexpr auto push_back(type_holder<type>) {
	return counter<tag, type> + 1;
}
template<typename tag, typename type>
constexpr auto push_back() {
	return push_back<tag>(type_c<type>);
}

template<typename tag, auto pos=counter_val<tag>, typename... list>
constexpr auto stored_types() {
	if constexpr (pos>0) return stored_types<tag, pos-1, typename decltype(counter_cnt<tag, pos>)::type, list...>();
	else if constexpr (pos==0) return type_list<typename decltype(counter_cnt<tag, pos>)::type, list...>{};
	else return type_list<>{};
}

} // namespace stfmeta
