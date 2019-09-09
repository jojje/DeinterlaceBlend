// DeinterlaceBlend: Port of Virtualdub internal blend deinterlacer
// Copyright (C) 2019 Jonas Tingeborn
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
// USA.

#include <windows.h>
#include <intrin.h>
#include "blend.h"


#define uint32 unsigned int
#define uchar unsigned char

// All but the last function was originally written by Avery Lee of VirtualDub

#ifdef _M_IX86
static void __declspec(naked) asm_blend_row_clipped(void *dst, const void *src, uint32 w, ptrdiff_t srcpitch) {
	__asm {
		push	ebp
		push	edi
		push	esi
		push	ebx

		mov		edi, [esp + 20]
		mov		esi, [esp + 24]
		sub		edi, esi
		mov		ebp, [esp + 28]
		mov		edx, [esp + 32]

		xloop:
		mov		ecx, [esi]
			mov		eax, 0fefefefeh

			mov		ebx, [esi + edx]
			and eax, ecx

			shr		eax, 1
			and ebx, 0fefefefeh

			shr		ebx, 1
			add		esi, 4

			add		eax, ebx
			dec		ebp

			mov[edi + esi - 4], eax
			jnz		xloop

			pop		ebx
			pop		esi
			pop		edi
			pop		ebp
			ret
	};
}

void asm_blend_row_SSE2(void *dst, const void *src, uint32 w, ptrdiff_t srcpitch) {
	__m128i zero = _mm_setzero_si128();
	__m128i inv = _mm_cmpeq_epi8(zero, zero);

	w = (w + 3) >> 2;

	const __m128i *src1 = (const __m128i *)src;
	const __m128i *src2 = (const __m128i *)((const char *)src + srcpitch);
	const __m128i *src3 = (const __m128i *)((const char *)src + srcpitch * 2);
	__m128i *dstrow = (__m128i *)dst;
	do {
		__m128i a = *src1++;
		__m128i b = *src2++;
		__m128i c = *src3++;

		*dstrow++ = _mm_avg_epu8(_mm_xor_si128(_mm_avg_epu8(_mm_xor_si128(a, inv), _mm_xor_si128(c, inv)), inv), b);
	} while (--w);
}
#else
static void asm_blend_row_clipped(void *dst0, const void *src0, uint32 w, ptrdiff_t srcpitch) {
	uint32 *dst = (uint32 *)dst0;
	const uint32 *src = (const uint32 *)src0;
	const uint32 *src2 = (const uint32 *)((const char *)src + srcpitch);

	do {
		const uint32 x = *src++;
		const uint32 y = *src2++;

		*dst++ = (x | y) - (((x^y) & 0xfefefefe) >> 1);
	} while (--w);
}

static void asm_blend_row_SSE2(void *dst0, const void *src0, uint32 w, ptrdiff_t srcpitch) {
	uint32 *dst = (uint32 *)dst0;
	const uint32 *src = (const uint32 *)src0;
	const uint32 *src2 = (const uint32 *)((const char *)src + srcpitch);
	const uint32 *src3 = (const uint32 *)((const char *)src2 + srcpitch);

	do {
		const uint32 a = *src++;
		const uint32 b = *src2++;
		const uint32 c = *src3++;
		const uint32 hi = (a & 0xfcfcfc) + 2 * (b & 0xfcfcfc) + (c & 0xfcfcfc);
		const uint32 lo = (a & 0x030303) + 2 * (b & 0x030303) + (c & 0x030303) + 0x020202;

		*dst++ = (hi + (lo & 0x0c0c0c)) >> 2;
	} while (--w);
}
#endif


void BlendPlane(uchar *dst, const int dstpitch, const uchar *src, const int srcpitch, int w, int h) {
	w = (w + 3) >> 2;

	asm_blend_row_clipped(dst, src, w, srcpitch);
	if (h -= 2)
		do {
			dst = ((uchar*)dst + dstpitch);

			asm_blend_row_SSE2(dst, src, w, srcpitch);

			src = ((uchar *)src + srcpitch);
		} while (--h);

		asm_blend_row_clipped((uchar *)dst + dstpitch, src, w, srcpitch);
}

void BlendPlanes(PVideoFrame& src, PVideoFrame& dst, VideoInfo& vi) {
	const uchar* srcp = src->GetReadPtr();
	const int src_pitch = src->GetPitch();
	const int src_width = src->GetRowSize();
	const int src_height = src->GetHeight();

	uchar* dstp = dst->GetWritePtr();
	const int dst_pitch = dst->GetPitch();
	const int dst_width = dst->GetRowSize();
	const int dst_height = dst->GetHeight();

	BlendPlane(dstp, dst_pitch, srcp, src_pitch, dst_width, dst_height);

	if (vi.IsPlanar()) {
		const uchar* u_srcp = src->GetReadPtr(PLANAR_U);
		const uchar* v_srcp = src->GetReadPtr(PLANAR_V);
		const int uv_src_pitch = src->GetPitch(PLANAR_U);
		const int uv_src_width = src->GetRowSize(PLANAR_U);
		const int uv_src_height = src->GetHeight(PLANAR_U);

		uchar* u_dstp = dst->GetWritePtr(PLANAR_U);
		uchar* v_dstp = dst->GetWritePtr(PLANAR_V);
		const int uv_dst_pitch = dst->GetPitch(PLANAR_U);
		const int uv_dst_width = dst->GetRowSize(PLANAR_U);
		const int uv_dst_height = dst->GetHeight(PLANAR_U);

		BlendPlane(u_dstp, uv_dst_pitch, u_srcp, uv_src_pitch, uv_dst_width, uv_dst_height);
		BlendPlane(v_dstp, uv_dst_pitch, v_srcp, uv_src_pitch, uv_dst_width, uv_dst_height);
	}
}
