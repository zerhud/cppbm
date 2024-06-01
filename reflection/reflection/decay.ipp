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

