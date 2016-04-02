// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     medfound.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Implements the Media Foundation
//     API for video playback
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "medfound.h"

#pragma region Globals

template <class Q>
HRESULT GetEventObject(IMFMediaEvent* mEvent, Q** object) {
	PROPVARIANT var;
	HRESULT result;
	
	*object = 0;

	result = mEvent->GetValue(&var);
	if (SUCCEEDED(result)) {
		if (var.vt = VT_UNKNOWN) {
			result = var.punkVal->QueryInterface(object);
		} else {
			result = MF_E_INVALIDTYPE;
		}

		PropVariantClear(&var);
	}

	return result;
}

HRESULT CreateMediaSource(PCWSTR url, IMFMediaSource** source) {
	MF_OBJECT_TYPE objectType;
	IMFSourceResolver* sourceResolver;
	IUnknown* source2;
	HRESULT result;

	objectType = MF_OBJECT_INVALID;
	sourceResolver = 0;
	source2 = 0;
	
	result = MFCreateSourceResolver(&sourceResolver);
	if (FAILED(result)) {
		goto done;
	}

	result = sourceResolver->CreateObjectFromURL(url, MF_RESOLUTION_MEDIASOURCE, NULL, &objectType, &source2);
	if (FAILED(result)) {
		goto done;
	}

	result = source2->QueryInterface(IID_PPV_ARGS(source));

done:
	if (sourceResolver) {
		sourceResolver->Release();
		sourceResolver = 0;
	}

	if (source2) {
		source2->Release();
		source2 = 0;
	}

	return result;
}

HRESULT CreateMediaSinkActivate(IMFStreamDescriptor* sourceSD, HWND videoHwnd, IMFActivate** activate) {
	IMFMediaTypeHandler* handler;
	IMFActivate* activate2;
	GUID guidMajorType;
	HRESULT result;

	handler = 0;
	activate2 = 0;

	result = sourceSD->GetMediaTypeHandler(&handler);
	if (FAILED(result)) {
		goto done;
	}

	result = handler->GetMajorType(&guidMajorType);
	if (FAILED(result)) {
		goto done;
	}

	if (MFMediaType_Audio == guidMajorType) {
		result = MFCreateAudioRendererActivate(&activate2);
	} else if (MFMediaType_Video == guidMajorType) {
		result = MFCreateVideoRendererActivate(videoHwnd, &activate2);
	} else {
		result = E_FAIL;
	}

	if (FAILED(result)) {
		goto done;
	}

	*activate = activate2;
	(*activate)->AddRef();

done:
	if (handler) {
		handler->Release();
		handler = 0;
	}

	if (activate2) {
		activate2->Release();
		activate2 = 0;
	}

	return result;
}

HRESULT AddSourceNode(IMFTopology* topology, IMFMediaSource* source, IMFPresentationDescriptor* PD, IMFStreamDescriptor* SD, IMFTopologyNode** node) {
	IMFTopologyNode* node2;
	HRESULT result;

	node2 = 0;

	result = MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &node2);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetUnknown(MF_TOPONODE_SOURCE, source);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, PD);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, SD);
	if (FAILED(result)) {
		goto done;
	}

	result = topology->AddNode(node2);
	if (FAILED(result)) {
		goto done;
	}

	*node = node2;
	(*node)->AddRef();

done:
	if (node2) {
		node2->Release();
		node2 = 0;
	}

	return result;
}

HRESULT AddOutputNode(IMFTopology* topology, IMFActivate* activate, DWORD Id, IMFTopologyNode** node) {
	IMFTopologyNode* node2;
	HRESULT result;

	node2 = 0;

	result = MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &node2);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetObject(activate);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetUINT32(MF_TOPONODE_STREAMID, Id);
	if (FAILED(result)) {
		goto done;
	}

	result = node2->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	if (FAILED(result)) {
		goto done;
	}

	result = topology->AddNode(node2);
	if (FAILED(result)) {
		goto done;
	}

	*node = node2;
	(*node)->AddRef();

done:
	if (node2) {
		node2->Release();
		node2 = 0;
	}

	return result;
}

