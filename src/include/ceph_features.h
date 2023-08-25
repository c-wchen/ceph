#ifndef __CEPH_FEATURES
#define __CEPH_FEATURES

#include "sys/types.h"

/*
 * Each time we reclaim bits for reuse we need to specify another bit
 * that, if present, indicates we have the new incarnation of that
 * feature.  Base case is 1 (first use)
 */
#define CEPH_FEATURE_INCARNATION_1 (0ull)
#define CEPH_FEATURE_INCARNATION_2 (1ull<<57)   // CEPH_FEATURE_SERVER_JEWEL

#define DEFINE_CEPH_FEATURE(bit, incarnation, name)			\
	const static uint64_t CEPH_FEATURE_##name = (1ULL<<bit);		\
	const static uint64_t CEPH_FEATUREMASK_##name =			\
		(1ULL<<bit | CEPH_FEATURE_INCARNATION_##incarnation);

// this bit is ignored but still advertised by release *when*
#define DEFINE_CEPH_FEATURE_DEPRECATED(bit, incarnation, name, when) \
	const static uint64_t DEPRECATED_CEPH_FEATURE_##name = (1ULL<<bit); \
	const static uint64_t DEPRECATED_CEPH_FEATUREMASK_##name =		\
		(1ULL<<bit | CEPH_FEATURE_INCARNATION_##incarnation);

// this bit is ignored by release *unused* and not advertised by
// release *unadvertised*
#define DEFINE_CEPH_FEATURE_RETIRED(bit, inc, name, unused, unadvertised)

// test for a feature.  this test is safer than a typical mask against
// the bit because it ensures that we have the bit AND the marker for the
// bit's incarnation.  this must be used in any case where the features
// bits may include an old meaning of the bit.
#define HAVE_FEATURE(x, name)				\
	(((x) & (CEPH_FEATUREMASK_##name)) == (CEPH_FEATUREMASK_##name))

/*
 * Notes on deprecation:
 *
 * A *major* release is a release through which all upgrades must pass
 * (e.g., jewel).  For example, no pre-jewel server will ever talk to
 * a post-jewel server (mon, osd, etc).
 *
 * For feature bits used *only* on the server-side:
 *
 *  - In the first phase we indicate that a feature is DEPRECATED as of
 *    a particular release.  This is the first major release X (say,
 *    jewel) that does not depend on its peers advertising the feature.
 *    That is, it safely assumes its peers all have the feature.  We
 *    indicate this with the DEPRECATED macro.  For example,
 *
 *      DEFINE_CEPH_FEATURE_DEPRECATED( 2, 1, MONCLOCKCHECK, JEWEL)
 *
 *    because 10.2.z (jewel) did not care if its peers advertised this
 *    feature bit.
 *
 *  - In the second phase we stop advertising the the bit and call it
 *    RETIRED.  This can normally be done in the *next* major release
 *    following the one in which we marked the feature DEPRECATED.  In
 *    the above example, for 12.0.z (luminous) we can say:
 *
 *      DEFINE_CEPH_FEATURE_RETIRED( 2, 1, MONCLOCKCHECK, JEWEL, LUMINOUS)
 *
 *  - The bit can be reused in the first post-luminous release, 13.0.z
 *    (m).
 *
 * This ensures that no two versions who have different meanings for
 * the bit ever speak to each other.
 */

DEFINE_CEPH_FEATURE(0, 1, UID)
    DEFINE_CEPH_FEATURE(1, 1, NOSRCADDR)
    DEFINE_CEPH_FEATURE_RETIRED(2, 1, MONCLOCKCHECK, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE(3, 1, FLOCK)
    DEFINE_CEPH_FEATURE(4, 1, SUBSCRIBE2)
    DEFINE_CEPH_FEATURE(5, 1, MONNAMES)
    DEFINE_CEPH_FEATURE(6, 1, RECONNECT_SEQ)
    DEFINE_CEPH_FEATURE(7, 1, DIRLAYOUTHASH)
    DEFINE_CEPH_FEATURE(8, 1, OBJECTLOCATOR)
    DEFINE_CEPH_FEATURE(9, 1, PGID64)
    DEFINE_CEPH_FEATURE(10, 1, INCSUBOSDMAP)
    DEFINE_CEPH_FEATURE(11, 1, PGPOOL3)
    DEFINE_CEPH_FEATURE(12, 1, OSDREPLYMUX)
    DEFINE_CEPH_FEATURE(13, 1, OSDENC)
    DEFINE_CEPH_FEATURE_RETIRED(14, 1, OMAP, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(14, 2, SERVER_KRAKEN)
    DEFINE_CEPH_FEATURE(15, 1, MONENC)
    DEFINE_CEPH_FEATURE_RETIRED(16, 1, QUERY_T, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE_RETIRED(17, 1, INDEP_PG_MAP, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE(18, 1, CRUSH_TUNABLES)
    DEFINE_CEPH_FEATURE_RETIRED(19, 1, CHUNKY_SCRUB, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(19, 2, OSD_PGLOG_HARDLIMIT)

    DEFINE_CEPH_FEATURE_RETIRED(20, 1, MON_NULLROUTE, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE_RETIRED(21, 1, MON_GV, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(21, 2, SERVER_LUMINOUS)
    DEFINE_CEPH_FEATURE(21, 2, RESEND_ON_SPLIT)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, RADOS_BACKOFF)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, OSDMAP_PG_UPMAP)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, CRUSH_CHOOSE_ARGS)
                                                // overlap
    DEFINE_CEPH_FEATURE_RETIRED(22, 1, BACKFILL_RESERVATION, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE(23, 1, MSG_AUTH)
    DEFINE_CEPH_FEATURE_RETIRED(24, 1, RECOVERY_RESERVATION, JEWEL, LUNINOUS)

    DEFINE_CEPH_FEATURE(25, 1, CRUSH_TUNABLES2)
    DEFINE_CEPH_FEATURE(26, 1, CREATEPOOLID)
    DEFINE_CEPH_FEATURE(27, 1, REPLY_CREATE_INODE)
    DEFINE_CEPH_FEATURE_RETIRED(28, 1, OSD_HBMSGS, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(28, 2, SERVER_M)
    DEFINE_CEPH_FEATURE(29, 1, MDSENC)
    DEFINE_CEPH_FEATURE(30, 1, OSDHASHPSPOOL)
    DEFINE_CEPH_FEATURE(31, 1, MON_SINGLE_PAXOS)// deprecate me
    DEFINE_CEPH_FEATURE_RETIRED(32, 1, OSD_SNAPMAPPER, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE_RETIRED(33, 1, MON_SCRUB, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE_RETIRED(34, 1, OSD_PACKED_RECOVERY, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE(35, 1, OSD_CACHEPOOL)
    DEFINE_CEPH_FEATURE(36, 1, CRUSH_V2)
    DEFINE_CEPH_FEATURE(37, 1, EXPORT_PEER)
    DEFINE_CEPH_FEATURE(38, 1, OSD_ERASURE_CODES)
    DEFINE_CEPH_FEATURE(38, 1, OSD_OSD_TMAP2OMAP)
                                                // overlap
    DEFINE_CEPH_FEATURE(39, 1, OSDMAP_ENC)
    DEFINE_CEPH_FEATURE(40, 1, MDS_INLINE_DATA)
    DEFINE_CEPH_FEATURE(41, 1, CRUSH_TUNABLES3)
    DEFINE_CEPH_FEATURE(41, 1, OSD_PRIMARY_AFFINITY)// overlap
    DEFINE_CEPH_FEATURE(42, 1, MSGR_KEEPALIVE2)
    DEFINE_CEPH_FEATURE(43, 1, OSD_POOLRESEND)
    DEFINE_CEPH_FEATURE(44, 1, ERASURE_CODE_PLUGINS_V2)
    DEFINE_CEPH_FEATURE_RETIRED(45, 1, OSD_SET_ALLOC_HINT, JEWEL, LUMINOUS)

    DEFINE_CEPH_FEATURE(46, 1, OSD_FADVISE_FLAGS)
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_REPOP, JEWEL, LUMINOUS)
                                                                // overlap
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_OBJECT_DIGEST, JEWEL, LUMINOUS)
                                                                        // overlap
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_TRANSACTION_MAY_LAYOUT, JEWEL, LUMINOUS)
                                                                                // overlap
    DEFINE_CEPH_FEATURE(47, 1, MDS_QUOTA)
    DEFINE_CEPH_FEATURE(48, 1, CRUSH_V4)
    DEFINE_CEPH_FEATURE_RETIRED(49, 1, OSD_MIN_SIZE_RECOVERY, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE_RETIRED(49, 1, OSD_PROXY_FEATURES, JEWEL, LUMINOUS)
                                                                        // overlap
    DEFINE_CEPH_FEATURE(50, 1, MON_METADATA)
    DEFINE_CEPH_FEATURE(51, 1, OSD_BITWISE_HOBJ_SORT)
    DEFINE_CEPH_FEATURE(52, 1, OSD_PROXY_WRITE_FEATURES)
    DEFINE_CEPH_FEATURE(53, 1, ERASURE_CODE_PLUGINS_V3)
    DEFINE_CEPH_FEATURE(54, 1, OSD_HITSET_GMT)
    DEFINE_CEPH_FEATURE(55, 1, HAMMER_0_94_4)
    DEFINE_CEPH_FEATURE(56, 1, NEW_OSDOP_ENCODING)
    DEFINE_CEPH_FEATURE(57, 1, MON_STATEFUL_SUB)
    DEFINE_CEPH_FEATURE(57, 1, MON_ROUTE_OSDMAP)// overlap
    DEFINE_CEPH_FEATURE(57, 1, OSDSUBOP_NO_SNAPCONTEXT)
                                                    // overlap
    DEFINE_CEPH_FEATURE(57, 1, SERVER_JEWEL)// overlap
    DEFINE_CEPH_FEATURE(58, 1, CRUSH_TUNABLES5)
    DEFINE_CEPH_FEATURE(58, 1, NEW_OSDOPREPLY_ENCODING)
                                                    // overlap
    DEFINE_CEPH_FEATURE(58, 1, FS_FILE_LAYOUT_V2)
                                                // overlap
    DEFINE_CEPH_FEATURE(59, 1, FS_BTIME)
    DEFINE_CEPH_FEATURE(59, 1, FS_CHANGE_ATTR)
                                            // overlap
    DEFINE_CEPH_FEATURE(59, 1, MSG_ADDR2)
                                        // overlap
    DEFINE_CEPH_FEATURE(60, 1, OSD_RECOVERY_DELETES)// *do not share this bit*
    DEFINE_CEPH_FEATURE(61, 1, CEPHX_V2)// *do not share this bit*
    DEFINE_CEPH_FEATURE(62, 1, RESERVED)// do not use; used as a sentinal
    DEFINE_CEPH_FEATURE_DEPRECATED(63, 1, RESERVED_BROKEN, LUMINOUS)// client-facing
/*
 * Features supported.  Should be everything above.
 */
#define CEPH_FEATURES_ALL		 \
	(CEPH_FEATURE_UID |		 \
	 CEPH_FEATURE_NOSRCADDR |	 \
	 CEPH_FEATURE_FLOCK |		 \
	 CEPH_FEATURE_SUBSCRIBE2 |	 \
	 CEPH_FEATURE_MONNAMES |	 \
	 CEPH_FEATURE_RECONNECT_SEQ |	 \
	 CEPH_FEATURE_DIRLAYOUTHASH |	 \
	 CEPH_FEATURE_OBJECTLOCATOR |	 \
	 CEPH_FEATURE_PGID64 |		 \
	 CEPH_FEATURE_INCSUBOSDMAP |	 \
	 CEPH_FEATURE_PGPOOL3 |		 \
	 CEPH_FEATURE_OSDREPLYMUX |	 \
	 CEPH_FEATURE_OSDENC |		 \
	 CEPH_FEATURE_MONENC |		 \
	 CEPH_FEATURE_CRUSH_TUNABLES |	 \
	 CEPH_FEATURE_MSG_AUTH |	     \
	 CEPH_FEATURE_CRUSH_TUNABLES2 |	     \
	 CEPH_FEATURE_CREATEPOOLID |	     \
	 CEPH_FEATURE_REPLY_CREATE_INODE |   \
	 CEPH_FEATURE_MDSENC |			\
	 CEPH_FEATURE_OSDHASHPSPOOL |       \
	 CEPH_FEATURE_NEW_OSDOP_ENCODING |        \
         CEPH_FEATURE_NEW_OSDOPREPLY_ENCODING | \
	 CEPH_FEATURE_MON_SINGLE_PAXOS |    \
	 CEPH_FEATURE_OSD_CACHEPOOL |	    \
	 CEPH_FEATURE_CRUSH_V2 |	    \
	 CEPH_FEATURE_EXPORT_PEER |	    \
         CEPH_FEATURE_OSD_ERASURE_CODES |   \
	 CEPH_FEATURE_OSDMAP_ENC |          \
	 CEPH_FEATURE_MDS_INLINE_DATA |	    \
	 CEPH_FEATURE_CRUSH_TUNABLES3 |	    \
	 CEPH_FEATURE_OSD_PRIMARY_AFFINITY |	\
	 CEPH_FEATURE_MSGR_KEEPALIVE2 |	\
	 CEPH_FEATURE_OSD_POOLRESEND |	\
         CEPH_FEATURE_ERASURE_CODE_PLUGINS_V2 |   \
	 CEPH_FEATURE_OSD_FADVISE_FLAGS |     \
	 CEPH_FEATURE_MDS_QUOTA | \
         CEPH_FEATURE_CRUSH_V4 |	     \
	 CEPH_FEATURE_MON_METADATA |			 \
	 CEPH_FEATURE_OSD_BITWISE_HOBJ_SORT |		 \
         CEPH_FEATURE_ERASURE_CODE_PLUGINS_V3 |   \
         CEPH_FEATURE_OSD_PROXY_WRITE_FEATURES |         \
	 CEPH_FEATURE_OSD_HITSET_GMT |			 \
	 CEPH_FEATURE_HAMMER_0_94_4 |		 \
	 CEPH_FEATURE_MON_STATEFUL_SUB |	 \
	 CEPH_FEATURE_MON_ROUTE_OSDMAP |	 \
	 CEPH_FEATURE_CRUSH_TUNABLES5 |	    \
	 CEPH_FEATURE_SERVER_JEWEL |  \
	 CEPH_FEATURE_FS_FILE_LAYOUT_V2 |		 \
	 CEPH_FEATURE_SERVER_KRAKEN |	\
	 CEPH_FEATURE_FS_BTIME |			 \
	 CEPH_FEATURE_FS_CHANGE_ATTR |			 \
	 CEPH_FEATURE_MSG_ADDR2 | \
	 CEPH_FEATURE_SERVER_LUMINOUS |		\
	 CEPH_FEATURE_RESEND_ON_SPLIT |		\
	 CEPH_FEATURE_RADOS_BACKOFF |		\
	 CEPH_FEATURE_OSD_RECOVERY_DELETES | \
	 CEPH_FEATURE_CEPHX_V2 | \
	 CEPH_FEATURE_OSD_PGLOG_HARDLIMIT | \
	 0ULL)
#define CEPH_FEATURES_SUPPORTED_DEFAULT  CEPH_FEATURES_ALL
/*
 * crush related features
 */
#define CEPH_FEATURES_CRUSH			\
	(CEPH_FEATURE_CRUSH_TUNABLES |		\
	 CEPH_FEATURE_CRUSH_TUNABLES2 |		\
	 CEPH_FEATURE_CRUSH_TUNABLES3 |		\
	 CEPH_FEATURE_CRUSH_TUNABLES5 |		\
	 CEPH_FEATURE_CRUSH_V2 |		\
	 CEPH_FEATURE_CRUSH_V4 |		\
	 CEPH_FEATUREMASK_CRUSH_CHOOSE_ARGS)
/*
 * make sure we don't try to use the reserved features
 */
#define CEPH_STATIC_ASSERT(x) (void)(sizeof(int[((x)==0) ? -1 : 0]))
static inline void ____build_time_check_for_reserved_bits(void)
{
    CEPH_STATIC_ASSERT((CEPH_FEATURES_ALL &
                        (CEPH_FEATURE_RESERVED |
                         DEPRECATED_CEPH_FEATURE_RESERVED_BROKEN)) == 0);
}

#endif
