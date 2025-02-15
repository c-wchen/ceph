// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#include <iostream>
#include <string.h>

#include "include/types.h"
#include "include/crc32c.h"
#include "include/utime.h"
#include "common/Clock.h"

#include "gtest/gtest.h"

#include "common/sctp_crc32.h"
#include "common/crc32c_intel_baseline.h"
#include "common/crc32c_aarch64.h"

TEST(Crc32c, Small)
{
    const char *a = "foo bar baz";
    const char *b = "whiz bang boom";
    ASSERT_EQ(4119623852u, ceph_crc32c(0, (unsigned char *)a, strlen(a)));
    ASSERT_EQ(881700046u, ceph_crc32c(1234, (unsigned char *)a, strlen(a)));
    ASSERT_EQ(2360230088u, ceph_crc32c(0, (unsigned char *)b, strlen(b)));
    ASSERT_EQ(3743019208u, ceph_crc32c(5678, (unsigned char *)b, strlen(b)));
}

TEST(Crc32c, PartialWord)
{
    const char *a = (const char *)malloc(5);
    const char *b = (const char *)malloc(35);
    memset((void *)a, 1, 5);
    memset((void *)b, 1, 35);
    ASSERT_EQ(2715569182u, ceph_crc32c(0, (unsigned char *)a, 5));
    ASSERT_EQ(440531800u, ceph_crc32c(0, (unsigned char *)b, 35));
    free((void *)a);
    free((void *)b);
}

TEST(Crc32c, Big)
{
    int len = 4096000;
    char *a = (char *)malloc(len);
    memset(a, 1, len);
    ASSERT_EQ(31583199u, ceph_crc32c(0, (unsigned char *)a, len));
    ASSERT_EQ(1400919119u, ceph_crc32c(1234, (unsigned char *)a, len));
    free(a);
}

TEST(Crc32c, Performance)
{
    int len = 1000 * 1024 * 1024;
    char *a = (char *)malloc(len);
    std::cout << "populating large buffer" << std::endl;
    for (int i = 0; i < len; i++) {
        a[i] = i & 0xff;
    }
    std::cout << "calculating crc" << std::endl;

    {
        utime_t start = ceph_clock_now();
        unsigned val = ceph_crc32c(0, (unsigned char *)a, len);
        utime_t end = ceph_clock_now();
        float rate = (float)len / (float)(1024 * 1024) / (float)(end - start);
        std::cout << "best choice = " << rate << " MB/sec" << std::endl;
        ASSERT_EQ(261108528u, val);
    }
    {
        utime_t start = ceph_clock_now();
        unsigned val = ceph_crc32c(0xffffffff, (unsigned char *)a, len);
        utime_t end = ceph_clock_now();
        float rate = (float)len / (float)(1024 * 1024) / (float)(end - start);
        std::cout << "best choice 0xffffffff = " << rate << " MB/sec" << std::endl;
        ASSERT_EQ(3895876243u, val);
    }
    {
        utime_t start = ceph_clock_now();
        unsigned val = ceph_crc32c_sctp(0, (unsigned char *)a, len);
        utime_t end = ceph_clock_now();
        float rate = (float)len / (float)(1024 * 1024) / (float)(end - start);
        std::cout << "sctp = " << rate << " MB/sec" << std::endl;
        ASSERT_EQ(261108528u, val);
    }
    {
        utime_t start = ceph_clock_now();
        unsigned val = ceph_crc32c_intel_baseline(0, (unsigned char *)a, len);
        utime_t end = ceph_clock_now();
        float rate = (float)len / (float)(1024 * 1024) / (float)(end - start);
        std::cout << "intel baseline = " << rate << " MB/sec" << std::endl;
        ASSERT_EQ(261108528u, val);
    }
#if defined(__arm__) || defined(__aarch64__)
    if (ceph_arch_aarch64_crc32) { // Skip if CRC32C instructions are not defined.
        utime_t start = ceph_clock_now();
        unsigned val = ceph_crc32c_aarch64(0, (unsigned char *)a, len);
        utime_t end = ceph_clock_now();
        float rate = (float)len / (float)(1024 * 1024) / (float)(end - start);
        std::cout << "aarch64 = " << rate << " MB/sec" << std::endl;
        ASSERT_EQ(261108528u, val);
    }
#endif
    free(a);
}


