namespace tt {
#include "reflection.ipp"
}

namespace tt2 {

struct st_1f {
	int f;
};
struct st_2f {
	int f;
	double long_name_CAPS_test;
};
struct st_4f {
	int f1, f2, f3, f4=42;
};

} // namespace tt2

static_assert( tt::size<tt2::st_2f> == 2 );
static_assert( tt::get<0>(tt2::st_1f{32}) == 32 );
static_assert( tt::get<0>(tt2::st_2f{32}) == 32 );
static_assert( tt::get<3>(tt2::st_4f{}) == 42 );
static_assert( tt::type_c<decltype(tt::get<0>(tt2::st_2f{32}))> == tt::type_c<int&&> );
static_assert( []{       tt2::st_2f obj{32}; return tt::type_c<decltype(tt::get<0>(obj))>; }() == tt::type_c<int&> );
static_assert( []{ const tt2::st_2f obj{32}; return tt::type_c<decltype(tt::get<0>(obj))>; }() == tt::type_c<const int&> );

#include <string_view>
#include <source_location>

struct factory {
	using source_location = std::source_location;
	using string_view = std::string_view;
};

using namespace std::literals;
static_assert( tt::field_name<factory, 0, tt2::st_2f>() == "f"sv );
static_assert( tt::field_name<factory, 1, tt2::st_2f>() == "long_name_CAPS_test"sv );

#include <optional>

namespace tt2 { 
struct st_opt {
	int f;
	std::optional<int> opt_f;
	constexpr static auto struct_fields_count() { return 2; }
};
} // namespace tt2

static_assert( tt::size<tt2::st_opt> == 2 );

int main(int,char**) {
	return 0;
}
