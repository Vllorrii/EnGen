// Includes
#include "cmdline.h"

CommandLine* g_CmdLine = 0;
std::vector<WCHAR> g_Line;
WCHAR* g_Command = 0;

CommandLine::CommandLine() {
	m_DWrite = 0;
	g_CmdLine = 0;
	g_Command = 0;
}

CommandLine::CommandLine(const CommandLine& other) {
}

CommandLine::~CommandLine() {
}

bool CommandLine::Initialize(HWND hwnd) {
	HRESULT hresult;
	bool result;

	m_hwnd = hwnd;

	m_DWrite = new DWrite;
	hresult = m_DWrite->Initialize(m_hwnd);
	if (FAILED(hresult)) {
		return false;
	}

	result = Write(L"EnGen v1.0 Interpreter\nCopyright AP 2016\n\nFor a list of commands, type help\n>");
	if (!result) {
		return false;
	}

	g_CmdLine = this;
	if (!g_CmdLine) {
		return false;
	}

	return true;
}

void CommandLine::Shutdown() {	
	if (m_DWrite) {
		m_DWrite->Shutdown();
		delete m_DWrite;
		m_DWrite = 0;
	}

	if (g_Command) {
		delete[] g_Command;
		g_Command = 0;
	}

	if (g_CmdLine) {
		g_CmdLine = 0;
	}

	return;
}

bool CommandLine::Write(WCHAR* string) {
	if (!string) {
		return false;
	}

	WCHAR* text;
	unsigned int i, sizeText, sizeString;
	bool result;

	text = 0;
	
	sizeString = wcslen(string);
	for (i = 0; i < sizeString; ++i) {
		m_text.push_back(string[i]);
	}
	sizeText = m_text.size();

	text = new WCHAR[sizeText + 1];
	if (!text) {
		return false;
	}

	for (i = 0; i < sizeText; ++i) {
		text[i] = m_text[i];
	}

	text[sizeText] = '\0';

	result = m_DWrite->RenderText(text);
	
	if (text) {
		delete[] text;
		text = 0;
	}

	if (!result) {
		return false;
	}

	return true;
}

bool CommandLine::Input(WCHAR character) {
	if (!character) {
		return false;
	}

	/* if (!isalnum(character)) {
		return false;
	} */

	WCHAR *text, *cmd;
	unsigned int i, sizeText, sizeCmd;
	bool result;

	text = 0;
	cmd = 0;
	result = true;

	if (character == '\b') {
		if (m_text.size() >= 1 && g_Line.size() >= 1) {
			m_text.pop_back();
			g_Line.pop_back();
		}
	} else {
		m_text.push_back(character);
		g_Line.push_back(character);
	}
	sizeText = m_text.size();
	sizeCmd = g_Line.size();

	text = new WCHAR[sizeText + 1];
	if (!text) {
		goto done;
	}

	cmd = new WCHAR[sizeCmd + 1];
	if (!cmd) {
		goto done;
	}

	for (i = 0; i < sizeText; ++i) {
		text[i] = m_text[i];
	}

	text[sizeText] = '\0';
	
done:
	result = m_DWrite->RenderText(text);

	if (text) {
		delete[] text;
		text = 0;
	}

	if (cmd) {
		delete[] cmd;
		cmd = 0;
	}

	if (!result) {
		return false;
	}

	return true;
}

/*bool CommandLine::Render() {
	WCHAR* text;
	unsigned int i, size;
	bool result;

	size = m_text.size();
	text = new WCHAR[size];

	for (i = 0; i < size; ++i) {
		text[i] = m_text[i];
	}

	result = m_DWrite->RenderText(text);
	if (!result) {
		return false;
	}

	return true;
}*/

bool ConvertLineToCommand(std::vector<WCHAR> string) {
	if (string.size() <= 0) {
		return false;
	}
	
	unsigned int i, size;

	g_Command = 0;

	size = string.size();
	g_Command = new WCHAR[size + 1];
	if (!g_Command) {
		return false;
	}

	for (i = 0; i < size; ++i) {
		g_Command[i] = string[i];
	}

	g_Command[size] = '\0';

	return true;
}

void OnCmdKeyPress(WPARAM wParam, HWND hwnd) {
	if (GS_CURRENT != GS_MAINMENU) {
		return;
	}

	if (wParam == VK_ESCAPE) {
		g_CmdLine->Write(L"\nPressed Escape firmly\n>");
		return;
	}

	if (wParam == VK_BACK) {
		g_CmdLine->Input('\b');
		return;
	}
	
	CMD_RESULT cmdresult;
	WCHAR* output;

	output = 0;
	
	if (!g_CmdLine) {
		return;
	}

	if (wParam == VK_RETURN) {
		if (!ConvertLineToCommand(g_Line)) {
			return;
		}

		g_CmdLine->Write(L"\n");

		CMD_GET_COMMAND(g_Command);
		cmdresult = CMD_DISPATCH(g_Command, (&output)[0]);
		if (CMD_FAILED(cmdresult) || !output) {
			g_CmdLine->Write(L"Error executing command. Please check syntax.");
		}

		g_CmdLine->Write(output);

		g_CmdLine->Write(L"\n>");

		g_Line.clear();

		/*cmdresult = OnCmdExecute(g_Command, hwnd);
		if (CMD_FAILED(cmdresult)) {
			return;
		}*/

		return;
	}

	g_CmdLine->Input(static_cast<WCHAR>(wParam));

	if (output) {
		delete[] output;
		output = 0;
	}

	return;
}

CMD_RESULT OnCmdExecute(WCHAR* cmd, HWND hwnd) {
	if (!g_CmdLine->Write(L"\n")) return CMD_OUTPUT_ERROR;

	if (wcscmp(cmd, L"helloworld") == 0) {
		if (!g_CmdLine->Write(L"Executing hello world command...")) return CMD_OUTPUT_ERROR;
		MessageBox(hwnd, L"Hello World!", L"[DEBUG]", MB_OK);
	} else if (wcscmp(cmd, L"help") == 0) {
		if (!g_CmdLine->Write(L"A list of available commands:\n\t- exit\n\t- help\n\t- helloworld")) return CMD_OUTPUT_ERROR;
	} else if (wcscmp(cmd, L"exit") == 0) {
		if (!g_CmdLine->Write(L"Exiting the program...")) return CMD_OUTPUT_ERROR;

		PostQuitMessage(0);
	} else if (wcscmp(cmd, L"game") == 0) {
		ARG argument = 0;
		if (!CMD_GET_ARGS(0, g_Command, argument)) return CMD_FAIL;

		if (wcscmp(argument, L"start") == 0) {

		}

		if (argument) {
			delete[] argument;
			argument = 0;
		}

	} else {
		if (!g_CmdLine->Write(L"ERROR: No such command found. Maybe you made a typo?")) return CMD_OUTPUT_ERROR;
	}

	if (!g_CmdLine->Write(L"\n>")) return CMD_OUTPUT_ERROR;

	g_Line.clear();

	return CMD_SUCCESS;
}