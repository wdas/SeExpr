/*
CONFIDENTIAL INFORMATION: This software is the confidential and proprietary
information of Walt Disney Animation Studios ("WDAS"). This software may not
be used, disclosed, reproduced or distributed for any purpose without prior
written authorization and license from WDAS. Reproduction of any section of
this software must include this legend and all copyright notices.
© Disney Enterprises, Inc. All rights reserved.
*/

#pragma once

#include <tbb/tbb.h>

template <typename T, typename CallableT>
inline void serial_for(T begin, T end, T incr, CallableT f)
{
    for (T i = begin; i != end; i = i + incr) {
        f(i);
    }
}

#undef SEEXPR_ENABLE_TBB
#ifdef SEEXPR_ENABLE_TBB
template<typename ...Params>
inline void CONCURRENT_FOR(Params&& ...params) { tbb::parallel_for(std::forward<Params>(params)...); }
#else
template<typename ...Params>
inline void CONCURRENT_FOR(Params&& ...params) { serial_for(std::forward<Params>(params)...); }
#endif
