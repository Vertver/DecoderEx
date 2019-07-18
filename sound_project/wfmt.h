/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* DecoderEx - little ultimate decoder
* MIT-License
**********************************************************
* Module Name: Wave format 
**********************************************************
* wfmt.h
* Waveformat struct
*********************************************************/
#pragma once
#include <stdlib.h>
#include <windows.h>

typedef enum 
{
	NoneType = 0,
	ByteType,
	ShortType, 
	ThreeByteType,
	IntType,
	FloatType,
	DoubleType
} WaveFormatType;

typedef struct  
{
	char Channels;
	char isFloat;
	int Bits;
	int SampleRate;
} MINIWAVEFORMAT;

