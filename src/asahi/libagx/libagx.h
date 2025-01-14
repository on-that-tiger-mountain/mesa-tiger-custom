/*
 * Copyright 2023 Alyssa Rosenzweig
 * SPDX-License-Identifier: MIT
 */

#ifndef LIBAGX_H
#define LIBAGX_H

/* Define stdint types compatible between the CPU and GPU for shared headers */
#ifndef __OPENCL_VERSION__
#include <stdint.h>
#include "util/macros.h"
#define GLOBAL(type_)            uint64_t
#define CONSTANT(type_)          uint64_t
#define AGX_STATIC_ASSERT(_COND) static_assert(_COND, #_COND)
#define global_
#define constant_
#else
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#define PACKED          __attribute__((packed, aligned(4)))
#define GLOBAL(type_)   global type_ *
#define CONSTANT(type_) constant type_ *
#define global_         global
#define constant_       constant
#define ASSERTED
#define assert(x)
#define BITFIELD_BIT(b)  (1u << b)
#define BITFIELD_MASK(m) (((m) == 32) ? 0xffffffff : ((1u << (m)) - 1))

typedef ulong uint64_t;
typedef uint uint32_t;
typedef ushort uint16_t;
typedef uchar uint8_t;

typedef long int64_t;
typedef int int32_t;
typedef short int16_t;
typedef char int8_t;

/* Define NIR intrinsics for CL */
uint32_t nir_interleave_agx(uint16_t x, uint16_t y);
void nir_doorbell_agx(uint8_t value);
void nir_stack_map_agx(uint16_t index, uint32_t address);
uint32_t nir_stack_unmap_agx(uint16_t index);
uint32_t nir_load_core_id_agx(void);
uint32_t nir_load_helper_op_id_agx(void);
uint32_t nir_load_helper_arg_lo_agx(void);
uint32_t nir_load_helper_arg_hi_agx(void);
void nir_fence_helper_exit_agx(void);

uint4 nir_bindless_image_load(uint2 handle, int4 coord, uint sample, uint lod,
                              uint image_dim, uint image_array, uint format,
                              uint access, uint dest_type);

void nir_bindless_image_store(uint2 handle, int4 coord, uint sample,
                              uint4 datum, uint lod, uint image_dim,
                              uint image_array, uint format, uint access,
                              uint src_type);

/* I have no idea why CL doesn't have this */
uint nir_ballot(bool cond);

#define _S(x)            #x
#define AGX_PASTE_(x, y) x##y
#define AGX_PASTE(x, y)  AGX_PASTE_(x, y)
#define AGX_STATIC_ASSERT(_COND)                                               \
   typedef char AGX_PASTE(static_assertion, __LINE__)[(_COND) ? 1 : -1]

#define KERNEL(x) __attribute__((reqd_work_group_size(x, 1, 1))) kernel void

static inline uint
align(uint x, uint y)
{
   return (x + y - 1) & ~(y - 1);
}

static inline uint32_t
libagx_logbase2_ceil(uint32_t n)
{
   return (n <= 1) ? 0 : 32 - clz(n - 1);
}

#define offsetof(x, y) __builtin_offsetof(x, y)

#endif

#endif
