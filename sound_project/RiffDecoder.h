/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* DecoderEx - little ultimate decoder
* MIT-License
**********************************************************
* Module Name: RIFF Decoder implementation
**********************************************************
* RiffDecoder.h
* CRIFFDecoder interface header
*********************************************************/
#pragma once
#include "BaseDecoder.h"

struct RIFFHEADER
{
	DWORD MagicWord;
	DWORD SubchunkSize;
};

struct WAVEHEADER 
{
	WORD Format;
	WORD Channels;
	DWORD SampleRate;
	DWORD ByteRate;
	WORD BlockAlign;
	WORD Bits;
};

/*
	RIFF (.wav) decoder class
*/
class CRIFFDecoder : public IBaseDecoder
{
public:
	~CRIFFDecoder() { Close(); }

	bool Open(const char* PathToFile, DWORD Flags) override;
	void Close() override;

	void Read(void* OutData, size_t FramesToRead, char Type) override;
	void Read(float* OutData, size_t FramesToRead) override;
	void Read(short* OutData, size_t FramesToRead) override;

	void Write(void* Data, size_t FramesToWrite, char Type) override;
	void Write(float* Data, size_t FramesToWrite) override;
	void Write(short* Data, size_t FramesToWrite) override;

	void Seek(size_t FramesToSeek) override;

	size_t GetFrames() override;
	size_t GetFullSize() override;
	MINIWAVEFORMAT GetFormat() override;

private:
	void* pHandle = nullptr;
	MINIWAVEFORMAT fmt = {};
	size_t Frames = 0;
	size_t HeaderSize = 0;
	DWORD Temp = 0;
};