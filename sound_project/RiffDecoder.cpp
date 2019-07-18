/*********************************************************
* Copyright (C) VERTVER, 2019. All rights reserved.
* DecoderEx - little ultimate decoder
* MIT-License
**********************************************************
* Module Name: RIFF Decoder implementation
**********************************************************
* RiffDecoder.cpp
* CRIFFDecoder interface implementation
*********************************************************/
#include "RiffDecoder.h"

#define BUFSIZE 2048
#define maxmin(a, minimum, maximum)  min(max(a, minimum), maximum)

bool		
CRIFFDecoder::Open(const char* PathToFile, DWORD Flags)
{
	DWORD MagicWord = 0;
	DWORD ReadWriteAccesses = Flags == 1 ? GENERIC_READ : GENERIC_WRITE | GENERIC_READ;
	DWORD ShareAcceses = Flags == 1 ? FILE_SHARE_READ : FILE_SHARE_WRITE;
	DWORD AdditionalAcceses = Flags == 1 ? OPEN_EXISTING : CREATE_ALWAYS;
	size_t SizeToJump = 0;
	char Buffer[128] = {};
	wchar_t szPath[520] = {};

	if (MultiByteToWideChar(CP_UTF8, 0, PathToFile, strlen(PathToFile) - 1, szPath, 520 - 1))
	{
		pHandle = CreateFileW(szPath, ReadWriteAccesses, ShareAcceses, NULL, AdditionalAcceses, 0, NULL);
		if (!pHandle) return false;
	}

	if (!ReadFile(pHandle, &MagicWord, 4, &Temp, nullptr))
	{
		Close();
		return false;
	}

	/*
		If our file hasn't RIFF Magic - exit
	*/
	if (MagicWord != 0x46464952 && MagicWord != 0x45564157 && MagicWord != 0x20746d66)
	{
		Close();
		return false;
	}

	if (!ReadFile(pHandle, Buffer, 128, &Temp, nullptr)) 
	{
		Close();
		return false;
	}

	/*
		Get information about wave format 
	*/
	for (size_t i = 0; i < 8; i++)
	{
		RIFFHEADER* pHeader = (RIFFHEADER*)Buffer[SizeToJump];
		if (pHeader->MagicWord == 0x666d7420)	// 'fmt ' chunk
		{
			WAVEHEADER* pWaveHeader = (WAVEHEADER*)pHeader;
			fmt.Bits = pWaveHeader->Bits;
			fmt.Channels = pWaveHeader->Channels;
			fmt.SampleRate = pWaveHeader->SampleRate;
			fmt.isFloat = pWaveHeader->Format == 3 ? 1 : 0;
		}

		/*
			If our magic word is 'data' == we've done
		*/
		if (pHeader->MagicWord == 0x64617461)
		{
			Frames = pHeader->SubchunkSize / fmt.Channels / (fmt.Bits / 8);
			SizeToJump += 8;
			break;
		}

		SizeToJump += pHeader->SubchunkSize + 8;
	}

	HeaderSize = SizeToJump;
}

void	
CRIFFDecoder::Close()
{
	if (pHandle || pHandle != INVALID_HANDLE_VALUE) CloseHandle(pHandle);
}

void 
CRIFFDecoder::Read(void* OutData, size_t FramesToRead, char Type) 
{
	switch (Type)
	{
	case ShortType:
	{
		short* pDataThis = (short*)OutData;
		Read(pDataThis, FramesToRead);
	}	
	case FloatType:
	{
		float* pDataThis = (float*)OutData;
		Read(pDataThis, FramesToRead);
	}
	default:
		break;
	}
}

void 
CRIFFDecoder::Read(float* OutData, size_t FramesToRead)
{
	if (fmt.isFloat)
	{
		if (FramesToRead > 0)
		{
			ReadFile(pHandle, OutData, FramesToRead * fmt.Channels * (fmt.Bits / 8), &Temp, nullptr);
		}
	}
	else
	{
		if (fmt.Bits == 8) return;

		if (fmt.Bits == 16)
		{
			short Buffer[BUFSIZE] = {};
			size_t ToRead = FramesToRead;
			
			while (ToRead)
			{
				/*
					If we can't read file - exit
				*/
				if (!ReadFile(pHandle, Buffer, ToRead * fmt.Channels < BUFSIZE ? ToRead * fmt.Channels * (fmt.Bits / 8) : BUFSIZE * (fmt.Bits / 8), &Temp, nullptr)) break;

				/*
					Copy data from local buffer and convert to float type
				*/
				for (size_t i = 0; i < (ToRead * fmt.Channels < BUFSIZE ? ToRead * fmt.Channels : BUFSIZE); i++)
				{
					OutData[FramesToRead - ToRead + i] = (float)Buffer[i] / 32768.0f;
				}

				ToRead -= BUFSIZE;
				ToRead = ToRead < 0 ? 0 : ToRead;
			}
		}
	}
}

void 
CRIFFDecoder::Read(short* OutData, size_t FramesToRead)
{
	if (fmt.isFloat)
	{
		float Buffer[BUFSIZE] = {};
		size_t ToRead = FramesToRead;

		while (ToRead)
		{
			/*
				If we can't read file - exit
			*/
			if (!ReadFile(pHandle, Buffer, ToRead * fmt.Channels < BUFSIZE ? ToRead * fmt.Channels * (fmt.Bits / 8) : BUFSIZE * (fmt.Bits / 8), &Temp, nullptr)) break;

			/*
				Copy data from local buffer and convert to float type
			*/
			for (size_t i = 0; i < (ToRead * fmt.Channels < BUFSIZE ? ToRead * fmt.Channels : BUFSIZE); i++)
			{
				OutData[FramesToRead - ToRead + i] = maxmin(((short)(Buffer[i] * 32768.0f)), -32768, 32767);
			}

			ToRead -= BUFSIZE;
			ToRead = ToRead < 0 ? 0 : ToRead;
		}
	}
	else
	{
		if (FramesToRead > 0 && fmt.Bits == 16)
		{
			ReadFile(pHandle, OutData, FramesToRead * fmt.Channels * (fmt.Bits / 8), &Temp, nullptr);
		}
	}
}

void 
CRIFFDecoder::Write(void* Data, size_t FramesToWrite, char Type)
{
	/*
		#TODO:
	*/
	if (FramesToWrite > 0)
	{
		WriteFile(pHandle, Data, FramesToWrite * fmt.Channels * (fmt.Bits / 8), &Temp, nullptr);
	}
}

void 
CRIFFDecoder::Write(float* Data, size_t FramesToWrite)
{

}

void 
CRIFFDecoder::Write(short* Data, size_t FramesToWrite)
{

}

void
CRIFFDecoder::Seek(size_t FramesToSeek)
{
	LARGE_INTEGER largTemp = {};
	LARGE_INTEGER larg = {};
	larg.QuadPart = HeaderSize + (fmt.Channels * (fmt.Bits / 8) * FramesToSeek);

	SetFilePointerEx(pHandle, larg, &largTemp, FILE_BEGIN);
}

size_t	
CRIFFDecoder::GetFrames()
{
	return Frames;
}

size_t		
CRIFFDecoder::GetFullSize()
{
	return (Frames * (fmt.Bits / 8) * fmt.Channels) + HeaderSize;
}

MINIWAVEFORMAT
CRIFFDecoder::GetFormat()
{
	return fmt;
}
