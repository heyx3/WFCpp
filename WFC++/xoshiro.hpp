#pragma once

/*
 * A C++ implementation of a family of Xoshiro generators.
 *
 * See:
 *     https://arxiv.org/abs/1805.01407
 *     http://xoshiro.di.unimi.it/xoshiro256plus.c
 *     http://xoshiro.di.unimi.it/xoshiro256starstar.c
 *     http://xoshiro.di.unimi.it/xoshiro512plus.c
 *     http://xoshiro.di.unimi.it/xoshiro512starstar.c
 *
 * Based on the design and public domain implementation of Xoshiro by
 * written in 2018 by David Blackman and Sebastiano Vigna.  It is a
 * variation on George Marsaglia's for XorShift PRNG scheme and the
 * same (G)LFSR theory applies.
 *
 * Xoshiro256** fails PractRand if passed through certain bijections (e.g.,
 * multiplication by 57) and the entire family suffers from issues due
 * to near repeats but these near-repeat issues are unlikely to be
 * encountered in normal-sized variants without deliberately contriving a
 * PRNG state that generates them.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-18 Melissa E. O'Neill
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <cstdint>

namespace WFC {
namespace xoshiro_detail {

template <typename itype, typename rtype,
          unsigned int a, unsigned int b>
class xoshiro_x4 {
protected:
    itype s0_, s1_, s2_, s3_;

    static constexpr unsigned int ITYPE_BITS = 8*sizeof(itype);
    static constexpr unsigned int RTYPE_BITS = 8*sizeof(rtype);
    
    static inline itype rotl(const itype x, int k) {
        return (x << k) | (x >> (ITYPE_BITS - k));
    }

public:
    using state_type = itype;
    using result_type = rtype;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~ result_type(0); }

    xoshiro_x4(itype s0, itype s1, itype s2, itype s3)
        : s0_(s0), s1_(s1), s2_(s2), s3_((s0 || s1 || s2 || s3) ? s3 : 1)
    {
        // Nothing (else) to do.
    }

    // Note, Vigna recommends seeding using output from SplitMix, but
    // if you need another PRNG to use this PRNG, that's asking a bit
    // of a lot, so we seed directly.
    xoshiro_x4(itype seed = 1)
        : xoshiro_x4(0x01d353e5f3993bb0+seed, 0x7b9c0df6cb193b20*(seed+1), 
                     0xfdfcaa91110765b6-seed, 0x2d24cbe0ef44dcd2*(seed-1))
        // Easter-egg seed value for Xoshiro_256** to remind users that
        // they should seed their PRNGs properly.  Only occurs if you
        // seed with zero.
    {
        for (int i = 0; i < 16; ++i)
            advance();
    }

    void advance()
    {
        const uint64_t t = s1_ << a;

        s2_ ^= s0_;
        s3_ ^= s1_;
        s1_ ^= s2_;
        s0_ ^= s3_;

        s2_ ^= t;

        s3_ = rotl(s3_, b);
    }

    bool operator==(const xoshiro_x4& rhs)
    {
        return (s0_ == rhs.s0_) && (s1_ == rhs.s1_) 
               && (s2_ == rhs.s2_) && (s3_ == rhs.s3_);
    }

    bool operator!=(const xoshiro_x4& rhs)
    {
        return !operator==(rhs);
    }

    // Not (yet) implemented:
    //   - arbitrary jumpahead (doable, but annoying to write).
    //   - I/O
    //   - Seeding from a seed_seq.
};

template <typename itype, typename rtype,
          unsigned int a, unsigned int b>
class xoshiro_x8 {
protected:
    itype s0_, s1_, s2_, s3_, s4_, s5_, s6_, s7_;

    static constexpr unsigned int ITYPE_BITS = 8*sizeof(itype);
    static constexpr unsigned int RTYPE_BITS = 8*sizeof(rtype);
    
    static inline itype rotl(const itype x, int k) {
        return (x << k) | (x >> (ITYPE_BITS - k));
    }

public:
    using state_type = itype;
    using result_type = rtype;

    static constexpr result_type min() { return 0; }
    static constexpr result_type max() { return ~ result_type(0); }

    xoshiro_x8(itype s0, itype s1, itype s2, itype s3, 
               itype s4, itype s5, itype s6, itype s7)
        : s0_(s0), s1_(s1), s2_(s2), s3_(s3), s4_(s4), s5_(s5), s6_(s6),
          s7_((s0 || s1 || s2 || s3 || s4 || s5 || s6 || s7) ? s7 : 1)
    {
        // Nothing (else) to do.
    }

    // Note, Vigna recommends seeding using output from SplitMix, but
    // if you need another PRNG to use this PRNG, that's asking a bit
    // of a lot, so we seed directly.
    xoshiro_x8(itype seed = 1)
        : xoshiro_x8(0x1ced436497db2a59+seed, 0x75474f85d8a6892c*(seed+1),
                     0xa0fef4b8094c9c86-seed, 0x748fa1a9bb555169*(seed-1), 
                     0xd7a59a6d64e66858+seed, 0xf03b7efdb73db601*(seed+1),
                     0xfab342a99dd71962-seed, 0x8a6921456faa6b54*(seed-1))
        // Easter-egg seed value for Xoshiro_512** to remind users that
        // they should seed their PRNGs properly.  Only occurs if you
        // seed with zero.
    {
        for (int i = 0; i < 16; ++i)
            advance();
    }

    void advance()
    {
        const uint64_t t = s1_ << a;

        s2_ ^= s0_;
        s5_ ^= s1_;
        s1_ ^= s2_;
        s7_ ^= s3_;
        s3_ ^= s4_;
        s4_ ^= s5_;
        s0_ ^= s6_;
        s6_ ^= s7_;

        s6_ ^= t;

        s7_ = rotl(s7_, b);
    }

    bool operator==(const xoshiro_x8& rhs)
    {
        return (s0_ == rhs.s0_) && (s1_ == rhs.s1_) 
               && (s2_ == rhs.s2_) && (s3_ == rhs.s3_)
               && (s4_ == rhs.s4_) && (s5_ == rhs.s5_)
               && (s6_ == rhs.s6_) && (s7_ == rhs.s7_);
    }

    bool operator!=(const xoshiro_x8& rhs)
    {
        return !operator==(rhs);
    }

    // Not (yet) implemented:
    //   - arbitrary jumpahead (doable, but annoying to write).
    //   - I/O
    //   - Seeding from a seed_seq.
};


template <typename xoshiro_base>
class xoshiro_plus : public xoshiro_base {
private:
    using base = xoshiro_base;
public:
    using base::base;

    typename base::result_type operator()()
    {
        const typename base::state_type result = base::s0_ + base::s3_;

        base::advance();

        return result >> (base::ITYPE_BITS - base::RTYPE_BITS);
    }
};

template <typename xoshiro_base, typename xoshiro_base::state_type mult>
class xoshiro_star : public xoshiro_base {
private:
    using base = xoshiro_base;
public:
    using base::base;

    typename base::result_type operator()()
    {
        const typename base::state_type result_star = base::s1_ * mult;

        base::advance();

        return result_star >> (base::ITYPE_BITS - base::RTYPE_BITS);
    }
};

template <typename xoshiro_base, 
          typename xoshiro_base::state_type mult1, 
          unsigned int orot, 
          typename xoshiro_base::state_type mult2>
class xoshiro_starstar : public xoshiro_base {
private:
    using base = xoshiro_base;
public:
    using base::base;

    typename base::result_type operator()()
    {
        const typename base::state_type result_ss = 
            base::rotl(base::s1_ * mult1, orot) * mult2;

        base::advance();

        return result_ss >> (base::ITYPE_BITS - base::RTYPE_BITS);
    }
};

} // namespace xoshiro_detail
    
// - 512 state bits, uint64_t output, period 2^512 - 1

using xoshiro512plus64v1_0 =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x8<uint64_t, uint64_t, 11, 21>>;

using xoshiro512plus64 = xoshiro512plus64v1_0;

using xoshiro512starstar64v1_0 =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x8<uint64_t, uint64_t, 11, 21>,
        5, 7, 9>;

using xoshiro512starstar64 = xoshiro512starstar64v1_0;

// - 256 state bits, uint64_t output, period 2^256 - 1

using xoshiro256plus64v1_0 =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x4<uint64_t, uint64_t, 17, 45>>;

using xoshiro256plus64 = xoshiro256plus64v1_0;

using xoshiro256starstar64v1_0 =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x4<uint64_t, uint64_t, 17, 45>,
        5, 7, 9>;

using xoshiro256starstar64 = xoshiro256starstar64v1_0;

// - 128 state bits, uint32_t output, period 2^128 - 1

using xoshiro128plus32v1_0 =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x4<uint32_t, uint32_t, 9, 11>>;

using xoshiro128plus32 = xoshiro128plus32v1_0;

using xoshiro128starstar32v1_0 =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x4<uint32_t, uint32_t, 9, 11>,
        5, 7, 9>;

using xoshiro128starstar32 = xoshiro128starstar32v1_0;


// VERSIONS BELOW ARE FOR SPECIALIZED USES ONLY!
// (Constants courtesy of David Blackman.)

// - 64 state bits, uint16_t output, period 2^64 - 1                          

using xoshiro64plus16xxx =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x4<uint16_t, uint16_t, 5, 11>>;

using xoshiro64plus16 = xoshiro64plus16xxx;

using xoshiro64starstar16xxx =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x4<uint16_t, uint16_t, 5, 11>,
        5, 7, 9>;

using xoshiro64starstar16 = xoshiro64starstar16xxx;

// - 32 state bits, uint8_t output, period 2^32 - 1                          

using xoshiro32plus8xxx =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x4<uint8_t, uint8_t, 3, 7>>;

using xoshiro32plus8yyy =
    xoshiro_detail::xoshiro_plus<
        xoshiro_detail::xoshiro_x4<uint8_t, uint8_t, 3, 1>>;

using xoshiro32plus8 = xoshiro32plus8xxx;

using xoshiro32starstar8xxx =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x4<uint8_t, uint8_t, 3, 7>,
        5, 7, 9>;

using xoshiro32starstar8yyy =
    xoshiro_detail::xoshiro_starstar<
        xoshiro_detail::xoshiro_x4<uint8_t, uint8_t, 3, 1>,
        5, 7, 9>;

using xoshiro32starstar8 = xoshiro32starstar8xxx;

}