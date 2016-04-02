// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     xaudio.h
//   Author:
//     Floris van Riel
//   Description:
//     Audio portion of the engine
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _XAUDIO_H_
#define _XAUDIO_H_

// Chunk identifiers
#ifdef _XBOX //Big-Endian
#define CID_RIFF 'RIFF'
#define CID_DATA 'data'
#define CID_FMT  'fmt '
#define CID_WAVE 'WAVE'
#define CID_XWMA 'XWMA'
#define CID_DPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define CID_RIFF 'FFIR'
#define CID_DATA 'atad'
#define CID_FMT  ' tmf'
#define CID_WAVE 'EVAW'
#define CID_XWMA 'AMWX'
#define CID_DPDS 'sdpd'
#endif

// Linking
#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "dxguid.lib")

// Includes
#include <xaudio2.h>
#include <fstream>
using namespace std;

class Audio {
public:
	Audio();
	Audio(const Audio& other);
	~Audio();

	bool Initialize();
	void Shutdown();

	bool LoadWavFromFile(WCHAR*);
	bool PlayWavFile();

private:
	IXAudio2* m_xAudio;
	IXAudio2MasteringVoice* m_masterVoice;
	IXAudio2SourceVoice* m_sourceVoice;
	
	WAVEFORMATEX m_waveFormat;
	XAUDIO2_BUFFER m_buffer;
	BYTE* m_data;
};

#endif