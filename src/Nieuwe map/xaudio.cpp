// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     xaudio.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Methods for playing ingame sound
//
// An AP product (c) 2016
// ------------------------------------ //

#include "xaudio.h"

Audio::Audio() {
	m_xAudio = 0;
	m_masterVoice = 0;
	m_sourceVoice = 0;
	m_data = 0;

	ZeroMemory(&m_buffer, sizeof(m_buffer));
	ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));
}

Audio::Audio(const Audio& other) {
}

Audio::~Audio() {
}

bool Audio::Initialize() {
	HRESULT result;

	// Create the core audio engine component
	result = XAudio2Create(&m_xAudio, NULL, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(result)) {
		return false;
	}

	// Create the mastering voice that assembles the audio output
	result = m_xAudio->CreateMasteringVoice(&m_masterVoice);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void Audio::Shutdown() {
	// Release the source voice pointer
	if (m_sourceVoice) {
		m_sourceVoice->Stop();
		m_sourceVoice->DestroyVoice();
		m_sourceVoice = 0;
	}

	// Release the master voice pointer
	if (m_masterVoice) {
		m_masterVoice->DestroyVoice();
		m_masterVoice = 0;
	}

	// Destroy the wave data
	if (m_data) {
		delete[] m_data;
		m_data = 0;
	}

	// Release the core audio engine pointer
	if (m_xAudio) {
		m_xAudio->Release();
		m_xAudio = 0;
	}

	return;
}

bool Audio::LoadWavFromFile(WCHAR* filename) {
	DWORD chunkId, fileSize, chunkSize, extra;
	unsigned int i;
	bool done;

	done = false;

	chunkId = 0;
	fileSize = 0;
	chunkSize = 0;
	extra = 0;

	if (!filename) {
		return false;
	}

	ifstream fin(filename, ios::binary | ios::in);
	if (fin.bad()) {
		return false;
	}

	fin.seekg(0, ios::beg);
	fin.read(reinterpret_cast<char*>(&chunkId), sizeof(chunkId));
	if (chunkId != CID_RIFF) {
		fin.close();
		return false;
	}

	fin.seekg(4, ios::beg);
	fin.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
	if (fileSize <= 16) {
		fin.close();
		return false;
	}

	fin.seekg(8, ios::beg);
	fin.read(reinterpret_cast<char*>(&extra), sizeof(extra));
	if (extra != CID_WAVE) {
		fin.close();
		return false;
	}

	for (i = 12; i < fileSize; ) {
		fin.seekg(i, ios::beg);
		fin.read(reinterpret_cast<char*>(&chunkId), sizeof(chunkId));
		fin.seekg(i + 4, ios::beg);
		fin.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));
		if (chunkId == CID_FMT) {
			fin.seekg(i + 8, ios::beg);
			fin.read(reinterpret_cast<char*>(&m_waveFormat), sizeof(m_waveFormat));
			done = true;
			break;
		}

		chunkSize += 8;
		chunkSize += 1;
		chunkSize &= 0xfffffffe;
		i += chunkSize;
	}

	if (!done) {
		fin.close();
		return false;
	}

	done = false;

	for (unsigned int i = 12; i < fileSize; ) {
		fin.seekg(i, std::ios::beg);
		fin.read(reinterpret_cast<char*>(&chunkId), sizeof(chunkId));
		fin.seekg(i + 4, std::ios::beg);
		fin.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize));

		if (chunkId == CID_DATA) {
			m_data = new BYTE[chunkSize];
			if (!m_data) {
				fin.close();
				return false;
			}
			fin.seekg(i + 8, std::ios::beg);
			fin.read(reinterpret_cast<char*>(m_data), chunkSize);
			m_buffer.AudioBytes = chunkSize;
			m_buffer.pAudioData = m_data;
			m_buffer.PlayBegin = 0;
			m_buffer.PlayLength = 0;
			done = true;
			break;
		}

		chunkSize += 8; //add offsets of the chunk id, and chunk size data entries
		chunkSize += 1;
		chunkSize &= 0xfffffffe; //guarantees WORD padding alignment
		i += chunkSize;
	}

	if (!done) {
		fin.close();
		return false;
	}

	fin.close();
	
	return true;
}

bool Audio::PlayWavFile() {
	HRESULT result;

	result = m_xAudio->CreateSourceVoice(&m_sourceVoice, &m_waveFormat);
	if (FAILED(result)) {
		return false;
	}

	result = m_sourceVoice->Start();
	if (FAILED(result)) {
		return false;
	}

	result = m_sourceVoice->SubmitSourceBuffer(&m_buffer);
	if (FAILED(result)) {
		return false;
	}

	return true;
}