HRESULT AddBranchToPartialTopology(IMFTopology* topology, IMFMediaSource* source, IMFPresentationDescriptor* PD, DWORD stream, HWND videoHwnd) {
	IMFStreamDescriptor* SD;
	IMFActivate* activate;
	IMFTopologyNode* sourceNode;
	IMFTopologyNode* outputNode;
	BOOL selected;
	HRESULT result;

	SD = 0;
	activate = 0;
	sourceNode = 0;
	outputNode = 0;
	selected = FALSE;

	result = PD->GetStreamDescriptorByIndex(stream, &selected, &SD);
	if (FAILED(result)) {
		goto done;
	}

	if (selected) {
		result = CreateMediaSinkActivate(SD, videoHwnd, &activate);
		if (FAILED(result)) {
			goto done;
		}

		result = AddSourceNode(topology, source, PD, SD, &sourceNode);
		if (FAILED(result)) {
			goto done;
		}

		result = AddOutputNode(topology, activate, 0, &outputNode);
		if (FAILED(result)) {
			goto done;
		}

		result = sourceNode->ConnectOutput(0, outputNode, 0);
	}

done:
	if (SD) {
		SD->Release();
		SD = 0;
	}

	if (activate) {
		activate->Release();
		activate = 0;
	}

	if (sourceNode) {
		sourceNode->Release();
		sourceNode = 0;
	}

	if (outputNode) {
		outputNode->Release();
		outputNode = 0;
	}

	return result;
}

HRESULT CreatePlaybackTopology(IMFMediaSource* source, IMFPresentationDescriptor* PD, HWND videoHwnd, IMFTopology** topology) {
	IMFTopology* topology2;
	DWORD sourceStreams, i;
	HRESULT result;

	topology2 = 0;
	sourceStreams = 0;

	result = MFCreateTopology(&topology2);
	if (FAILED(result)) {
		goto done;
	}

	result = PD->GetStreamDescriptorCount(&sourceStreams);
	if (FAILED(result)) {
		goto done;
	}

	for (i = 0; i < sourceStreams; i++) {
		result = AddBranchToPartialTopology(topology2, source, PD, i, videoHwnd);
		if (FAILED(result)) {
			goto done;
		}
	}

	*topology = topology2;
	(*topology)->AddRef();

done:
	if (topology2) {
		topology2->Release();
		topology2 = 0;
	}

	return result;
}

#pragma endregion

// CPlayer member functions

CPlayer::CPlayer(HWND videoHwnd, HWND eventHwnd) {
	m_session = 0;
	m_source = 0;
	m_videoDisplay = 0;
	m_hwndVideo = videoHwnd;
	m_hwndEvent = eventHwnd;
	m_state = PBS_CLOSED;
	m_closeEvent = 0;
	m_refCount = 1;
}

CPlayer::~CPlayer() {
	assert(!m_session);

	Shutdown();
}

HRESULT CPlayer::CreateInstance(HWND videoHwnd, HWND eventHwnd, CPlayer** player) {
	HRESULT result;
	
	if (!player) {
		return E_POINTER;
	}

	CPlayer* player2 = new (std::nothrow) CPlayer(videoHwnd, eventHwnd);
	if (!player2) {
		return E_OUTOFMEMORY;
	}

	result = player2->Initialize();
	if (SUCCEEDED(result)) {
		*player = player2;
	} else {
		player2->Release();
	}

	return result;
}

HRESULT CPlayer::Initialize() {
	HRESULT result;

	result = MFStartup(MF_VERSION);
	if (SUCCEEDED(result)) {
		m_closeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (!m_closeEvent) {
			result = HRESULT_FROM_WIN32(GetLastError());
		}
	}

	return result;
}

HRESULT CPlayer::OpenURL(const WCHAR* url) {
	IMFTopology* topology;
	IMFPresentationDescriptor* sourcePD;
	HRESULT result;

	topology = 0;
	sourcePD = 0;

	result = CreateSession();
	if (FAILED(result)) {
		goto done;
	}

	result = CreateMediaSource(url, &m_source);
	if (FAILED(result)) {
		goto done;
	}

	result = m_source->CreatePresentationDescriptor(&sourcePD);
	if (FAILED(result)) {
		goto done;
	}

	result = CreatePlaybackTopology(m_source, sourcePD, m_hwndVideo, &topology);
	if (FAILED(result)) {
		goto done;
	}

	result = m_session->SetTopology(0, topology);
	if (FAILED(result)) {
		goto done;
	}

	m_state = PBS_OPENPENDING;

	m_state = PBS_STOPPED;

done:
	if (FAILED(result)) {
		m_state = PBS_CLOSED;
	}

	if (sourcePD) {
		sourcePD->Release();
		sourcePD = 0;
	}

	if (topology) {
		topology->Release();
		topology = 0;
	}

	return result;
}

