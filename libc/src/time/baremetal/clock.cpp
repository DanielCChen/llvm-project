//===-- Baremetal implementation of the clock function --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/time/clock.h"
#include "hdr/time_macros.h"
#include "hdr/types/struct_timespec.h"
#include "src/__support/CPP/limits.h"
#include "src/__support/common.h"
#include "src/__support/macros/config.h"
#include "src/__support/time/units.h"

namespace LIBC_NAMESPACE_DECL {

extern "C" bool __llvm_libc_timespec_get_active(struct timespec *ts);

LLVM_LIBC_FUNCTION(clock_t, clock, ()) {
  using namespace time_units;
  struct timespec ts;
  if (!__llvm_libc_timespec_get_active(&ts))
    return clock_t(-1);

  // The above call gets the CPU time in seconds plus nanoseconds.
  // The standard requires that we return clock_t(-1) if we cannot represent
  // clocks as a clock_t value.
  constexpr clock_t CLOCK_SECS_MAX =
      cpp::numeric_limits<clock_t>::max() / CLOCKS_PER_SEC;
  if (ts.tv_sec > CLOCK_SECS_MAX)
    return clock_t(-1);
  if (ts.tv_nsec / 1_s_ns > CLOCK_SECS_MAX - ts.tv_sec)
    return clock_t(-1);

  // For the integer computation converting tv_nsec to clocks to work
  // correctly, we want CLOCKS_PER_SEC to be less than 1000000000.
  static_assert(1_s_ns > CLOCKS_PER_SEC,
                "Expected CLOCKS_PER_SEC to be less than 1'000'000'000.");
  return clock_t(ts.tv_sec * CLOCKS_PER_SEC +
                 ts.tv_nsec / (1_s_ns / CLOCKS_PER_SEC));
}

} // namespace LIBC_NAMESPACE_DECL
