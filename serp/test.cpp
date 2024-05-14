#include "serp.hpp"

//#define SNITCH_IMPLEMENTATION
//#include <snitch_all.hpp>
#include <snitch/snitch.hpp>

#include <bit>
#include <new>
#include <map>
#include <span>
#include <memory>
#include <vector>
#include <string>



template<typename type, template<typename...>class tmpl> constexpr const bool is_specialization_of = false;
template<template<typename...>class type, typename... args> constexpr const bool is_specialization_of<type<args...>, type> = true;

template<template<typename...>class tuple, typename... types> constexpr auto remove_const(const tuple<types...>&) { return tuple<std::remove_const_t<types>...>{}; }

struct io {
	using byte_type = std::byte;
	std::vector<byte_type> store;

	constexpr std::uint32_t container_size(std::size_t v) { return static_cast<std::uint32_t>(v); }

	template<typename type>
	constexpr auto create_item_for_emplace() {
		if constexpr(requires{ typename type::value_type{}; })
			return remove_const(typename type::value_type{});
		else return int{};
	}

	template<typename type>
	constexpr auto init_ptr(){
		if constexpr(std::is_pointer_v<type>) return new std::remove_pointer_t<type>{};
		else if constexpr(is_specialization_of<type, std::unique_ptr>) return std::make_unique<typename type::element_type>();
		else return std::make_shared<typename type::element_type>();
	}

	template<typename in_type, typename size_type>
	constexpr void write(const in_type* data, size_type size) {
		for(auto i=0;i<size;++i) {
			auto arr = std::bit_cast<std::array<byte_type,sizeof(in_type)>>(data[i]);
			for(auto& b:arr) store.emplace_back(b);
		}
	}
	template<typename size_type, typename type>
	constexpr void read(size_type start_pos, type& to) const {
		std::array<byte_type,sizeof(type)> cur_d{};
		for(unsigned i=0;i<sizeof(type);++i)
			cur_d[i] = store.at(i+start_pos);
		to = std::bit_cast<type>(cur_d);
	}
};

static_assert( !serp::details::cpp17_concepts<int>::is_adl_get );
static_assert( !serp::details::cpp17_concepts<int>::is_size_member );
static_assert( serp::details::cpp17_concepts<std::tuple<int,char>>::is_adl_get );
static_assert( serp::details::cpp17_concepts<std::string>::is_size_member );
static_assert( serp::details::cpp17_concepts<std::string>::is_resize_member );

static_assert( serp::details::cpp17_concepts<std::variant<int,std::string>>::is_variant );
static_assert( !serp::details::cpp17_concepts<std::tuple<int,char>>::is_variant );

using map_str_int = std::map<std::string,int>;
using string_cpp17_concepts = serp::details::cpp17_concepts<std::string>;
using map_str_int_cpp17_concepts = serp::details::cpp17_concepts<map_str_int>;
static_assert( serp::details::cpp17_concepts<map_str_int>::is_emplace_member );
static_assert( string_cpp17_concepts::is_size_member && string_cpp17_concepts::is_resize_member );
static_assert( map_str_int_cpp17_concepts::is_size_member && !map_str_int_cpp17_concepts::is_resize_member && map_str_int_cpp17_concepts::is_emplace_member );

constexpr auto test_pack(auto&& data) {
	io io;
	serp::pack(io, data);
	return io;
}
constexpr auto test_make_tuple_d() {
	return std::make_tuple((std::uint8_t)10, (std::int8_t)11, (std::uint64_t)12, (double)1.3);
}
constexpr auto test_make_tuple_a() {
	return std::make_tuple((std::uint8_t)0, (std::int8_t)0, (std::uint64_t)0, (double)0);
}

TEST_CASE("write simple") {
	auto test = []{
		io io;
		int o_pod = 0;
		int i_pod = 10, i2_pod = 20;
		serp::pack(io, i_pod);
		serp::pack(io, i2_pod);
		auto rbc = serp::read(io, o_pod, sizeof(i_pod));
		rbc /= (rbc == sizeof(i_pod)*2);
		return o_pod;
	};
	CHECK(test() == 20);
	static_assert( test() == 20 );
}

