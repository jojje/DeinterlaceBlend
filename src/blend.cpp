#include <windows.h>
#include <intrin.h>
#include "blend.h"


#define uint32 unsigned int
#define uchar unsigned char

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