static uint32_t crc_check_table[] = {
    0xcfc75c75, 0x7aa1b1a7, 0xd761a4fe, 0xd699eeb6, 0x2a136fff, 0x9782190d, 0xb5017bb0, 0xcffb76a9,
    0xc79d0831, 0x4a5da87e, 0x76fb520c, 0x9e19163d, 0xe8eacd22, 0xefd4319e, 0x1eaa804b, 0x7ff41ccb,
    0x94141dab, 0xb4c2588f, 0x484bf16f, 0x77725048, 0xf27d43ee, 0x3604f655, 0x20bb9b79, 0xd6ee30ba,
    0xf402f02d, 0x59992eec, 0x159c0449, 0xe2d72e60, 0xc519c744, 0xf56f7995, 0x7e40be36, 0x695ccedc,
    0xc95c4ae3, 0xb0d2d6bc, 0x85872e14, 0xea2c01b0, 0xe9b75f1a, 0xebb23ae3, 0x39faee13, 0x313cb413,
    0xe683eb7d, 0xd22e2ae1, 0xf49731dd, 0x897a8e60, 0x923b510e, 0xe0e0f3b, 0x357dd0f, 0x63b7aa7d,
    0x6f5c2a40, 0x46b09a37, 0x80324751, 0x380fd024, 0x78b122c6, 0xb29d1dde, 0x22f19ddc, 0x9d6ee6d6,
    0xfb4e7e1c, 0xb9780044, 0x85feef90, 0x8e4fae11, 0x1a71394a, 0xbe21c888, 0xde2f6f47, 0x93c365f0,
    0xfd1d3814, 0x6e0a23df, 0xc6739c17, 0x2d48520d, 0x3357e475, 0x5d57058a, 0x22c4b9f7, 0x5a498b58,
    0x7bed8ddb, 0xcf1eb035, 0x2094f389, 0xb6a7c977, 0x289d29e2, 0x498d5b7, 0x8db77420, 0x85300608,
    0x5d1c04c4, 0x5acfee62, 0x99ad4694, 0x799f9833, 0x50e76ce1, 0x72dc498, 0x70a393be, 0x905a364d,
    0x1af66b95, 0x5b3eed9e, 0xa3e4da14, 0xc720fece, 0x555200df, 0x169fd3e0, 0x531c18c0, 0x6f9b6092,
    0x6d16638b, 0x5a8c8b6a, 0x818ebab2, 0xd75b10bb, 0xcaa01bfa, 0x67377804, 0xf8a085ae, 0xfc7d88b8,
    0x5e2debc1, 0x9759cb1f, 0x24c39b63, 0x210afbba, 0x22f7c6f7, 0xa8f8dc11, 0xf1d4550c, 0x1d2b1e47,
    0x59a44605, 0x25402e97, 0x18401ea, 0xb1884203, 0xd6ef715, 0x1797b686, 0x9e7f5aa7, 0x30795e88,
    0xb280b636, 0x77258b7d, 0x5f8dbff3, 0xbb57ea03, 0xa2c35cce, 0x1acce538, 0xa50be97a, 0x417f4b57,
    0x6d94792f, 0x4bb6fb34, 0x3787440c, 0x9a77b0b9, 0x67ece3d0, 0x5a8450fe, 0x8e66f55b, 0x3cefce93,
    0xf7ca60ab, 0xce7cd3b7, 0x97976493, 0xa05632f8, 0x77ac4546, 0xed24c705, 0x92a2f20, 0xc0b1cc9,
    0x831ae4e1, 0x5b3f28b1, 0xee6fca02, 0x74acc743, 0xaf40043f, 0x5f21e837, 0x9e168fc0, 0x64e28de,
    0x88ae891d, 0xac2e4ff5, 0xaeaf9c27, 0x158a2d3, 0x5226fb01, 0x9bf56ae1, 0xe4a2dd8d, 0x2599d6de,
    0xe798b5ee, 0x39efe57a, 0xbb9965c7, 0x4516fde0, 0xa41831f5, 0xd7cd0797, 0xd07b7d5c, 0xb330d048,
    0x3a47e35d, 0x87dd39e5, 0xa806fb31, 0xad228dd, 0xcc390816, 0x9237a4de, 0x8dfe1c20, 0x304f6bc,
    0x3ad98572, 0xec13f349, 0x4e5278d7, 0x784c4bf4, 0x7b93cb23, 0xa18c87ae, 0x84ff79dd, 0x8e95061d,
    0xd972f4d4, 0x4ad50380, 0x23cbc187, 0x7fa7f22c, 0x6062c18e, 0x42381901, 0x10cf51d9, 0x674e22a4,
    0x28a63445, 0x6fc1b591, 0xa4dc117a, 0x744a00d0, 0x8a5470ea, 0x9539c6a7, 0xc961a584, 0x22f81498,
    0xae299e51, 0x5653fcd3, 0x7bfa474f, 0x7f502c42, 0xfb41c744, 0xd478fb95, 0x7b676978, 0xb22f5610,
    0xbcbe730c, 0x70ff5773, 0xde990b63, 0xebcbf9d5, 0x2d029133, 0xf39513e1, 0x56229640, 0x660529e5,
    0x3b90bdf8, 0xc9822978, 0x4e3daab1, 0x2e43ce72, 0x572bb6ff, 0xdc4b17bd, 0x6c290d46, 0x7d9644ca,
    0x7652fd89, 0x66d72059, 0x521e93d4, 0xd626ff95, 0xdc4eb57e, 0xb0b3307c, 0x409adbed, 0x49ae2d28,
    0x8edd249a, 0x8e4fb6ec, 0x5a191fbf, 0xe1751948, 0xb4ae5d00, 0xabeb1bdd, 0xbe204b60, 0xbc97aad4,
    0xb8cb5915, 0x54f33261, 0xc5d83b28, 0x99d0d099, 0xfb06f8b2, 0x57305f66, 0xf9fde17b, 0x192f143c,
    0xcc3c58fd, 0x36e2e420, 0x17118208, 0xcac7e42a, 0xb45ad63d, 0x8ad5e475, 0xb7a3bc1e, 0xe03e64ad,
    0x2c197d77, 0x1a0ff1fe, 0xbcd443fb, 0x7589393a, 0xd66b1f67, 0xdddf0a66, 0x4750b7c7, 0xc62a79db,
    0xcf02a0d3, 0xb4012205, 0x9733d16c, 0x9a29cff8, 0xdd3d6427, 0x15c0273a, 0x97b289b, 0x358ff573,
    0x73a9ceb7, 0xc3788b1a, 0xda7a5155, 0x2990a31, 0x9fa4705, 0x5eb4e2e2, 0x98465bb2, 0x74a17883,
    0xe87df542, 0xe20f22f1, 0x48ffd67e, 0xc94fab5f, 0x9eb431d2, 0xffd673cb, 0xc374dc18, 0xa542fbf7,
    0xb8fea538, 0x43f5431f, 0xcbe3fb7d, 0x2734e0e4, 0x5cb05a8, 0xd00fcf47, 0x248dbbae, 0x47d4de6c,
    0xecc97151, 0xca8c379b, 0x49049fd, 0xeb2acd18, 0xab178ac, 0xc98ab95d, 0xb9e0be20, 0x36664a13,
    0x95d81459, 0xb54973a9, 0x27f9579c, 0xa24fb6df, 0x3f6f8cea, 0xe11efdd7, 0x68166281, 0x586e0a6,
    0x5fad7b57, 0xd58f50ad, 0x6e0d3be8, 0x27a00831, 0x543b3761, 0x96c862fb, 0xa823ed4f, 0xf6043f37,
    0x980703eb, 0xf5e69514, 0x42a2082, 0x495732a2, 0x793eea23, 0x6a6a17fb, 0x77d75dc5, 0xb3320ec4,
    0x10d4d01e, 0xa17508a6, 0x6d578355, 0xd136c445, 0xafa6acc6, 0x2307831d, 0x5bf345fd, 0xb9a04582,
    0x2627a686, 0xf6f4ce3b, 0xd0ac868f, 0x78d6bdb3, 0xfe42945a, 0x8b06cbf3, 0x2b169628, 0xf072b8b7,
    0x8652a0ca, 0x3f52fc42, 0xa0415b9a, 0x16e99341, 0x7394e9c7, 0xac92956c, 0x7bff7137, 0xb0e8ea5c,
    0x42d8c22, 0x4318a18, 0x42097180, 0x57d17dba, 0xb1f7a567, 0x55186d60, 0xf527e0ca, 0xd58b0b48,
    0x31d9155b, 0xd5fd0441, 0x6024d751, 0xe14d03c3, 0xba032e1c, 0xd6d89ae7, 0x54f1967a, 0xe401c200,
    0x8ee973ff, 0x3d24277e, 0xab394cbf, 0xe3b39762, 0x87f43766, 0xe4c2bdff, 0x1234c0d7, 0x8ef3e1bd,
    0xeeb00f61, 0x15d17d4b, 0x7d40ac8d, 0xada8606f, 0x7ba5e3a1, 0xcf487cf9, 0x98dda708, 0x6d7c9bea,
    0xaecb321c, 0x9f7801b2, 0x53340341, 0x7ae27355, 0xbf859829, 0xa36a00b, 0x99339435, 0x8342d1e,
    0x4ab4d7ea, 0x862d01cd, 0x7f94fbee, 0xe329a5a3, 0x2cb7ba81, 0x50bae57a, 0x5bbd65cf, 0xf06f60e4,
    0x569ad444, 0xfa0c16c, 0xb8c2b472, 0x3ea64ea1, 0xc6dc4c18, 0x5d6d654a, 0x5369a931, 0x2163bf7f,
    0xe45bd590, 0xcc826d18, 0xb4ce22f6, 0x200f7232, 0x5f2f869c, 0xffd5cc17, 0x1a578942, 0x930da3ea,
    0x216377f, 0x9f07a04b, 0x1f2a777c, 0x13c95089, 0x8a64d032, 0x1eecb206, 0xc537dc4, 0x319f9ac8,
    0xe2131194, 0x25d2f716, 0xa27f471a, 0xf6434ce2, 0xd51a10b9, 0x4e28a61, 0x647c888a, 0xb383d2ff,
    0x93aa0d0d, 0x670d1317, 0x607f36e2, 0x73e01833, 0x2bd372b0, 0x86404ad2, 0x253d5cc4, 0x1348811c,
    0x8756f2d5, 0xe1e55a59, 0x5247e2d1, 0x798ab6b, 0x181bbc57, 0xb9ea36e0, 0x66081c68, 0x9bf0bad7,
    0x892b1a6, 0x8a6a9aed, 0xda955d0d, 0x170e5128, 0x81733d84, 0x6d9f6b10, 0xd60046fd, 0x7e401823,
    0xf9904ce6, 0xaa765665, 0x2fd5c4ee, 0xbb9c1580, 0x391dac53, 0xbffe4270, 0x866c30b1, 0xd629f22,
    0x1ee5bfee, 0x5af91c96, 0x96b613bf, 0xa65204c9, 0x9b8cb68c, 0xd08b37c1, 0xf1863f8f, 0x1e4c844a,
    0x876abd30, 0x70c07eff, 0x63d8e875, 0x74351f92, 0xffe7712d, 0x58c0171d, 0x7b826b99, 0xc09afc78,
    0xd81d3065, 0xccced8b1, 0xe258b1c9, 0x5659d6b, 0x1959c406, 0x53bd05e6, 0xa32f784b, 0x33351e4b,
    0xb6b9d769, 0x59e5802c, 0x118c7ff7, 0x46326e0b, 0xa7376fbe, 0x7218aed1, 0x28c8f707, 0x44610a2f,
    0xf8eafea1, 0xfe36fdae, 0xb4b546f1, 0x2e27ce89, 0xc1fde8a0, 0x99f2f157, 0xfde687a1, 0x40a75f50,
    0x6c653330, 0xf3e38821, 0xf4663e43, 0x2f7e801e, 0xfca360af, 0x53cd3c59, 0xd20da292, 0x812a0241
};

