namespace tt {
#include "reflection.ipp"
}

static_assert( tt::type_c<tt::decay_t<int>> == tt::type_c<int> );
static_assert( tt::type_c<tt::decay_t<int&>> == tt::type_c<int> );
static_assert( tt::type_c<tt::decay_t<int&&>> == tt::type_c<int> );
static_assert( tt::type_c<tt::decay_t<const int&>> == tt::type_c<int> );
static_assert( tt::type_c<tt::decay_t<const int&&>> == tt::type_c<int> );
static_assert( tt::type_c<tt::decay_t<const int*>> == tt::type_c<const int*> );
static_assert( tt::type_c<tt::decay_t<int[2]>> == tt::type_c<int*> );
static_assert( tt::type_c<tt::decay_t<int(int)>> == tt::type_c<int(*)(int)> );

static_assert( tt::is_rvalue_reference_v<int&&> );
static_assert( !tt::is_rvalue_reference_v<int&> );

static_assert( !tt::is_lvalue_reference_v<int&&> );
static_assert( tt::is_lvalue_reference_v<int&> );

static_assert( !tt::is_const_v<const int&&> );
static_assert( !tt::is_const_v<const int&> );
static_assert( !tt::is_const_v<const int*> );
static_assert( tt::is_const_v<const int> );
static_assert( tt::is_const_v<const int> );
static_assert( tt::is_const_v<int * const> );

static_assert( tt::type_c<decltype(tt::forward_like<char&&>(tt::lref<int&>()))> == tt::type_c<int&&> );
static_assert( tt::type_c<decltype(tt::forward_like<char&&>(tt::lref<int&&>()))> == tt::type_c<int&&> );
static_assert( tt::type_c<decltype(tt::forward_like<const char&&>(tt::lref<const int&>()))> == tt::type_c<const int&&> );
static_assert( tt::type_c<decltype(tt::forward_like<const char&&>(tt::lref<const int&&>()))> == tt::type_c<const int&&> );
static_assert( tt::type_c<decltype(tt::forward_like<char&>(tt::lref<int&>()))> == tt::type_c<int&> );
static_assert( tt::type_c<decltype(tt::forward_like<char&>(tt::lref<int&&>()))> == tt::type_c<int&> );
static_assert( tt::type_c<decltype(tt::forward_like<const char&>(tt::lref<const int&>()))> == tt::type_c<const int&> );
static_assert( tt::type_c<decltype(tt::forward_like<const char&>(tt::lref<const int&&>()))> == tt::type_c<const int&> );

int main(int,char**) {
	return 0;
}
