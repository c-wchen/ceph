// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_LIBRBD_CACHE_PWL_SSD_LOG_OPERATION_H
#define CEPH_LIBRBD_CACHE_PWL_SSD_LOG_OPERATION_H

#include "librbd/cache/pwl/LogOperation.h"


namespace librbd
{
namespace cache
{
namespace pwl
{
namespace ssd
{

class DiscardLogOperation : public pwl::DiscardLogOperation
{
public:
    DiscardLogOperation(
        std::shared_ptr<SyncPoint> sync_point, uint64_t image_offset_bytes,
        uint64_t write_bytes, uint32_t discard_granularity_bytes,
        utime_t dispatch_time, PerfCounters *perfcounter, CephContext *cct)
        : pwl::DiscardLogOperation(sync_point, image_offset_bytes, write_bytes,
                                   discard_granularity_bytes, dispatch_time,
                                   perfcounter, cct) {}
    void init_op(
        uint64_t current_sync_gen, bool persist_on_flush,
        uint64_t last_op_sequence_num, Context *write_persist,
        Context *write_append) override;
};

} // namespace ssd
} // namespace pwl
} // namespace cache
} // namespace librbd

#endif // CEPH_LIBRBD_CACHE_PWL_SSD_LOG_OPERATION_H
