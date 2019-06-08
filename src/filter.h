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

#pragma once

#include "avisynth.h"

#define VERSION "0.0.2"
#define PLUGIN "DeinterlaceBlend"
#define AUTHOR "tinjon[at]gmail.com"

class DeinterlaceBlend : public GenericVideoFilter {
public:
	DeinterlaceBlend(PClip _child, IScriptEnvironment* env);
	PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};

AVSValue __cdecl Create_DeinterlaceBlend(AVSValue args, void* user_data, IScriptEnvironment* env);

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env) {
	env->AddFunction(PLUGIN, "c", Create_DeinterlaceBlend, 0);
	return PLUGIN " v" VERSION ", author: " AUTHOR;
}
