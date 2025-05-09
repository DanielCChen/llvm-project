//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <unordered_set>

// template <class Value, class Hash = hash<Value>, class Pred = equal_to<Value>,
//           class Alloc = allocator<Value>>
// class unordered_set

// explicit unordered_set(size_type n);

#include <unordered_set>
#include <cassert>

#include "test_macros.h"
#include "../../../NotConstructible.h"
#include "../../../test_compare.h"
#include "../../../test_hash.h"
#include "test_allocator.h"

int main(int, char**) {
  {
    typedef std::unordered_set<NotConstructible,
                               test_hash<NotConstructible>,
                               test_equal_to<NotConstructible>,
                               test_allocator<NotConstructible> >
        C;
    C c = 7;
    LIBCPP_ASSERT(c.bucket_count() == 7);
    assert(c.hash_function() == test_hash<NotConstructible>());
    assert(c.key_eq() == test_equal_to<NotConstructible>());
    assert(c.get_allocator() == (test_allocator<NotConstructible>()));
    assert(c.size() == 0);
    assert(c.empty());
    assert(std::distance(c.begin(), c.end()) == 0);
    assert(c.load_factor() == 0);
    assert(c.max_load_factor() == 1);
  }

  return 0;
}
