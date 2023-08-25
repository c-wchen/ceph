// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2013 eNovance SAS <licensing@enovance.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation. See file COPYING.
 *
 */
#include "include/page.h"

#include "rgw_rest.h"
#include "rgw_op.h"
#include "rgw_rest_s3.h"
#include "rgw_rest_metadata.h"
#include "rgw_client_io.h"
#include "common/errno.h"
#include "common/strtol.h"
#include "rgw/rgw_b64.h"
#include "include/assert.h"

#define dout_context g_ceph_context
#define dout_subsys ceph_subsys_rgw

const string RGWOp_Metadata_Get::name()
{
    return "get_metadata";
}

static inline void frame_metadata_key(req_state * s, string & out)
{
    bool exists;
    string key = s->info.args.get("key", &exists);

    string section;
    if (!s->init_state.url_bucket.empty()) {
        section = s->init_state.url_bucket;
    }
    else {
        section = key;
        key.clear();
    }

    out = section;

    if (!key.empty()) {
        out += string(":") + key;
    }
}

void RGWOp_Metadata_Get::execute()
{
    string metadata_key;

    frame_metadata_key(s, metadata_key);

    /* Get keys */
    http_ret = store->meta_mgr->get(metadata_key, s->formatter);
    if (http_ret < 0) {
        dout(5) << "ERROR: can't get key: " << cpp_strerror(http_ret) << dendl;
        return;
    }

    http_ret = 0;
}

const string RGWOp_Metadata_List::name()
{
    return "list_metadata";
}

void RGWOp_Metadata_List::execute()
{
    string marker;
    ldout(s->cct, 16) << __func__
        << " raw marker " << s->info.args.get("marker")
        << dendl;

    try {
        marker = s->info.args.get("marker");
        if (!marker.empty()) {
            marker = rgw::from_base64(marker);
        }
        ldout(s->cct, 16) << __func__ << " marker " << marker << dendl;
    }
    catch( ...) {
        marker = std::string("");
    }

    bool max_entries_specified;
    string max_entries_str =
        s->info.args.get("max-entries", &max_entries_specified);

    bool extended_response = (max_entries_specified);   /* for backward compatibility, if max-entries is not specified
                                                           we will send the old response format */
    uint64_t max_entries = 0;

    if (max_entries_specified) {
        string err;
        max_entries =
            (unsigned)strict_strtol(max_entries_str.c_str(), 10, &err);
        if (!err.empty()) {
            dout(5) << "Error parsing max-entries " << max_entries_str << dendl;
            http_ret = -EINVAL;
            return;
        }
    }

    string metadata_key;

    frame_metadata_key(s, metadata_key);
    /* List keys */
    void *handle;
    int max = 1000;

    /* example markers:
       marker = "3:b55a9110:root::bu_9:head";
       marker = "3:b9a8b2a6:root::sorry_janefonda_890:head";
       marker = "3:bf885d8f:root::sorry_janefonda_665:head";
     */

    http_ret = store->meta_mgr->list_keys_init(metadata_key, marker, &handle);
    if (http_ret < 0) {
        dout(5) << "ERROR: can't get key: " << cpp_strerror(http_ret) << dendl;
        return;
    }

    bool truncated;
    uint64_t count = 0;

    if (extended_response) {
        s->formatter->open_object_section("result");
    }

    s->formatter->open_array_section("keys");

    uint64_t left;
    do {
        list < string > keys;
        left = (max_entries_specified ? max_entries - count : max);
        http_ret =
            store->meta_mgr->list_keys_next(handle, left, keys, &truncated);
        if (http_ret < 0) {
            dout(5) << "ERROR: lists_keys_next(): " << cpp_strerror(http_ret)
                << dendl;
            return;
        }

        for (list < string >::iterator iter = keys.begin(); iter != keys.end();
             ++iter) {
            s->formatter->dump_string("key", *iter);
            ++count;
        }

    } while (truncated && left > 0);

    s->formatter->close_section();

    if (extended_response) {
        encode_json("truncated", truncated, s->formatter);
        encode_json("count", count, s->formatter);
        if (truncated) {
            string esc_marker =
                rgw::to_base64(store->meta_mgr->get_marker(handle));
            encode_json("marker", esc_marker, s->formatter);
        }
        s->formatter->close_section();
    }
    store->meta_mgr->list_keys_complete(handle);

    http_ret = 0;
}

int RGWOp_Metadata_Put::get_data(bufferlist & bl)
{
    size_t cl = 0;
    char *data;
    int read_len;

    if (s->length)
        cl = atoll(s->length);
    if (cl) {
        data = (char *)malloc(cl + 1);
        if (!data) {
            return -ENOMEM;
        }
        read_len = recv_body(s, data, cl);
        if (cl != (size_t) read_len) {
            dout(10) << "recv_body incomplete" << dendl;
        }
        if (read_len < 0) {
            free(data);
            return read_len;
        }
        bl.append(data, read_len);
    }
    else {
        int chunk_size = CEPH_PAGE_SIZE;
        const char *enc = s->info.env->get("HTTP_TRANSFER_ENCODING");
        if (!enc || strcmp(enc, "chunked")) {
            return -ERR_LENGTH_REQUIRED;
        }
        data = (char *)malloc(chunk_size);
        if (!data) {
            return -ENOMEM;
        }
        do {
            read_len = recv_body(s, data, chunk_size);
            if (read_len < 0) {
                free(data);
                return read_len;
            }
            bl.append(data, read_len);
        } while (read_len == chunk_size);
    }

    free(data);
    return 0;
}

