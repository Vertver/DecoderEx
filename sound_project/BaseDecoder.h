/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* DecoderEx - little ultimate decoder
* MIT-License
**********************************************************
* Module Name: Base Decoder interface
**********************************************************
* BaseDecoder.h
* IBaseDecoder interface header
*********************************************************/
#pragma once
#include "wfmt.h"

class IBaseDecoder
{
public:
	virtual bool Open(const char* PathToFile, DWORD Flags) = 0;
	virtual void Close() = 0;

	virtual void Read(void* OutData, size_t FramesToRead, char Type) = 0;
	virtual void Read(float* OutData, size_t FramesToRead) = 0;
	virtual void Read(short* OutData, size_t FramesToRead) = 0;

	virtual void Write(void* Data, size_t FramesToWrite, char Type) = 0;
	virtual void Write(float* Data, size_t FramesToWrite) = 0;
	virtual void Write(short* Data, size_t FramesToWrite) = 0;

	virtual void Seek(size_t FramesToSeek) = 0;

	virtual size_t GetFrames() = 0;
	virtual size_t GetFullSize() = 0;
	virtual MINIWAVEFORMAT GetFormat() = 0;
};