static_assert( []{
	auto io = test_pack( test_make_tuple_d() );
	auto ans = std::make_tuple((std::uint8_t)0, (std::int8_t)0);
	serp::read(io, ans);
	return get<0>(ans) + get<1>(ans);}() == 21);
TEST_CASE("write tuple") {
	auto io = test_pack( test_make_tuple_d() );

	auto ans = std::make_tuple((std::uint8_t)0, (std::int8_t)0);
	serp::read(io, ans);

	CHECK(get<0>(ans)==10);
	CHECK(get<1>(ans)==11);
}

static_assert( []{
	auto src = test_make_tuple_d();
	auto io = test_pack( src );
	auto answ = test_make_tuple_a();
       	auto rbc = serp::read(io, answ);
	auto correct_size = sizeof(get<0>(src)) + sizeof(get<1>(src)) + sizeof(get<2>(src)) + sizeof(get<3>(src));
	rbc /= (rbc == correct_size);
	return get<0>(answ) + get<1>(answ) + get<2>(answ) + get<3>(answ); 
}() == 34.3 );
TEST_CASE("read tuple") {
	auto src = test_make_tuple_d();
	auto io = test_pack( src );
	auto answ = test_make_tuple_a();
       	auto rbc = serp::read(io, answ);
	CHECK(rbc == (sizeof(get<0>(src)) + sizeof(get<1>(src)) + sizeof(get<2>(src)) + sizeof(get<3>(src))) );
	CHECK(get<0>(answ) == get<0>(src));
	CHECK(get<1>(answ) == get<1>(src));
	CHECK(get<2>(answ) == get<2>(src));
	CHECK(get<3>(answ) == get<3>(src));
}

TEST_CASE("rw string") {
	using namespace std::literals;
	auto src = "foo"s;
	io io;
	serp::pack(io, src);
	std::string answ;
	CHECK( serp::read(io, answ) == src.size() + sizeof(decltype(io.container_size(src.size()))) );
	CHECK( answ == src );
}

static_assert( []{ io io;
	using namespace std::literals;
	std::vector<std::string> answ, src{ {"foo"s, "bar"s, "baz"s} };
	serp::pack(io, src);
	serp::read(io, answ);
	return (answ.size() == 3) + (2*(answ[0][2]=='o')) + (4*(answ[1][2]=='r')) + (8*(answ[2][2]=='z'));
	}() == 15 );
TEST_CASE("rw vector of strings") {
	using namespace std::literals;
	std::vector<std::string> answ, src{ {"foo"s, "bar"s, "baz"s} };
	io io;
	serp::pack(io, src);
	serp::read(io, answ);
	CHECK(answ.size() == src.size());
	CHECK(answ[0] == "foo"sv);
	CHECK(answ[1] == "bar"sv);
	CHECK(answ[2] == "baz"sv);
}

static_assert( []{
	using namespace std::literals;
	io io;
	auto src = make_tuple("foo"s, "ops"sv);
	std::tuple<std::string,std::string> answ;
	serp::pack(io, src);
	serp::read(io, answ);
	return get<1>(answ)[2];
}() == 's' );

TEST_CASE("rw tuple with strings") {
	using namespace std::literals;
	io io;
	auto src = make_tuple("foo"s, "long string"sv);
	std::tuple<std::string,std::string> answ;
	CHECK(src != answ);
	serp::pack(io, src);
	serp::read(io, answ);
	CHECK(get<0>(src) == get<0>(answ));
	CHECK(get<1>(src) == get<1>(answ));
}

TEST_CASE("rw key pair") {
	using namespace std::literals;
	io io;
	std::map<std::string, int> answ, src{ {"key"s, 1}, {"oo"s, 2} };
	serp::pack(io, src);
	auto rb = serp::read(io, answ);
	CHECK( rb == 3 + 2 + (sizeof(decltype(io.container_size(src.size())))*3 + sizeof(int)*2) );
	CHECK( answ.size() == 2 );
	CHECK( answ.at("oo"s) == 2 );
	CHECK( answ.at("key"s) == 1 );
}


