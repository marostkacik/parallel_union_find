#pragma once

#include "union_find/union_find.hpp"
#include "lockfree/node/node.hpp"

namespace parallel_union_find::lockfree
{
    using union_find = parallel_union_find::union_find<node>;
}
