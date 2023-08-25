#ifndef __CEPH_FEATURES
#define __CEPH_FEATURES

#include "sys/types.h"

/*
 * Each time we reclaim bits for reuse we need to specify another
 * bitmask that, if all bits are set, indicates we have the new
 * incarnation of that feature.  Base case is 1 (first use)
 */
#define CEPH_FEATURE_INCARNATION_1 (0ull)
#define CEPH_FEATURE_INCARNATION_2 (1ull<<57)   // SERVER_JEWEL
#define CEPH_FEATURE_INCARNATION_3 ((1ull<<57)|(1ull<<28))  // SERVER_MIMIC

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
 * For feature bits used *only* on the server-side:
 *
 *  - In the first phase we indicate that a feature is DEPRECATED as of
 *    a particular release.  This is the first major release X (say,
 *    mimic) that does not depend on its peers advertising the feature.
 *    That is, it safely assumes its peers all have the feature.  We
 *    indicate this with the DEPRECATED macro.  For example,
 *
 *      DEFINE_CEPH_FEATURE_DEPRECATED( 2, 1, MON_METADATA, MIMIC)
 *
 *    because 13.2.z (mimic) did not care if its peers advertised this
 *    feature bit.
 *
 *  - In the second phase we stop advertising the the bit and call it
 *    RETIRED.  This can normally be done 2 major releases
 *    following the one in which we marked the feature DEPRECATED.  In
 *    the above example, for 15.0.z (octopus) we can say:
 *
 *      DEFINE_CEPH_FEATURE_RETIRED( 2, 1, MON_METADATA, MIMIC, OCTOPUS)
 *
 *  - The bit can be reused in the next release that will never talk to
 *    a pre-octopus daemon (13 mimic or 14 nautlius) that advertises the
 *    bit: in this case, the 16.y.z (P-release).
 *
 * This ensures that no two versions who have different meanings for
 * the bit ever speak to each other.
 */

/*
 * Notes on the kernel client:
 *
 * - "X" means that the feature bit has been advertised and supported
 *   since kernel X
 *
 * - "X req" means that the feature bit has been advertised and required
 *   since kernel X
 *
 * The remaining feature bits are not and have never been used by the
 * kernel client.
 */