HRESULT CPlayer::CreateSession() {
	HRESULT result;

	result = CloseSession();
	if (FAILED(result)) {
		goto done;
	}

	assert(m_state == PBS_CLOSED);

	result = MFCreateMediaSession(NULL, &m_session);
	if (FAILED(result)) {
		goto done;
	}

	result = m_session->BeginGetEvent((IMFAsyncCallback*)this, NULL);
	if (FAILED(result)) {
		goto done;
	}

	m_state = PBS_READY;

done:
	return result;
}

HRESULT CPlayer::QueryInterface(REFIID riid, void** v) {
	static const QITAB qit[] = {
		QITABENT(CPlayer, IMFAsyncCallback),
		{ 0 }
	};

	return QISearch(this, qit, riid, v);
}

ULONG CPlayer::AddRef() {
	return InterlockedIncrement(&m_refCount);
}

ULONG CPlayer::Release() {
	ULONG count = InterlockedDecrement(&m_refCount);
	if (count == 0) {
		delete this;
	}

	return count;
}

HRESULT CPlayer::Invoke(IMFAsyncResult* result) {
	MediaEventType meType;
	IMFMediaEvent* mEvent;
	HRESULT hresult;

	meType = MEUnknown;
	mEvent = 0;

	hresult = m_session->EndGetEvent(result, &mEvent);
	if (FAILED(hresult)) {
		goto done;
	}

	hresult = mEvent->GetType(&meType);
	if (FAILED(hresult)) {
		goto done;
	}

	if (meType == MESessionClosed) {
		SetEvent(m_closeEvent);
	} else {
		hresult = m_session->BeginGetEvent(this, NULL);
		if (FAILED(hresult)) {
			goto done;
		}
	}

	if (m_state != PBS_CLOSING) {
		mEvent->AddRef();
		SendMessage(m_hwndEvent, NGM_PLAYER_EVENT, (WPARAM)mEvent, (LPARAM)meType);
		// PostMessage(m_hwndEvent, WM_APP_PLAYER_EVENT, (WPARAM)mEvent, (LPARAM)meType);
	}

done:
	if (mEvent) {
		mEvent->Release();
		mEvent = 0;
	}

	return S_OK;
}

HRESULT CPlayer::HandleEvent(UINT_PTR eventPtr) {
	MediaEventType meType;
	IMFMediaEvent* mEvent;
	HRESULT result, hrStatus;

	meType = MEUnknown;
	mEvent = (IMFMediaEvent*)eventPtr;
	hrStatus = S_OK;

	if (!mEvent) {
		return E_POINTER;
	}

	result = mEvent->GetType(&meType);
	if (FAILED(result)) {
		goto done;
	}

	result = mEvent->GetStatus(&hrStatus);
	if (SUCCEEDED(result) && FAILED(hrStatus)) {
		result = hrStatus;
	}

	if (FAILED(result)) {
		goto done;
	}

	switch (meType) {
		case MESessionTopologyStatus:
			result = OnTopologyStatus(mEvent);
			break;
			
		case MEEndOfPresentation:
			result = OnPresentationEnded(mEvent);
			break;

		case MENewPresentation:
			result = OnNewPresentation(mEvent);
			break;

		default:
			result = OnSessionEvent(mEvent, meType);
			break;
	}

done:
	if (mEvent) {
		mEvent->Release();
		mEvent = 0;
	}

	return result;
}

HRESULT CPlayer::OnTopologyStatus(IMFMediaEvent* mEvent) {
	UINT32 status;
	HRESULT result;

	result = mEvent->GetUINT32(MF_EVENT_TOPOLOGY_STATUS, &status);
	if (SUCCEEDED(result) && (status == MF_TOPOSTATUS_READY)) {
		if (m_videoDisplay) {
			m_videoDisplay->Release();
			m_videoDisplay = 0;
		}

		(void)MFGetService(m_session, MR_VIDEO_RENDER_SERVICE, IID_PPV_ARGS(&m_videoDisplay));

		result = StartPlayback();
	}

	return result;
}

