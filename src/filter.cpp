// SmoothSkip: AVISynth filter for processing stuttering clips
// Copyright (C) 2015 Jonas Tingeborn
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

#include <Windows.h>
#include "filter.h"
#include "blend.h"

PVideoFrame __stdcall DeinterlaceBlend::GetFrame(int n, IScriptEnvironment* env) {
	VideoInfo vi = child->GetVideoInfo();
	PVideoFrame src = child->GetFrame(n, env);
	PVideoFrame dst = env->NewVideoFrame(vi);
	BlendPlanes(src, dst, vi);
	return dst;
}

// Constructor
DeinterlaceBlend::DeinterlaceBlend(PClip _child, IScriptEnvironment* env) : GenericVideoFilter(_child) {
	VideoInfo vi = child->GetVideoInfo();
	if (vi.IsPlanar()) {
	//	env->ThrowError(PLUGIN ": input clip to filter must be in YUY2 or RGB color space");
	}
}

AVSValue __cdecl Create_DeinterlaceBlend(AVSValue args, void* user_data, IScriptEnvironment* env) {
	return new DeinterlaceBlend(args[0].AsClip(), env);
}

