/*
 * Zupt — CPU Feature Detection
 * Copyright (c) 2026 Cristian Cezar Moisés — MIT License
 *
 * Detects AES-NI, PCLMUL, AVX, AVX2, AVX-512, SSE4.1 at runtime.
 * Used to dispatch optimized code paths when available.
 */
#include "zupt_cpuid.h"
#include <string.h>

/* Global instance */
zupt_cpu_features_t zupt_cpu = {0};

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
  #define ZUPT_HAS_CPUID 1
#else
  #define ZUPT_HAS_CPUID 0
#endif

#if ZUPT_HAS_CPUID

#if defined(_MSC_VER)
  #include <intrin.h>

  static void zupt_cpuid(int leaf, int subleaf, int *eax, int *ebx, int *ecx, int *edx) {
      int regs[4];
      __cpuidex(regs, leaf, subleaf);
      *eax = regs[0];
      *ebx = regs[1];
      *ecx = regs[2];
      *edx = regs[3];
  }

  static uint64_t zupt_xgetbv(unsigned int xcr) {
      return _xgetbv(xcr);
  }

#elif defined(__GNUC__) || defined(__clang__)
  #include <cpuid.h>

  static void zupt_cpuid(int leaf, int subleaf, int *eax, int *ebx, int *ecx, int *edx) {
      unsigned int a = 0, b = 0, c = 0, d = 0;
      __cpuid_count((unsigned int)leaf, (unsigned int)subleaf, a, b, c, d);
      *eax = (int)a;
      *ebx = (int)b;
      *ecx = (int)c;
      *edx = (int)d;
  }

  static uint64_t zupt_xgetbv(unsigned int xcr) {
      unsigned int eax, edx;
      __asm__ __volatile__ (".byte 0x0f, 0x01, 0xd0"
                            : "=a"(eax), "=d"(edx)
                            : "c"(xcr));
      return ((uint64_t)edx << 32) | eax;
  }

#else
  static void zupt_cpuid(int leaf, int subleaf, int *eax, int *ebx, int *ecx, int *edx) {
      __asm__ __volatile__ (
          "cpuid"
          : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
          : "a"(leaf), "c"(subleaf)
      );
  }

  static uint64_t zupt_xgetbv(unsigned int xcr) {
      unsigned int eax, edx;
      __asm__ __volatile__ (
          ".byte 0x0f, 0x01, 0xd0"
          : "=a"(eax), "=d"(edx)
          : "c"(xcr)
      );
      return ((uint64_t)edx << 32) | eax;
  }
#endif

void zupt_detect_cpu(zupt_cpu_features_t *f) {
    int eax, ebx, ecx, edx;
    int max_leaf = 0;
    uint64_t xcr0 = 0;
    int os_has_avx_state = 0;
    int os_has_avx512_state = 0;

    memset(f, 0, sizeof(*f));

    /* Maximum supported CPUID leaf */
    zupt_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    max_leaf = eax;

    if (max_leaf >= 1) {
        zupt_cpuid(1, 0, &eax, &ebx, &ecx, &edx);

        f->has_aesni   = (ecx >> 25) & 1;
        f->has_pclmul  = (ecx >>  1) & 1;
        f->has_sse41   = (ecx >> 19) & 1;
        f->has_avx     = (ecx >> 28) & 1;
        f->has_osxsave = (ecx >> 27) & 1;

        /*
         * For AVX usage, OS must enable XMM and YMM state:
         * XCR0[1] = XMM state
         * XCR0[2] = YMM state
         */
        if (f->has_osxsave) {
            xcr0 = zupt_xgetbv(0);
            os_has_avx_state = ((xcr0 & 0x6) == 0x6);
        }

        if (!os_has_avx_state) {
            f->has_avx = 0;
        }
    }

    if (max_leaf >= 7) {
        zupt_cpuid(7, 0, &eax, &ebx, &ecx, &edx);

        /*
         * AVX2 requires AVX OS state enabled too.
         */
        if (os_has_avx_state) {
            f->has_avx2 = (ebx >> 5) & 1;
        }

        /*
         * For AVX-512 usage, OS must enable:
         * XCR0[1] = XMM
         * XCR0[2] = YMM
         * XCR0[5] = opmask
         * XCR0[6] = ZMM_hi256
         * XCR0[7] = Hi16_ZMM
         *
         * Required mask = bits 1,2,5,6,7 = 0xE6
         */
        if (f->has_osxsave) {
            os_has_avx512_state = ((xcr0 & 0xE6) == 0xE6);
        }

        if (os_has_avx512_state) {
            f->has_avx512f  = (ebx >> 16) & 1;
            f->has_avx512dq = (ebx >> 17) & 1;
            f->has_avx512cd = (ebx >> 28) & 1;
            f->has_avx512bw = (ebx >> 30) & 1;
            f->has_avx512vl = (ebx >> 31) & 1;
        }
    }
}

#else /* Non-x86 architecture */

void zupt_detect_cpu(zupt_cpu_features_t *f) {
    memset(f, 0, sizeof(*f));
    /* No x86 CPUID/XGETBV on ARM/RISC-V/etc */
}

#endif /* ZUPT_HAS_CPUID */