TEST(Crc32c, Range)
{
    int len = sizeof(crc_check_table) / sizeof(crc_check_table[0]);
    unsigned char *b = (unsigned char *)malloc(len);
    memset(b, 1, len);
    uint32_t crc = 0;
    uint32_t *check = crc_check_table;
    for (int i = 0 ; i < len; i++, check++) {
        crc = ceph_crc32c(crc, b + i, len - i);
        ASSERT_EQ(crc, *check);
    }
    free(b);
}

static uint32_t crc_zero_check_table[] = {
    0xbd6f81f8, 0x6213374d, 0x72952aeb, 0x8ecb5e52, 0xa04914b4, 0xaf3aaea9, 0xb88d42d6, 0x81797724,
    0xc0022634, 0x4dbf46a4, 0xc7813aa, 0x172150e0, 0x13d8d958, 0x339fd933, 0xd9e725f4, 0x20b65b14,
    0x349c971c, 0x7f812818, 0x5228e357, 0x811f231f, 0xe4bdaeee, 0xcdd22442, 0x26ae3c58, 0xf9628c5e,
    0x8118e80b, 0xca0ea635, 0xc5028f6d, 0xbd2270, 0x4d9171a3, 0xe810af42, 0x904c7218, 0xdc62c735,
    0x3c8b3748, 0x7cae4eef, 0xed170242, 0xdc0a6a28, 0x4afb0591, 0x4643748a, 0xad28d5b, 0xeb2d60d3,
    0x479d21a9, 0x2a0916c1, 0x144cd9fb, 0x2498ba7a, 0x196489f, 0x330bb594, 0x5abe491d, 0x195658fe,
    0xc6ef898f, 0x94b251a1, 0x4f968332, 0xfbf5f29d, 0x7b4828ce, 0x3af20a6f, 0x653a721f, 0x6d92d018,
    0xf43ca065, 0xf55da16e, 0x94af47c6, 0xf08abdc, 0x11344631, 0xb249e575, 0x1f9f992b, 0xfdb6f490,
    0xbd40d84b, 0x945c69e1, 0x2a94e2e3, 0xe5aa9b91, 0x89cebb57, 0x175a3097, 0x502b7d34, 0x174f2c92,
    0x2a8f01c0, 0x645a2db8, 0x9e9a4a8, 0x13adac02, 0x2759a24b, 0x8bfcb972, 0xfa1edbfe, 0x5a88365e,
    0x5c107fd9, 0x91ac73a8, 0xbd40e99e, 0x513011ca, 0x97bd2841, 0x336c1c4e, 0x4e88563e, 0x6948813e,
    0x96e1cbee, 0x64b2faa5, 0x9671e44, 0x7d492fcb, 0x3539d74a, 0xcbe26ad7, 0x6106e673, 0x162115d,
    0x8534e6a6, 0xd28a1ea0, 0xf73beb20, 0x481bdbae, 0xcd12e442, 0x8ab52843, 0x171d72c4, 0xd97cb216,
    0x60fa0ecf, 0x74336ebb, 0x4d67fd86, 0x9393e96a, 0x63670234, 0x3f2a31da, 0x4036c11f, 0x55cc2ceb,
    0xf75b27dc, 0xcabdca83, 0x80699d1a, 0x228c13a1, 0x5ea7f8a9, 0xc7631f40, 0x710b867a, 0xaa6e67b9,
    0x27444987, 0xd693cd2a, 0xc4e21e0c, 0xd340e1cb, 0x2a2a346f, 0xac55e843, 0xfcd2750c, 0x4529a016,
    0x7ac5802, 0xa2eb291f, 0x4a0fb9ea, 0x6a58a9a0, 0x51f56797, 0xda595134, 0x267aba96, 0x8ba80ee,
    0x4474659e, 0x2b7bacb, 0xba524d37, 0xb60981bb, 0x5fd43811, 0xca41594a, 0x98ace58, 0x3fc5b984,
    0x6a290b91, 0x6576108a, 0x8c33c85e, 0x52622407, 0x99cf8723, 0x68198dc8, 0x18b7341d, 0x540fc0f9,
    0xf4a7b6f6, 0xfade9dfa, 0x725471ca, 0x5c160723, 0x5f33b243, 0xecec5d09, 0x6f520abb, 0x139c7bca,
    0x58349acb, 0x1fccef32, 0x1d01aa0f, 0x3f477a65, 0xebf55472, 0xde9ae082, 0x76d3119e, 0x937e2708,
    0xba565506, 0xbe820951, 0xc1f336fa, 0xfc41afb6, 0x4ef12d88, 0xd6f6d4f, 0xb33fb3fe, 0x9c6d1ae,
    0x24ae1c29, 0xf9ae57f7, 0x51d1e4c9, 0x86dc73fc, 0x54b7bf38, 0x688a141c, 0x91d4ea7a, 0xd57a0fd0,
    0x5cdcd16f, 0xc59c135a, 0x5bb003b5, 0x730b52f3, 0xc1dc5b1e, 0xf083f53, 0x8159e7c8, 0xf396d2e3,
    0x1c7f18ec, 0x5bedc75e, 0x2f11fbfd, 0xb4437094, 0x77c55e3, 0x1d8636e1, 0x159bf2f, 0x6cbabf5b,
    0xf4d005bc, 0x39f0bc55, 0x3d525f54, 0x8422e29d, 0xfb8a413d, 0x66e78593, 0xa0e14663, 0x880b8fa1,
    0x24b53713, 0x12105ff3, 0xa94dd90f, 0x3ff981bc, 0xaf2366af, 0x8e98710, 0x48eb45c6, 0xbc3aee53,
    0x6933d852, 0xe236cfd3, 0x3e6c50af, 0xe309e3fd, 0x452eac88, 0x725bf633, 0xbe89339a, 0x4b54eff7,
    0xa57e392f, 0x6ee15bef, 0x67630f96, 0x31656c71, 0x77fc97f0, 0x1d29682f, 0xa4b0fc5d, 0xb3fd0ee1,
    0x9d10aa57, 0xf104e21, 0x478b5f75, 0xaf1ca64b, 0x13e8a297, 0x21caa105, 0xb3cb8e9d, 0xd4536cb,
    0x425bdfce, 0x90462d05, 0x8cace1cf, 0xc0ab7293, 0xbcf288cb, 0x5edcdc11, 0x4ec8b5e0, 0x42738654,
    0x4ba49663, 0x2b264337, 0x41d1a5ce, 0xaa8acb92, 0xe79714aa, 0x86695e7c, 0x1330c69a, 0xe0c6485f,
    0xb038b81a, 0x6f823a85, 0x4eeff0e4, 0x7355d58f, 0x7cc87e83, 0xe23e4619, 0x7093faa0, 0x7328cb2f,
    0x7856db5e, 0xbc38d892, 0x1e4307c8, 0x347997e1, 0xb26958, 0x997ddf1e, 0x58dc72e3, 0x4b6e9a77,
    0x49eb9924, 0x36d555db, 0x59456efd, 0x904bd6d2, 0xd932837d, 0xf96a24ec, 0x525aa449, 0x5fd05bc7,
    0x84778138, 0xd869bfe1, 0xe6bbd546, 0x2f796af4, 0xbaab980f, 0x7f18a176, 0x3a8e00d9, 0xb589ea81,
    0x77920ee3, 0xc6730dbc, 0x8a5df534, 0xb7df9a12, 0xdc93009c, 0x215b885, 0x309104b, 0xf47e380b,
    0x23f6cdef, 0xe112a923, 0x83686f38, 0xde2c7871, 0x9f728ec7, 0xeaae7af6, 0x6d7b7b0a, 0xaf0cde04,
    0xfcb51a1f, 0xf0cd53cf, 0x7aa5556a, 0xa64ccf7e, 0x854c2084, 0xc493ddd4, 0x92684099, 0x913beb92,
    0xe4067ea8, 0x9557605a, 0x934346d6, 0x23a3a7c7, 0x588b2805, 0xe1e755ae, 0xe4c05e84, 0x8e09d0f3,
    0x1343a510, 0x6175c2c3, 0x39bb7947, 0x4a1b9b6b, 0xf0e373da, 0xe7b9a201, 0x24b7a392, 0x91a27584,
    0x9ac3a10f, 0x91fc9314, 0xc495d878, 0x3fcbc776, 0x7f81d6da, 0x973edb2f, 0xa9d731c6, 0x2dc022a8,
    0xa066c881, 0x7e082dff, 0xa1ff394d, 0x1cb0c2bb, 0xef87a116, 0x5179810b, 0xa1594c92, 0xe291e155,
    0x3578c98f, 0xb801f82c, 0xa1778ad9, 0xbdd48b76, 0x74f1ce54, 0x46b8de63, 0x3861112c, 0x46a8920f,
    0x3e1075e7, 0x220a49dd, 0x3e51d6d2, 0xbf1f22cd, 0x5d1490c5, 0x7f1e05f5, 0xa0c1691d, 0x9108debf,
    0xe69899b, 0xe771d8b6, 0x878c92c1, 0x973e37c0, 0x833c4c25, 0xcffe7b03, 0x92e0921e, 0xccee9836,
    0xa9739832, 0xc774f2f2, 0xf34f9467, 0x608cef83, 0x97a584d2, 0xf5218c9, 0x73eb9524, 0xb3fb4870,
    0x53296e3d, 0x8836f46f, 0x9d6a40b0, 0x789b5e91, 0x62a915ba, 0x32c02d74, 0xc93de2f3, 0xefa67fc7,
    0x169ee4f1, 0x72bbbe9e, 0x49357cf2, 0x219207bf, 0x12516225, 0x182df160, 0x230c9a3f, 0x137a8497,
    0xa429ad30, 0x4aa66f88, 0x40319931, 0xfa241c42, 0x1e5189ec, 0xca693ada, 0xe7b923f4, 0xff546a06,
    0xf01103c2, 0x99875a32, 0x4bbf55a9, 0x48abdf3e, 0x85eb3dec, 0x2d009057, 0x14c2a682, 0xfabe68af,
    0x96a31fa6, 0xf52f4686, 0x73f72b61, 0x92f39e13, 0x66794863, 0x7ca4c2aa, 0x37a2fe39, 0x33be288a,
    0x1ff9a59c, 0xd65e667, 0x5d7c9332, 0x8a6a2d8b, 0x37ec2d3b, 0x9f935ab9, 0x67fcd589, 0x48a09508,
    0xc446e984, 0x58f69202, 0x968dfbbb, 0xc93d7626, 0x82344e, 0xf1d930a4, 0xcc3acdde, 0x20cf92bf,
    0x94b7616d, 0xb0e45050, 0xdc36c072, 0x74cba0, 0x6478300a, 0x27803b97, 0xb7b2ebd0, 0xb3a691e,
    0x35c2f261, 0x3fcff45a, 0x3e4b7b93, 0x86b680bd, 0x720333ce, 0x67f933ca, 0xb10256de, 0xe939bb3f,
    0xb540a02f, 0x39a8b8e4, 0xb6a63aa5, 0x5e1d56ee, 0xa415a16, 0xcb5753d, 0x17fabd19, 0x90eac10d,
    0x2308857d, 0xb8f6224c, 0x71790390, 0x18749d48, 0xed778f1b, 0x69f0e17c, 0xbd622f4, 0x52c3a79e,
    0x9697bf51, 0xa768755c, 0x9fe860ea, 0xa852b0ac, 0x9549ec64, 0x8669c603, 0x120e289c, 0x3f0520f5,
    0x9b15884, 0x2d06fa7f, 0x767b12f6, 0xcb232dd6, 0x4e2b4590, 0x97821835, 0x4506a582, 0xd974dbaa,
    0x379bd22f, 0xb9d65a2f, 0x8fad14d9, 0x72a55b5f, 0x34d56c6e, 0xc0badd55, 0xc20ee31b, 0xeb567f69,
    0xdadac1c, 0xb6dcc8f5, 0xc6d89117, 0x16c4999d, 0xc9b0da2a, 0xfcd6e9b3, 0x72d299ae, 0x4c2b345b,
    0x5d2c06cb, 0x9b9a3ce2, 0x8e84866, 0x876d1806, 0xbaeb6183, 0xe2a89d5d, 0x4604d2fe, 0x9909c5e0,
    0xf2fb7bec, 0x7e04dcd0, 0xe5b24865, 0xda96b760, 0x74a4d01, 0xb0f35bea, 0x9a2edb2, 0x5327a0d3
};


