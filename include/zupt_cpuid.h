/*
 * Zupt — CPU Feature Detection
 * Copyright (c) 2026 Cristian Cezar Moisés — MIT License
 */
#ifndef ZUPT_CPUID_H
#define ZUPT_CPUID_H

#include <stdint.h>

typedef struct {
    int has_aesni;   /* CPUID.01H:ECX[25] — AES-NI instructions */
    int has_pclmul;  /* CPUID.01H:ECX[1]  — CLMUL (carry-less multiply) */
    int has_avx2;    /* CPUID.07H:EBX[5]  — AVX2 (256-bit SIMD) */
    int has_sse41;   /* CPUID.01H:ECX[19] — SSE4.1 */

    int has_avx;       /* CPUID.01H:ECX[28]  — AVX */
    int has_osxsave;   /* CPUID.01H:ECX[27]  — XSAVE/XGETBV enabled by OS */

    int has_avx512f;   /* CPUID.07H:EBX[16]  — AVX-512 Foundation */
    int has_avx512dq;  /* CPUID.07H:EBX[17]  — AVX-512 Doubleword/Quadword */
    int has_avx512cd;  /* CPUID.07H:EBX[28]  — AVX-512 Conflict Detection */
    int has_avx512bw;  /* CPUID.07H:EBX[30]  — AVX-512 Byte/Word */
    int has_avx512vl;  /* CPUID.07H:EBX[31]  — AVX-512 Vector Length */

} zupt_cpu_features_t;

/*@ assigns f->has_aesni, f->has_pclmul, f->has_avx2, f->has_sse41;
              f->has_avx, f->has_osxsave,
              f->has_avx512f, f->has_avx512dq, f->has_avx512cd,
              f->has_avx512bw, f->has_avx512vl;
  @ ensures f->has_aesni == 0 || f->has_aesni == 1;
  @ ensures f->has_pclmul == 0 || f->has_pclmul == 1;
  @ ensures f->has_avx2 == 0 || f->has_avx2 == 1;
  @ ensures f->has_sse41 == 0 || f->has_sse41 == 1;
  @ ensures f->has_avx == 0 || f->has_avx == 1;
  @ ensures f->has_osxsave == 0 || f->has_osxsave == 1;
  @ ensures f->has_avx512f == 0 || f->has_avx512f == 1;
  @ ensures f->has_avx512dq == 0 || f->has_avx512dq == 1;
  @ ensures f->has_avx512cd == 0 || f->has_avx512cd == 1;
  @ ensures f->has_avx512bw == 0 || f->has_avx512bw == 1;
  @ ensures f->has_avx512vl == 0 || f->has_avx512vl == 1;  
*/
void zupt_detect_cpu(zupt_cpu_features_t *f);

/* Global instance — set once at program start */
extern zupt_cpu_features_t zupt_cpu;

#endif /* ZUPT_CPUID_H */
