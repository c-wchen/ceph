// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2019 Red Hat Inc.
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include <ostream>

#include "osd/scheduler/OpScheduler.h"

#include "common/WeightedPriorityQueue.h"
#include "osd/scheduler/mClockScheduler.h"

namespace ceph::osd::scheduler
{

OpSchedulerRef make_scheduler(
    CephContext *cct, int whoami, uint32_t num_shards, int shard_id,
    bool is_rotational, std::string_view osd_objectstore, MonClient *monc)
{
    const std::string *type = &cct->_conf->osd_op_queue;
    if (*type == "debug_random") {
        static const std::string index_lookup[] = { "mclock_scheduler",
                                                    "wpq"
                                                  };
        srand(time(NULL));
        unsigned which = rand() % (sizeof(index_lookup) / sizeof(index_lookup[0]));
        type = &index_lookup[which];
    }

    // Force the use of 'wpq' scheduler for filestore OSDs.
    // The 'mclock_scheduler' is not supported for filestore OSDs.
    if (*type == "wpq" || osd_objectstore == "filestore") {
        return std::make_unique <
               ClassedOpQueueScheduler<WeightedPriorityQueue<OpSchedulerItem, client> >> (
                   cct,
                   cct->_conf->osd_op_pq_max_tokens_per_priority,
                   cct->_conf->osd_op_pq_min_cost
               );
    } else if (*type == "mclock_scheduler") {
        // default is 'mclock_scheduler'
        return std::make_unique <
               mClockScheduler > (cct, whoami, num_shards, shard_id, is_rotational, monc);
    } else {
        ceph_assert("Invalid choice of wq" == 0);
    }
}

std::ostream &operator<<(std::ostream &lhs, const OpScheduler &rhs)
{
    rhs.print(lhs);
    return lhs;
}

}
