#pragma once

#include "union_find/union_find.hpp"
#include "waitfree/node/node.hpp"

namespace parallel_union_find::waitfree
{
    using union_find = parallel_union_find::union_find<node>;
}
