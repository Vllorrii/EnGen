#ifndef _CMDLINE_H_
#define _CMDLINE_H_

// Includes
#include "directwrite.h"
#include "gamex.h"

class CommandLine {
public:
	CommandLine();
	CommandLine(const CommandLine& other);
	~CommandLine();

	bool Initialize(HWND);
	void Shutdown();

	bool Input(WCHAR);
	bool Write(WCHAR*);
	// bool Render();

private:
	DWrite* m_DWrite;
	std::vector<WCHAR> m_text;
	HWND m_hwnd;
};

extern std::vector<WCHAR> g_Line;
extern WCHAR* g_Command;

void OnCmdKeyPress(WPARAM, HWND);
CMD_RESULT OnCmdExecute(WCHAR*, HWND);
bool ConvertLineToCommand(std::vector<WCHAR>);

#endif