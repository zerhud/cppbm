
namespace tt {
#include "reflection.ipp"
}

namespace tt2 {

struct st_1f {
	int f;
	double long_name_CAPS_test;
};

} // namespace tt2

static_assert( tt::size<tt2::st_1f> == 2 );
static_assert( tt::get<0>(tt2::st_1f{32}) == 32 );
static_assert( tt::type_c<decltype(tt::get<0>(tt2::st_1f{32}))> == tt::type_c<int&> );

#include <string_view>
#include <source_location>

struct factory {
	using source_location = std::source_location;
	using string_view = std::string_view;
};

using namespace std::literals;
static_assert( tt::field_name<factory, 0, tt2::st_1f>() == "f"sv );
static_assert( tt::field_name<factory, 1, tt2::st_1f>() == "long_name_CAPS_test"sv );

int main(int,char**) {
	return 0;
}
