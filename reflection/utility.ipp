#pragma once

/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */

template<typename t>
struct _decay {
	template<typename type> static const type& lref() ;
	template<typename u> struct _t{ using type = u; };
	template<typename u> static _t<u> impl(const u&) ;
	template<typename u, unsigned int sz> static _t<u*> impl(const u(&)[sz]) ;
	template<typename r, typename... p> static _t<r(*)(p...)> impl(r(&)(p...)) ;
	using type = typename decltype(impl(lref<t>()))::type;
};

template<typename t> using decay_t = typename _decay<t>::type;

template<typename t, t v>
struct integral_constant {
	static constexpr t value = v;
	using value_type = t;
	using type = integral_constant;
	constexpr operator value_type() const noexcept { return value; }
	constexpr value_type operator()() const noexcept { return value; }
};
struct true_type : integral_constant<bool, true> {}; 
struct false_type : integral_constant<bool, false> {}; 
template<typename t> struct is_rvalue_reference : false_type {};
template<typename t> struct is_rvalue_reference<t&&> : true_type {};
template<typename t> struct is_lvalue_reference : false_type {};
template<typename t> struct is_lvalue_reference<t&> : true_type {};
template<typename t> struct is_const : false_type {};
template<typename t> struct is_const<const t> : true_type {};
template<typename t> inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<t>::value;
template<typename t> inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<t>::value;
template<typename t> inline constexpr bool is_const_v = is_const<t>::value;

template<class t> struct remove_reference { typedef t type; };
template<class t> struct remove_reference<t&> { typedef t type; };
template<class t> struct remove_reference<t&&> { typedef t type; };
template< class t > using remove_reference_t = typename remove_reference<t>::type;

template<class t, class u>
constexpr auto&& forward_like(u&& x) noexcept
{
	constexpr bool is_adding_const = is_const_v<remove_reference_t<t>>;
	if constexpr (is_rvalue_reference_v<t&&>) {
		if constexpr (is_adding_const) return static_cast<const decay_t<u>&&>(x);
		else return static_cast<decay_t<u>&&>(x);
	}
	else {
		if constexpr (is_adding_const) return static_cast<const u&>(x);
		else return static_cast<u&>(x);
	}
}