void RGWOp_Metadata_Put::execute()
{
    bufferlist bl;
    string metadata_key;

    http_ret = get_data(bl);
    if (http_ret < 0) {
        return;
    }

    http_ret = do_aws4_auth_completion();
    if (http_ret < 0) {
        return;
    }

    frame_metadata_key(s, metadata_key);

    RGWMetadataHandler::sync_type_t sync_type =
        RGWMetadataHandler::APPLY_ALWAYS;

    bool mode_exists = false;
    string mode_string = s->info.args.get("update-type", &mode_exists);
    if (mode_exists) {
        bool parsed = RGWMetadataHandler::string_to_sync_type(mode_string,
                                                              sync_type);
        if (!parsed) {
            http_ret = -EINVAL;
            return;
        }
    }

    http_ret = store->meta_mgr->put(metadata_key, bl, sync_type,
                                    &ondisk_version);
    if (http_ret < 0) {
        dout(5) << "ERROR: can't put key: " << cpp_strerror(http_ret) << dendl;
        return;
    }
    // translate internal codes into return header
    if (http_ret == STATUS_NO_APPLY)
        update_status = "skipped";
    else if (http_ret == STATUS_APPLIED)
        update_status = "applied";
}

void RGWOp_Metadata_Put::send_response()
{
    int http_return_code = http_ret;
    if ((http_ret == STATUS_NO_APPLY) || (http_ret == STATUS_APPLIED))
        http_return_code = STATUS_NO_CONTENT;
    set_req_state_err(s, http_return_code);
    dump_errno(s);
    stringstream ver_stream;
    ver_stream << "ver:" << ondisk_version.ver << ",tag:" << ondisk_version.tag;
    dump_header_if_nonempty(s, "RGWX_UPDATE_STATUS", update_status);
    dump_header_if_nonempty(s, "RGWX_UPDATE_VERSION", ver_stream.str());
    end_header(s);
}

void RGWOp_Metadata_Delete::execute()
{
    string metadata_key;

    frame_metadata_key(s, metadata_key);
    http_ret = store->meta_mgr->remove(metadata_key);
    if (http_ret < 0) {
        dout(5) << "ERROR: can't remove key: " << cpp_strerror(http_ret) <<
            dendl;
        return;
    }
    http_ret = 0;
}

void RGWOp_Metadata_Lock::execute()
{
    string duration_str, lock_id;
    string metadata_key;

    frame_metadata_key(s, metadata_key);

    http_ret = 0;

    duration_str = s->info.args.get("length");
    lock_id = s->info.args.get("lock_id");

    if ((!s->info.args.exists("key")) ||
        (duration_str.empty()) || lock_id.empty()) {
        dout(5) << "Error invalid parameter list" << dendl;
        http_ret = -EINVAL;
        return;
    }

    int dur;
    string err;

    dur = strict_strtol(duration_str.c_str(), 10, &err);
    if (!err.empty() || dur <= 0) {
        dout(5) << "invalid length param " << duration_str << dendl;
        http_ret = -EINVAL;
        return;
    }
    http_ret =
        store->meta_mgr->lock_exclusive(metadata_key, make_timespan(dur),
                                        lock_id);
    if (http_ret == -EBUSY)
        http_ret = -ERR_LOCKED;
}

void RGWOp_Metadata_Unlock::execute()
{
    string lock_id;
    string metadata_key;

    frame_metadata_key(s, metadata_key);

    http_ret = 0;

    lock_id = s->info.args.get("lock_id");

    if ((!s->info.args.exists("key")) || lock_id.empty()) {
        dout(5) << "Error invalid parameter list" << dendl;
        http_ret = -EINVAL;
        return;
    }

    http_ret = store->meta_mgr->unlock(metadata_key, lock_id);
}

RGWOp *RGWHandler_Metadata::op_get()
{
    if (s->info.args.exists("key"))
        return new RGWOp_Metadata_Get;
    else
        return new RGWOp_Metadata_List;
}

RGWOp *RGWHandler_Metadata::op_put()
{
    return new RGWOp_Metadata_Put;
}

RGWOp *RGWHandler_Metadata::op_delete()
{
    return new RGWOp_Metadata_Delete;
}

RGWOp *RGWHandler_Metadata::op_post()
{
    if (s->info.args.exists("lock"))
        return new RGWOp_Metadata_Lock;
    else if (s->info.args.exists("unlock"))
        return new RGWOp_Metadata_Unlock;

    return NULL;
}
