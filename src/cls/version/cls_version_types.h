// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_CLS_VERSION_TYPES_H
#define CEPH_CLS_VERSION_TYPES_H

#include "include/encoding.h"
#include "include/types.h"

class JSONObj;


struct obj_version {
    uint64_t ver;
    std::string tag;

    obj_version() : ver(0) {}

    void encode(ceph::buffer::list &bl) const
    {
        ENCODE_START(1, 1, bl);
        encode(ver, bl);
        encode(tag, bl);
        ENCODE_FINISH(bl);
    }

    void decode(ceph::buffer::list::const_iterator &bl)
    {
        DECODE_START(1, bl);
        decode(ver, bl);
        decode(tag, bl);
        DECODE_FINISH(bl);
    }

    void inc()
    {
        ver++;
    }

    void clear()
    {
        ver = 0;
        tag.clear();
    }

    bool empty() const
    {
        return tag.empty();
    }

    bool compare(struct obj_version *v) const
    {
        return (ver == v->ver &&
                tag.compare(v->tag) == 0);
    }

    bool operator==(const struct obj_version &v) const
    {
        return (ver == v.ver &&
                tag.compare(v.tag) == 0);
    }

    void dump(ceph::Formatter *f) const;
    void decode_json(JSONObj *obj);
    static void generate_test_instances(std::list<obj_version *> &o);
};
WRITE_CLASS_ENCODER(obj_version)

enum VersionCond {
    VER_COND_NONE =      0,
    VER_COND_EQ,  /* equal */
    VER_COND_GT,  /* greater than */
    VER_COND_GE,  /* greater or equal */
    VER_COND_LT,  /* less than */
    VER_COND_LE,  /* less or equal */
    VER_COND_TAG_EQ,
    VER_COND_TAG_NE,
};

struct obj_version_cond {
    struct obj_version ver;
    VersionCond cond;

    void encode(ceph::buffer::list &bl) const
    {
        ENCODE_START(1, 1, bl);
        encode(ver, bl);
        uint32_t c = (uint32_t)cond;
        encode(c, bl);
        ENCODE_FINISH(bl);
    }

    void decode(ceph::buffer::list::const_iterator &bl)
    {
        DECODE_START(1, bl);
        decode(ver, bl);
        uint32_t c;
        decode(c, bl);
        cond = (VersionCond)c;
        DECODE_FINISH(bl);
    }

};
WRITE_CLASS_ENCODER(obj_version_cond)


#endif
