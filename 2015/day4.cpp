#include "prelude/prelude.hpp"
#include <openssl/md5.h>
#include <span>

int main(int, char **argv) {
    const std::string problem_input(argv[1]);
    std::array<unsigned char, MD5_DIGEST_LENGTH> digest;
    const std::array<unsigned char, MD5_DIGEST_LENGTH> zeros = {};
    const std::array<unsigned char, MD5_DIGEST_LENGTH> p1_mask = {
        0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };
    const std::array<unsigned char, MD5_DIGEST_LENGTH> p2_mask = {
        0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    };
    int part1 = -1, part2 = -1;
    for (int i = 0; part1 < 0 || part2 < 0; ++i) {
        const std::string str = fmt::format("{}{}", problem_input, i);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        MD5(reinterpret_cast<const unsigned char *>(str.data()), str.size(), digest.data());
#pragma clang diagnostic pop

        if (part1 < 0) {
            std::array<unsigned char, MD5_DIGEST_LENGTH> p1 = {};
            for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
                p1[i] = digest[i] & ~p1_mask[i];
            }
            if (p1 == zeros) {
                part1 = i;
            }
        }
        if (part2 < 0) {
            std::array<unsigned char, MD5_DIGEST_LENGTH> p2 = {};
            for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
                p2[i] = digest[i] & ~p2_mask[i];
            }
            if (p2 == zeros) {
                part2 = i;
            }
        }
    }
    fmt::print("part 1: {}\n", part1);
    fmt::print("part 2: {}\n", part2);
    return 0;
}