TEST(Crc32c, RangeZero)
{
    int len = sizeof(crc_zero_check_table) / sizeof(crc_zero_check_table[0]);
    unsigned char *b = (unsigned char *)malloc(len);
    memset(b, 0, len);
    uint32_t crc = 1; /* when checking zero buffer we want to start with a non zero crc, otherwise
                       all the results are going to be zero */
    uint32_t *check = crc_zero_check_table;
    for (int i = 0 ; i < len; i++, check++) {
        crc = ceph_crc32c(crc, b + i, len - i);
        ASSERT_EQ(crc, *check);
    }
    free(b);
}

TEST(Crc32c, RangeNull)
{
    int len = sizeof(crc_zero_check_table) / sizeof(crc_zero_check_table[0]);
    uint32_t crc = 1; /* when checking zero buffer we want to start with a non zero crc, otherwise
                       all the results are going to be zero */
    uint32_t *check = crc_zero_check_table;

    for (int i = 0 ; i < len; i++, check++) {
        crc = ceph_crc32c(crc, NULL, len - i);
        ASSERT_EQ(crc, *check);
    }
}

double estimate_clock_resolution()
{
    volatile char *p = (volatile char *)malloc(1024);
    utime_t start;
    utime_t end;
    std::set<double> S;
    for (int j = 10; j < 200; j += 1) {
        start = ceph_clock_now();
        for (int i = 0; i < j; i++) {
            p[i] = 1;
        }
        end = ceph_clock_now();
        S.insert((double)(end - start));
    }
    auto head = S.begin();
    auto tail = S.end();
    for (size_t i = 0; i < S.size() / 4; i++) {
        ++head;
        --tail;
    }
    double v = *(head++);
    double range = 0;
    while (head != tail) {
        range = std::max(range, *head - v);
        v = *head;
        head++;
    }
    free((void *)p);
    return range;
}

