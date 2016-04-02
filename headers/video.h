// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     video.h
//   Author:
//     Floris van Riel
//   Description:
//     Render videos to the screen
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _VIDEO_H_
#define _VIDEO_H_

// Includes
#include "medfound.h"

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// extern CPlayer* g_Player;

class Video {
public:
	Video();
	Video(const Video& other);
	~Video();

	bool Initialize(HWND);
	void Shutdown();

	LRESULT OnCreate(HWND);
	void OnPlayerEvent(WPARAM);
	void OnPaint();
	void OnResize(WORD, WORD);
	LRESULT OnDestroy(WPARAM);

private:
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	
private:
	CPlayer* m_player;

	HWND m_hwndEvent;
	HWND m_hwndParent;
	HINSTANCE m_hInstance;
};

static Video* g_VideoHandle = 0;

#endif