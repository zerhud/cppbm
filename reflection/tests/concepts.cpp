namespace tt {
#include "reflection.ipp"
}

#include <memory>
#include <vector>

static_assert( tt::vector<std::vector<int>> );
static_assert( tt::smart_ptr<std::shared_ptr<int>> );
static_assert( tt::iterable<std::vector<int>> );
static_assert( !tt::iterable<int> );
static_assert( !tt::any_ptr<int> );
static_assert( tt::any_ptr<int*> );

int main(int,char**) {
	return 0;
}
