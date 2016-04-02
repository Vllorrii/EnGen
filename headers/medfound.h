// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     medfound.h
//   Author:
//     Floris van Riel
//   Description:
//     Methods for rendering video playback
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _MEDFOUND_H_
#define _MEDFOUND_H_

// Linking
#pragma comment(lib, "shlwapi")

// Includes
#include <new>
#include <ShObjIdl.h>
#include <Shlwapi.h>
#include <assert.h>
#include <strsafe.h>

#include <mfapi.h>
#include <mfidl.h>
#include <Mferror.h>
#include <evr.h>

#include "gamex.h"

// Defined constants
#define IDD_MFPLAYBACK_DIALOG 102
#define IDM_EXIT 105
#define IDC_MFPLAYBACK 109
#define IDD_OPENURL 129
#define IDC_EDIT_URL 1000
#define ID_FILE_OPENFILE 32771
#define ID_FILE_OPENURL 32772
#define IDC_STATIC -1

// Playback states
enum PlaybackState {
	PBS_CLOSED = 0,
	PBS_READY,
	PBS_OPENPENDING,
	PBS_STARTED,
	PBS_PAUSED,
	PBS_STOPPED,
	PBS_CLOSING
};

class CPlayer : public IMFAsyncCallback {
public:
	static HRESULT CreateInstance(HWND, HWND, CPlayer**);

	STDMETHODIMP QueryInterface(REFIID, void**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP GetParameters(DWORD*, DWORD*) { return E_NOTIMPL; }

	STDMETHODIMP Invoke(IMFAsyncResult*);

	HRESULT OpenURL(const WCHAR*);
	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	HRESULT Shutdown();
	HRESULT HandleEvent(UINT_PTR);
	PlaybackState GetState() const { return m_state; }

	HRESULT Repaint();
	HRESULT ResizeVideo(WORD, WORD);

	BOOL HasVideo() const { return (m_videoDisplay != 0); }

protected:
	CPlayer(HWND, HWND);
	virtual ~CPlayer();

	HRESULT Initialize();
	HRESULT CreateSession();
	HRESULT CloseSession();
	HRESULT StartPlayback();

	virtual HRESULT OnTopologyStatus(IMFMediaEvent*);
	virtual HRESULT OnPresentationEnded(IMFMediaEvent*);
	virtual HRESULT OnNewPresentation(IMFMediaEvent*);

	virtual HRESULT OnSessionEvent(IMFMediaEvent*, MediaEventType) {
		return S_OK;
	}

protected:
	long m_refCount;

	IMFMediaSession* m_session;
	IMFMediaSource* m_source;
	IMFVideoDisplayControl* m_videoDisplay;

	HWND m_hwndVideo;
	HWND m_hwndEvent;
	PlaybackState m_state;
	HANDLE m_closeEvent;
};

#endif