TEST(Crc32c, zeros_performance_compare)
{
    double resolution = estimate_clock_resolution();
    utime_t start;
    utime_t pre_start;
    utime_t end;
    double time_adjusted;
    using namespace std::chrono;
    high_resolution_clock::now();
    for (size_t scale = 1; scale < 31; scale++) {
        size_t size = (1 << scale) + rand() % (1 << scale);
        pre_start = ceph_clock_now();
        start = ceph_clock_now();
        uint32_t crc_a = ceph_crc32c(111, nullptr, size);
        end = ceph_clock_now();
        time_adjusted = (end - start) - (start - pre_start);
        std::cout << "regular  method. size=" << size << " time= " << (double)(end - start)
                  << " at " << (double)size / (1024 * 1024) / (time_adjusted) << " MB/sec"
                  << " error=" << resolution / time_adjusted * 100 << "%" << std::endl;

        pre_start = ceph_clock_now();
        start = ceph_clock_now();
#ifdef HAVE_POWER8
        uint32_t crc_b = ceph_crc32c_zeros(111, size);
#else
        uint32_t crc_b = ceph_crc32c_func(111, nullptr, size);
#endif
        end = ceph_clock_now();
        time_adjusted = (end - start) - (start - pre_start);
#ifdef HAVE_POWER8
        std::cout << "ceph_crc32c_zeros method. size=" << size << " time="
                  << (double)(end - start) << " at " << (double)size / (1024 * 1024) / (time_adjusted)
                  << " MB/sec" << " error=" << resolution / time_adjusted * 100 << "%"
                  << std::endl;
#else
        std::cout << "fallback method. size=" << size << " time=" << (double)(end - start)
                  << " at " << (double)size / (1024 * 1024) / (time_adjusted) << " MB/sec"
                  << " error=" << resolution / time_adjusted * 100 << "%" << std::endl;
#endif
        EXPECT_EQ(crc_a, crc_b);
    }
}

TEST(Crc32c, zeros_performance)
{
    constexpr size_t ITER = 100000;
    utime_t start;
    utime_t end;

    start = ceph_clock_now();
    for (size_t i = 0; i < ITER; i++) {
        for (size_t scale = 1; scale < 31; scale++) {
            size_t size = (1 << scale) + rand() % (1 << scale);
            ceph_crc32c(rand(), nullptr, size);
        }
    }
    end = ceph_clock_now();
    std::cout << "iterations=" << ITER * 31 << " time=" << (double)(end - start) << std::endl;

}