DEFINE_CEPH_FEATURE(0, 1, UID)
    DEFINE_CEPH_FEATURE(1, 1, NOSRCADDR)// 2.6.35 req
    DEFINE_CEPH_FEATURE_RETIRED(2, 1, MONCLOCKCHECK, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(2, 3, SERVER_NAUTILUS)
    DEFINE_CEPH_FEATURE(3, 1, FLOCK)// 2.6.36
    DEFINE_CEPH_FEATURE(4, 1, SUBSCRIBE2)
                                        // 4.6 req
    DEFINE_CEPH_FEATURE(5, 1, MONNAMES)
    DEFINE_CEPH_FEATURE(6, 1, RECONNECT_SEQ)// 3.10 req
    DEFINE_CEPH_FEATURE(7, 1, DIRLAYOUTHASH)// 2.6.38
    DEFINE_CEPH_FEATURE(8, 1, OBJECTLOCATOR)
    DEFINE_CEPH_FEATURE(9, 1, PGID64)
                                    // 3.9 req
    DEFINE_CEPH_FEATURE(10, 1, INCSUBOSDMAP)
    DEFINE_CEPH_FEATURE(11, 1, PGPOOL3)
                                    // 3.9 req
    DEFINE_CEPH_FEATURE(12, 1, OSDREPLYMUX)
    DEFINE_CEPH_FEATURE(13, 1, OSDENC)
                                    // 3.9 req
    DEFINE_CEPH_FEATURE_RETIRED(14, 1, OMAP, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(14, 2, SERVER_KRAKEN)
    DEFINE_CEPH_FEATURE(15, 1, MONENC)
    DEFINE_CEPH_FEATURE_RETIRED(16, 1, QUERY_T, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(16, 3, SERVER_OCTOPUS)
    DEFINE_CEPH_FEATURE(16, 3, OSD_REPOP_MLCOD)
    DEFINE_CEPH_FEATURE_RETIRED(17, 1, INDEP_PG_MAP, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(17, 3, OS_PERF_STAT_NS)
    DEFINE_CEPH_FEATURE(18, 1, CRUSH_TUNABLES)
                                            // 3.6
    DEFINE_CEPH_FEATURE_RETIRED(19, 1, CHUNKY_SCRUB, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(19, 2, OSD_PGLOG_HARDLIMIT)
    DEFINE_CEPH_FEATURE_RETIRED(20, 1, MON_NULLROUTE, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(20, 3, SERVER_PACIFIC)
    DEFINE_CEPH_FEATURE_RETIRED(21, 1, MON_GV, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(21, 2, SERVER_LUMINOUS)
                                            // 4.13
    DEFINE_CEPH_FEATURE(21, 2, RESEND_ON_SPLIT)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, RADOS_BACKOFF)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, OSDMAP_PG_UPMAP)
                                            // overlap
    DEFINE_CEPH_FEATURE(21, 2, CRUSH_CHOOSE_ARGS)
                                                // overlap
    DEFINE_CEPH_FEATURE_RETIRED(22, 1, BACKFILL_RESERVATION, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(22, 2, OSD_FIXED_COLLECTION_LIST)
    DEFINE_CEPH_FEATURE(23, 1, MSG_AUTH)// 3.19 req (unless nocephx_require_signatures)
    DEFINE_CEPH_FEATURE_RETIRED(24, 1, RECOVERY_RESERVATION, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(24, 2, RECOVERY_RESERVATION_2)
    DEFINE_CEPH_FEATURE(25, 1, CRUSH_TUNABLES2)
                                            // 3.9
    DEFINE_CEPH_FEATURE(26, 1, CREATEPOOLID)
    DEFINE_CEPH_FEATURE(27, 1, REPLY_CREATE_INODE)
                                                // 3.9
    DEFINE_CEPH_FEATURE_RETIRED(28, 1, OSD_HBMSGS, HAMMER, JEWEL)
    DEFINE_CEPH_FEATURE(28, 2, SERVER_MIMIC)
    DEFINE_CEPH_FEATURE(29, 1, MDSENC)
                                    // 4.7
    DEFINE_CEPH_FEATURE(30, 1, OSDHASHPSPOOL)
                                            // 3.9
    DEFINE_CEPH_FEATURE_RETIRED(31, 1, MON_SINGLE_PAXOS, NAUTILUS, PACIFIC)
    DEFINE_CEPH_FEATURE(31, 3, SERVER_REEF)
    DEFINE_CEPH_FEATURE_RETIRED(32, 1, OSD_SNAPMAPPER, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(32, 3, STRETCH_MODE)
    DEFINE_CEPH_FEATURE_RETIRED(33, 1, MON_SCRUB, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(33, 3, SERVER_QUINCY)
    DEFINE_CEPH_FEATURE_RETIRED(34, 1, OSD_PACKED_RECOVERY, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE(34, 3, RANGE_BLOCKLIST)
    DEFINE_CEPH_FEATURE(35, 1, OSD_CACHEPOOL)
                                            // 3.14
    DEFINE_CEPH_FEATURE(36, 1, CRUSH_V2)// 3.14
    DEFINE_CEPH_FEATURE(37, 1, EXPORT_PEER)
                                        // 3.14
    DEFINE_CEPH_FEATURE_RETIRED(38, 1, OSD_ERASURE_CODES, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE(39, 1, OSDMAP_ENC)
                                        // 3.15
    DEFINE_CEPH_FEATURE(40, 1, MDS_INLINE_DATA)
                                            // 3.19
    DEFINE_CEPH_FEATURE(41, 1, CRUSH_TUNABLES3)
                                            // 3.15
    DEFINE_CEPH_FEATURE(41, 1, OSD_PRIMARY_AFFINITY)// overlap
    DEFINE_CEPH_FEATURE(42, 1, MSGR_KEEPALIVE2)
                                            // 4.3 (for consistency)
    DEFINE_CEPH_FEATURE(43, 1, OSD_POOLRESEND)
                                            // 4.13
    DEFINE_CEPH_FEATURE_RETIRED(44, 1, ERASURE_CODE_PLUGINS_V2, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(45, 1, OSD_SET_ALLOC_HINT, JEWEL, LUMINOUS)
// available
    DEFINE_CEPH_FEATURE(46, 1, OSD_FADVISE_FLAGS)
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_REPOP, JEWEL, LUMINOUS)
                                                                // overlap
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_OBJECT_DIGEST, JEWEL, LUMINOUS)
                                                                        // overlap
    DEFINE_CEPH_FEATURE_RETIRED(46, 1, OSD_TRANSACTION_MAY_LAYOUT, JEWEL, LUMINOUS)
                                                                                // overlap
    DEFINE_CEPH_FEATURE(47, 1, MDS_QUOTA)
                                        // 4.17
    DEFINE_CEPH_FEATURE(48, 1, CRUSH_V4)// 4.1
    DEFINE_CEPH_FEATURE_RETIRED(49, 1, OSD_MIN_SIZE_RECOVERY, JEWEL, LUMINOUS)
    DEFINE_CEPH_FEATURE_RETIRED(49, 1, OSD_PROXY_FEATURES, JEWEL, LUMINOUS)
                                                                        // overlap
// available
    DEFINE_CEPH_FEATURE_RETIRED(50, 1, MON_METADATA, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(51, 1, OSD_BITWISE_HOBJ_SORT, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(52, 1, OSD_PROXY_WRITE_FEATURES, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(53, 1, ERASURE_CODE_PLUGINS_V3, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(54, 1, OSD_HITSET_GMT, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE_RETIRED(55, 1, HAMMER_0_94_4, MIMIC, OCTOPUS)
// available
    DEFINE_CEPH_FEATURE(56, 1, NEW_OSDOP_ENCODING)
                                                // 4.13 (for pg_pool_t >= v25)
    DEFINE_CEPH_FEATURE(57, 1, MON_STATEFUL_SUB)// 4.13
    DEFINE_CEPH_FEATURE_RETIRED(57, 1, MON_ROUTE_OSDMAP, MIMIC, OCTOPUS)// overlap
    DEFINE_CEPH_FEATURE(57, 1, SERVER_JEWEL)// overlap
    DEFINE_CEPH_FEATURE(58, 1, CRUSH_TUNABLES5)
                                            // 4.5
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
    DEFINE_CEPH_FEATURE(61, 1, CEPHX_V2)// 4.19, *do not share this bit*
    DEFINE_CEPH_FEATURE(62, 1, RESERVED)// do not use; used as a sentinel
    DEFINE_CEPH_FEATURE_RETIRED(63, 1, RESERVED_BROKEN, LUMINOUS, QUINCY)
                                                                        // client-facing
// available
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
	 CEPH_FEATURE_OSD_CACHEPOOL |	    \
	 CEPH_FEATURE_CRUSH_V2 |	    \
	 CEPH_FEATURE_EXPORT_PEER |	    \
	 CEPH_FEATURE_OSDMAP_ENC |          \
	 CEPH_FEATURE_MDS_INLINE_DATA |	    \
	 CEPH_FEATURE_CRUSH_TUNABLES3 |	    \
	 CEPH_FEATURE_OSD_PRIMARY_AFFINITY |	\
	 CEPH_FEATURE_MSGR_KEEPALIVE2 |	\
	 CEPH_FEATURE_OSD_POOLRESEND |	\
	 CEPH_FEATURE_OSD_FADVISE_FLAGS |     \
	 CEPH_FEATURE_MDS_QUOTA | \
         CEPH_FEATURE_CRUSH_V4 |	     \
	 CEPH_FEATURE_MON_STATEFUL_SUB |	 \
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
	 CEPH_FEATURE_OSD_RECOVERY_DELETES |	\
	 CEPH_FEATURE_SERVER_MIMIC |		\
	 CEPH_FEATURE_RECOVERY_RESERVATION_2 |	\
	 CEPH_FEATURE_SERVER_NAUTILUS |		\
	 CEPH_FEATURE_CEPHX_V2 | \
	 CEPH_FEATURE_OSD_PGLOG_HARDLIMIT | \
	 CEPH_FEATUREMASK_SERVER_OCTOPUS | \
	 CEPH_FEATUREMASK_STRETCH_MODE | \
	 CEPH_FEATUREMASK_OSD_REPOP_MLCOD | \
	 CEPH_FEATUREMASK_SERVER_PACIFIC | \
	 CEPH_FEATURE_OSD_FIXED_COLLECTION_LIST | \
	 CEPH_FEATUREMASK_SERVER_QUINCY | \
	 CEPH_FEATURE_RANGE_BLOCKLIST | \
	 CEPH_FEATUREMASK_SERVER_REEF | \
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
    CEPH_STATIC_ASSERT((CEPH_FEATURES_ALL & CEPH_FEATURE_RESERVED) == 0);
}

#endif
