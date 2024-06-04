/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */
#pragma once

#include <utility>

namespace serp {

template<typename factory, typename type>
constexpr void pack(factory&& io, const type& what) ;

template<typename factory, typename type, typename size_type=decltype(sizeof(int))>
constexpr size_type read(factory&& io, type& to, size_type shift=0) ;

namespace details {

struct cpp17_adl_fail {};
template<auto ind, typename type> cpp17_adl_fail get(const type&);

template<template<typename...>class tuple, typename... types>
constexpr auto size(const tuple<types...>*) { return sizeof...(types); }

template<auto cur_ct_ind, template<typename...>class variant, typename... types, typename factory, typename shift_type, typename ind_type>
constexpr auto ct_var_emplace(variant<types...>& var, factory&& io, shift_type shift, ind_type cur, ind_type ind) {
	if constexpr(cur_ct_ind < sizeof...(types) ) {
		if(cur == ind) return read(io, var.template emplace<cur_ct_ind>(), shift);
		else return ct_var_emplace<cur_ct_ind+1>(var, io, shift, cur+1, ind);
	}
	else return shift;
}

template<typename type>
class cpp17_concepts {
	constexpr static bool _is_adl_get(const void*) { return false; }
	template<typename t> constexpr static auto _is_adl_get(const t* p) -> decltype(get<0>(*p), void(), true) {
		return !std::is_same_v<decltype(get<0>(*p)), cpp17_adl_fail>;
	}

	constexpr static bool _is_size_member(const void*) { return false; }
	template<typename t> constexpr static auto _is_size_member(const t* p) -> decltype(p->size(), void(), true) { return true; }

	constexpr static bool _is_resize_member(const void*) { return false; }
	template<typename t> constexpr static auto _is_resize_member(t* p) -> decltype(p->resize(0), void(), true) { return true; }

	constexpr static bool _is_emplace_member(const void*) { return false; }
	template<typename t> constexpr static auto _is_emplace_member(t* p) -> decltype(p->emplace(), void(), true) { return true; }

	constexpr static bool _is_element_type_member(const void*) { return false; }
	template<typename t> constexpr static auto _is_element_type_member(t*) -> decltype(std::declval<typename t::element_type>(), void(), true) { return true; }

	constexpr static bool _is_get_member_with_ptr(const void*) { return false; }
	template<typename t> constexpr static auto _is_get_member_with_ptr(t* p) -> decltype(p->get(), void(), true) { return std::is_pointer_v<decltype(p->get())>; }

	constexpr static bool _can_dereference(const void*) { return false; }
	template<typename t> constexpr static auto _can_dereference(t* p) -> decltype(*(*p), void(), true) { return true; }

	constexpr static bool _is_index_member(const void*) { return false; }
	template<typename t> constexpr static auto _is_index_member(const t* p) -> decltype(p->index(), void(), true) { return true; }

	constexpr static auto _visit_cpp17_helper = [](const auto&) {};
	constexpr static bool _is_adl_visit(const void*) { return false; }
	template<typename t> constexpr static auto _is_adl_visit(const t* p) -> decltype(visit(_visit_cpp17_helper, *p), void(), true) { return true; }

	constexpr static bool _has_op_bool(const void*) { return false; }
	template<typename t> constexpr static auto _has_op_bool(const t* p) -> decltype(!!(*p), void(), true) { return true; }
	
	constexpr static bool _has_op_star(const void*) { return false; }
	template<typename t> constexpr static auto _has_op_star(const t* p) ->decltype(*(*p), void(), true) { return true; }

