
namespace tt {
#include "reflection.ipp"
}

namespace tt2 {
	template<typename...> struct tlist{};
}


static_assert( tt::type_list<int,char>{} == tt::type_list<int,char>{} );
static_assert( tt::type_list<int,double>{} != tt::type_list<int,char>{} );

static_assert( push_back<char>(tt::type_list<int>{}) == tt::type_list<int,char>{} );
static_assert( push_back_if_not_contains<int>(tt::type_list<int>{}) == tt::type_list<int>{} );
static_assert( push_back_if_not_contains<char>(tt::type_list<int>{}) == tt::type_list<int,char>{} );
static_assert( push_front_if_not_contains<int>(tt::type_list<int>{}) == tt::type_list<int>{} );
static_assert( push_front_if_not_contains<char>(tt::type_list<int>{}) == tt::type_list<char,int>{} );

static_assert( transform_uniq(tt::type_list<int,char,double>{}) == tt::type_list<int,char,double>{} );
static_assert( transform_uniq(tt::type_list<int,char,int>{}) == tt::type_list<char,int>{} );

static_assert( tt::type_c<decltype(tt::create_list(tt2::tlist<int,char>{}))> == tt::type_c<tt::type_list<int,char>> );
static_assert( tt2::tlist<int,char>{} == tt::type_list<int,char>{} );
static_assert( tt::type_list<int,char>{} == tt2::tlist<int,char>{} );

int main(int,char**) {
	return 0;
}
