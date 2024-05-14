#include "serp.hpp"

#include <string>
#include <variant>

static_assert( !serp::details::cpp17_concepts<int>::is_adl_get );
static_assert( !serp::details::cpp17_concepts<int>::is_size_member );
static_assert( serp::details::cpp17_concepts<std::tuple<int,char>>::is_adl_get );
static_assert( serp::details::cpp17_concepts<std::string>::is_size_member );
static_assert( serp::details::cpp17_concepts<std::string>::is_resize_member );

static_assert( serp::details::cpp17_concepts<std::variant<int,std::string>>::is_variant );
static_assert( !serp::details::cpp17_concepts<std::tuple<int,char>>::is_variant );

int main(int,char**) {
}