	constexpr static bool _has_value_method(const void*) { return false; }
	template<typename t> constexpr static auto _has_value_method(const t* p) ->decltype((*p).value(), void(), true) { return true; }
public:
	constexpr static bool is_adl_get = _is_adl_get(static_cast<const type*>(nullptr));
	constexpr static bool is_size_member = _is_size_member(static_cast<type*>(nullptr));
	constexpr static bool is_resize_member = _is_resize_member(static_cast<type*>(nullptr));
	constexpr static bool is_emplace_member = _is_emplace_member(static_cast<type*>(nullptr));
	constexpr static bool is_element_type_member = _is_element_type_member(static_cast<type*>(nullptr));
	constexpr static bool is_get_member_with_ptr = _is_get_member_with_ptr(static_cast<type*>(nullptr));
	constexpr static bool can_dereference = _can_dereference(static_cast<type*>(nullptr));
	constexpr static bool is_smart_pointer = is_element_type_member && is_get_member_with_ptr && can_dereference;
	constexpr static bool is_pointer = std::is_pointer_v<type> || is_smart_pointer;
	constexpr static bool is_variant = _is_index_member(static_cast<const type*>(nullptr)) && is_adl_get && _is_adl_visit(static_cast<const type*>(nullptr));
	constexpr static bool is_optional = _has_op_bool(static_cast<const type*>(nullptr)) && _has_op_star(static_cast<const type*>(nullptr)) && _has_value_method(static_cast<const type*>(nullptr));
};

template<typename type>
constexpr decltype(sizeof(type)) size_of(const type* v) {
	(void)v;
	return sizeof(type);
}

template<auto... inds, typename factory_t, typename type>
constexpr auto call_pack_cpp17(factory_t&& io, const type& src) {
	if constexpr(sizeof...(inds)<size(static_cast<const type*>(nullptr)))
		call_pack_cpp17<inds..., sizeof...(inds)>( std::forward<decltype(io)>(io), src );
	else (pack(io, get<inds>(src)),...);
}

template<auto ind, typename factory_t, typename type, typename size_type>
constexpr auto read_tuple(factory_t&& io, type& to, size_type shift) {
	if constexpr( ind == size(static_cast<const type*>(nullptr)) ) return shift;
	else {
		auto& cur = get<ind>(to);
		auto cur_shift = read(io, cur, shift);
		return read_tuple<ind+1>(io, to, cur_shift);
	}
}

template<typename factory_t, typename type, typename size_type>
constexpr auto resize_as_need(factory_t&& io, type& to, size_type shift) {
	auto sz = io.container_size(to.size());
	auto ret = read(io, sz, shift);
	to.resize(sz);
	return ret;
}

} // namespace details

template<typename factory, typename type>
constexpr void pack(factory&& io, const type& what) {
	using concepts = details::cpp17_concepts<type>;
	if constexpr(concepts::is_variant) {
		pack(io, what.index());
		visit( [&io](const auto& v){ pack(io, v); }, what );
	}
	else if constexpr(concepts::is_optional) {
		pack(io, !!what);
		if(what) pack(io, *what);
	}
	else if constexpr(concepts::is_adl_get)
		details::call_pack_cpp17(io, what);
	else if constexpr(concepts::is_size_member) {
		pack(io, io.container_size(what.size()));
		for(auto& item:what) pack(io, item);
	}
	else if constexpr(concepts::is_pointer) {
		pack(io, bool{what==nullptr});
		if(what != nullptr) pack(io, *what);
	}
	else io.template write(&what, 1);
}


template<typename factory, typename type, typename size_type>
constexpr size_type read(factory&& io, type& to, size_type shift) {
	static_assert( !std::is_const_v<type>, "cannot read to constant storage" );

	using concepts = details::cpp17_concepts<type>;
	if constexpr(concepts::is_variant) {
		decltype(to.index()) ind;
		shift = read(io, ind, shift);
		return details::ct_var_emplace<0>(to, io, shift, (decltype(ind))0, ind);
	}
	else if constexpr(concepts::is_optional) {
		bool has_value;
		shift = read(io, has_value, shift);
		if(has_value) {
			auto& val = to.emplace();
			shift = read(io, val, shift);
		}
		return shift;
	}
	else if constexpr(concepts::is_adl_get) return details::read_tuple<0>(io, to, shift);
	else if constexpr(concepts::is_pointer) {
		bool is_null;
		shift = read(io, is_null, shift);
		if(!is_null) {
			to = io.template init_ptr<type>();
			shift = read(io, *to, shift);
		}
		return shift;
	}
	else if constexpr(concepts::is_size_member && concepts::is_resize_member) {
		shift = details::resize_as_need(io, to, shift);
		for(auto& item:to) shift = read(io, item, shift);
		return shift;
	}
	else if constexpr(concepts::is_size_member && !concepts::is_resize_member && concepts::is_emplace_member) {
		decltype(io.container_size(to.size())) sz;
		shift = read(io, sz, shift);
		for(decltype(sz) i=0;i<sz;++i) {
			auto cur = io.template create_item_for_emplace<type>();
			shift = read(io, cur, shift);
			to.emplace(std::move(cur));
		}
		return shift;
	}
	else {
		static_assert( std::is_integral_v<std::decay_t<type>> || std::is_floating_point_v<std::decay_t<type>> );
		io.read(shift, to);
		return shift + details::size_of(static_cast<type*>(nullptr));
	}
}

} // namespace serp