struct type_constexpr;
using base_tuple_constexpr = std::tuple<int,std::unique_ptr<type_constexpr>>;
struct type_constexpr : base_tuple_constexpr {
	using base_tuple_constexpr::base_tuple_constexpr;
	template<auto ind> friend constexpr auto& get(type_constexpr&v) { return std::get<ind>(static_cast<base_tuple_constexpr&>(v)); }
	template<auto ind> friend constexpr auto& get(const type_constexpr&v) { return std::get<ind>(static_cast<const base_tuple_constexpr&>(v)); }
};

struct type;
using base_tuple = std::tuple<int, type*, std::shared_ptr<type>>;
struct type : base_tuple {
	using base_tuple::base_tuple;
	template<auto ind> friend constexpr auto& get(type&v) { return std::get<ind>(static_cast<base_tuple&>(v)); }
	template<auto ind> friend constexpr auto& get(const type&v) { return std::get<ind>(static_cast<const base_tuple&>(v)); }
};
static_assert( serp::details::cpp17_concepts<type>::is_adl_get );
static_assert( serp::details::cpp17_concepts<std::shared_ptr<int>>::is_element_type_member );
static_assert( serp::details::cpp17_concepts<std::shared_ptr<int>>::is_get_member_with_ptr );
static_assert( serp::details::cpp17_concepts<std::shared_ptr<int>>::can_dereference );
static_assert( serp::details::cpp17_concepts<std::shared_ptr<int>>::is_smart_pointer );
static_assert( serp::details::cpp17_concepts<std::shared_ptr<int>>::is_pointer );
static_assert( serp::details::cpp17_concepts<const std::shared_ptr<int>>::is_pointer );
static_assert( serp::details::cpp17_concepts<const int*>::is_pointer );
static_assert( serp::details::cpp17_concepts<int*>::is_pointer );

static_assert( serp::details::cpp17_concepts<std::optional<int>>::is_optional );
static_assert( !serp::details::cpp17_concepts<int*>::is_optional );
static_assert( !serp::details::cpp17_concepts<int>::is_optional );

constexpr auto rw_pointer_maker() {
	io io;
	type_constexpr src{ 1, new type_constexpr }, answ;
	serp::pack(io, src);
	serp::read(io, answ);
	return answ;
}

static_assert( get<0>(rw_pointer_maker()) == 1 );
static_assert( get<0>( *get<1>(rw_pointer_maker()) ) == 0 );
static_assert( get<1>( *get<1>(rw_pointer_maker()) ) == nullptr );

TEST_CASE("rw pointers") {
	io io;
	type src{ 1, new type, std::make_shared<type>() }, answ;
	serp::pack(io, src);
	serp::read(io, answ);
	CHECK(get<0>(answ) == 1 );
	CHECK(get<1>(answ) != nullptr );
	CHECK(get<2>(answ) != nullptr );
	CHECK(get<0>(*get<1>(answ)) == 0);
	CHECK(get<0>(*get<2>(answ)) == 0);
	CHECK(get<1>(*get<1>(answ)) == nullptr);
	CHECK(get<2>(*get<2>(answ)) == nullptr);
}

static_assert( []{
	using namespace std::literals;
	io io;
	std::variant<std::vector<std::string>, std::string> answ, src = "str"s;
	serp::pack(io, src);
	auto shift = serp::read(io, answ);
	return (answ.index() == 1)
		+ (2*(get<1>(answ).size()==3))
		+ (4*(get<1>(answ)=="str"))
		+ (8*(shift==sizeof(decltype(src.index())) + sizeof(decltype(io.container_size(0))) + 3))
		;
}() == 15);

static_assert( []{
	using namespace std::literals;
	io io;
	std::optional<std::string> answ, src = "str"s;
	serp::pack(io, src);
	serp::read(io, answ);
	return answ.has_value() + (2*(*answ=="str"sv));
}() == 3);
static_assert( []{
	using namespace std::literals;
	io io;
	std::optional<std::string> answ, src;
	serp::pack(io, src);
	serp::read(io, answ);
	return answ.has_value();
}() == false);
