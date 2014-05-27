// Copyright 2014 Samsung Electronics. All rights reseoved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CACHE_PARAMS_EFL_H_
#define CACHE_PARAMS_EFL_H_

struct CacheParamsEfl {
  int64 cache_total_capacity;
  int64 cache_min_dead_capacity;
  int64 cache_max_dead_capacity;
#if 0
  double dead_decoded_data_deletion_interval;
  int64 page_cache_capacity;
  int64 url_cache_memory_capacity;
  int64 url_cache_disk_capacity;
#endif
};

#endif /* CACHE_PARAMS_EFL_H_ */
