namespace tt {
#include "reflection.ipp"
}

#include <memory>
#include <vector>

template<typename left, typename right>
constexpr bool same = tt::type_c<left> == tt::type_c<right>;

static_assert( tt::vector<std::vector<int>> );
static_assert( tt::smart_ptr<std::shared_ptr<int>> );
static_assert( tt::iterable<std::vector<int>> );
static_assert( !tt::iterable<int> );
static_assert( !tt::any_ptr<int> );
static_assert( tt::any_ptr<int*> );

static_assert( same<int, int> );
static_assert( same<tt::decay_t<int>, int> );
static_assert( same<tt::decay_t<int&>, int> );
static_assert( same<tt::decay_t<int&&>, int> );
static_assert( same<tt::decay_t<int*>, int*> );
static_assert( same<tt::decay_t<int[5]>, int*> );
static_assert( same<tt::decay_t<int(char)>, int(*)(char)> );

int main(int,char**) {
	return 0;
}
