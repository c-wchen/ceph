
// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab ft=cpp

/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2019 Red Hat, Inc.
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation. See file COPYING.
 *
 */


#pragma once

#include "common/ptr_wrapper.h"

#include "svc_meta_be.h"
#include "svc_meta_be_types.h"

class RGWSI_MetaBackend_Handler;

using RGWSI_Bucket_BE_Handler = ptr_wrapper<RGWSI_MetaBackend_Handler, RGWSI_META_BE_TYPES::BUCKET>;
using RGWSI_BucketInstance_BE_Handler = ptr_wrapper<RGWSI_MetaBackend_Handler, RGWSI_META_BE_TYPES::BI>;


using RGWSI_Bucket_EP_Ctx = ptr_wrapper<RGWSI_MetaBackend::Context, RGWSI_META_BE_TYPES::BUCKET>;
using RGWSI_Bucket_BI_Ctx = ptr_wrapper<RGWSI_MetaBackend::Context, RGWSI_META_BE_TYPES::BI>;

struct RGWSI_Bucket_X_Ctx {
    RGWSI_Bucket_EP_Ctx ep;
    RGWSI_Bucket_BI_Ctx bi;
};

