/* Copyright (C) 2024 Hudyaev Alexey
 * SPDX-License-Identifier:    MIT
 */

#include "counter.h"

struct tag1 {};
struct tag2 {};
struct tag3 {};
struct empty_tag {};

template<typename> struct spec_check{};

int main(int,char**) {
	static_assert( stfmeta::is_specialization_of<tag1, spec_check> == false );
	static_assert( stfmeta::is_specialization_of<spec_check<tag1>, spec_check> == true );

	static_assert( stfmeta::counter<tag1> == 0 );
	static_assert( stfmeta::counter<tag1> == 1 );
	static_assert( stfmeta::counter<tag1> == 2 );

	static_assert( stfmeta::counter<tag2> == 0 );
	static_assert( stfmeta::counter<tag2> == 1 );
	static_assert( stfmeta::counter<tag2, double> == 2 );
	static_assert( stfmeta::counter<tag2, char> == 3 );

	static_assert( stfmeta::counter_val<tag2> == 3 );

	static_assert( stfmeta::counter_val<tag3> == -1 );
	static_assert( stfmeta::counter_val<tag3> == -1 );

	static_assert( stfmeta::counter_cnt<tag2> == stfmeta::type_c<char> );
	static_assert( stfmeta::counter_cnt<tag2, 3> == stfmeta::type_c<char> );
	static_assert( stfmeta::counter_cnt<tag2, 2> == stfmeta::type_c<double> );

	static_assert( stfmeta::push_back<tag3, int>() == 1 );
	static_assert( stfmeta::push_back<tag3, char>() == 2 );
	static_assert( stfmeta::push_back<tag3, double>() == 3 );
	static_assert( stfmeta::stored_types<tag3>() == stfmeta::type_list<int,char,double>{} );

	static_assert( filter(stfmeta::stored_types<tag3>(), [](auto t)consteval{return t!=stfmeta::type_c<int>;}) == stfmeta::type_list<char,double>{} );
	static_assert( filter(stfmeta::stored_types<empty_tag>(), [](auto t)consteval{return t!=stfmeta::type_c<int>;}) == stfmeta::type_list<>{} );

	static_assert( []{
		int cnt = 0;
		for_each(stfmeta::stored_types<tag3>(), [&](auto t){++cnt;});
		return cnt;
	}() == 3 );
	static_assert( []{
		int cnt = 0;
		for_each(stfmeta::stored_types<empty_tag>(), [&](auto t){++cnt;});
		return cnt;
	}() == 0 );

	return 0;
}