HRESULT CPlayer::OnPresentationEnded(IMFMediaEvent* mEvent) {
	HRESULT result;

	result = S_OK;

	if (m_state == PBS_STARTED) {
		if (SUCCEEDED(result)) {
			PostMessage(m_hwndEvent, NGM_CHANGE_STATE, (WPARAM)GS_MAINMENU, 0);
		}
	}

	m_state = PBS_STOPPED;

	return S_OK;
}

HRESULT CPlayer::OnNewPresentation(IMFMediaEvent* mEvent) {
	IMFPresentationDescriptor* PD;
	IMFTopology* topology;
	HRESULT result;

	PD = 0;
	topology = 0;

	result = GetEventObject(mEvent, &PD);
	if (FAILED(result)) {
		goto done;
	}

	result = CreatePlaybackTopology(m_source, PD, m_hwndVideo, &topology);
	if (FAILED(result)) {
		goto done;
	}

	result = m_session->SetTopology(0, topology);
	if (FAILED(result)) {
		goto done;
	}

	m_state = PBS_OPENPENDING;

done:
	if (topology) {
		topology->Release();
		topology = 0;
	}

	if (PD) {
		PD->Release();
		PD = 0;
	}

	return S_OK;
}

HRESULT CPlayer::StartPlayback() {
	PROPVARIANT varStart;
	HRESULT result;

	assert(m_session);
	PropVariantInit(&varStart);

	result = m_session->Start(&GUID_NULL, &varStart);
	if (SUCCEEDED(result)) {
		m_state = PBS_STARTED;
	}

	PropVariantClear(&varStart);

	return result;
}

HRESULT CPlayer::Play() {
	if (m_state != PBS_PAUSED && m_state != PBS_STOPPED) {
		return MF_E_INVALIDREQUEST;
	}

	if (!m_session || !m_source) {
		return E_UNEXPECTED;
	}

	return StartPlayback();
}

HRESULT CPlayer::Pause() {
	HRESULT result;

	if (m_state != PBS_STARTED) {
		return MF_E_INVALIDREQUEST;
	}

	if (!m_session || !m_source) {
		return E_UNEXPECTED;
	}

	result = m_session->Pause();
	if (SUCCEEDED(result)) {
		m_state = PBS_PAUSED;
	}

	return result;
}

HRESULT CPlayer::Stop() {
	HRESULT result;
	
	if (m_state != PBS_STARTED && m_state != PBS_PAUSED) {
		return MF_E_INVALIDREQUEST;
	}

	if (!m_session) {
		return E_UNEXPECTED;
	}

	result = m_session->Stop();
	if (SUCCEEDED(result)) {
		m_state = PBS_STOPPED;
	}

	return result;
}

HRESULT CPlayer::Repaint() {
	if (m_videoDisplay) {
		return m_videoDisplay->RepaintVideo();
	} else {
		return S_OK;
	}
}

HRESULT CPlayer::ResizeVideo(WORD width, WORD height) {
	if (m_videoDisplay) {
		RECT rc = { 0, 0, width, height };

		return m_videoDisplay->SetVideoPosition(NULL, &rc);
	} else {
		return S_OK;
	}
}

HRESULT CPlayer::CloseSession() {
	DWORD waitResult;
	HRESULT result;

	result = S_OK;
	waitResult = 0;

	if (m_session) {
		m_state = PBS_CLOSING;

		result = m_session->Close();
		if (SUCCEEDED(result)) {
			waitResult = WaitForSingleObject(m_closeEvent, 5000);
			if (waitResult == WAIT_TIMEOUT) {
				assert(FALSE);
			}
		}
	}

	if (SUCCEEDED(result)) {
		if (m_source) {
			(void)m_source->Shutdown();
		}

		if (m_session) {
			(void)m_session->Shutdown();
		}
	}

	if (m_source) {
		m_source->Release();
		m_source = 0;
	}

	if (m_session) {
		m_session->Release();
		m_session = 0;
	}

	m_state = PBS_CLOSED;

	return result;
}

HRESULT CPlayer::Shutdown() {
	HRESULT result;

	result = CloseSession();

	MFShutdown();

	if (m_closeEvent) {
		CloseHandle(m_closeEvent);
		m_closeEvent = 0;
	}

	return result;
}