#include "gamex.h"

CMD g_pszCmd = L"";
GAMESTATE GS_CURRENT = GS_NOTSPECIFIED;
HWND g_hwnd;

CMD_RESULT ExecuteCommand(CMD pCommand, WCHAR* &pOutput) {	
	ARG argument;
	BOOL result;

	result = TRUE;

	argument = 0;
	pOutput = 0;

	if (wcscmp(g_pszCmd, CMD_HELLO_WORLD) == 0) {
		pOutput = L"Hello world!";
		goto done;
	}

	if (wcscmp(g_pszCmd, CMD_GAME) == 0) {
		result = CMD_GET_ARGS(0, pCommand, (&argument)[0]);
		if (result == FALSE || !argument) {
			goto done;
		}

		if (wcscmp(argument, L"start") == 0) {
			pOutput = L"Starting game in a moment...";
			SendMessage(g_hwnd, NGM_CHANGE_STATE, (WPARAM)GS_INGAME, 0);
			goto done;
		}
	}

	if (wcscmp(g_pszCmd, CMD_EXIT) == 0) {
		pOutput = L"Exiting...";
		PostQuitMessage(0);
		goto done;
	}

done:
	if (argument) {
		delete[] argument;
		argument = 0;
	}

	if (result == FALSE) {
		return CMD_FAIL;
	}
	
	return CMD_SUCCESS;;
}

BOOL StrToWCS(std::vector<WCHAR> vString, WCHAR** ppszWCS) {
	return FALSE;
}

BOOL GetArgument(
	unsigned int _index,
	CMD pCommand,
	ARG &pArgument
	)
{
	unsigned int i,
		cmdSize,
		startIndex,
		endIndex,
		count;

	if (!pCommand) {
		return FALSE;
	}

	cmdSize = wcslen(pCommand) + 1;
	startIndex = 0;
	endIndex = 0;
	count = 0;

	if (pArgument) {
		delete[] pArgument;
		pArgument = 0;
	}

	if (wcslen(pCommand) == 0) {
		return FALSE;
	}

	for (i = 0; i < cmdSize; ++i) {
		if (pCommand[i] == ' ' || pCommand[i] == '\0') {
			if (count == _index) {
				if (startIndex == 0) {
					startIndex = i + 1;
					continue;
				} else {
					endIndex = i - 1;
					break;
				}
			} else {
				++count;
			}
		}
	}

	if (startIndex == 0 || endIndex == 0) {
		return FALSE;
	}

	pArgument = new WCHAR[endIndex - startIndex + 2];
	if (!pArgument) {
		return FALSE;
	}

	for (i = startIndex; i <= endIndex; ++i) {
		pArgument[i - startIndex] = pCommand[i];
	}

	pArgument[endIndex - startIndex + 1] = '\0';

	return TRUE;
}

BOOL GetCommand(CMD pCommand) {
	unsigned int i, size, endIndex;

	size = wcslen(pCommand);
	if (size == 0) {
		return FALSE;
	}

	endIndex = 0;

	for (i = 0; i < size; ++i) {
		if (pCommand[i] == ' ') {
			endIndex = i - 1;
			break;
		}
	}

	if (endIndex == 0) {
		endIndex = size - 1;
	}

	if (g_pszCmd) {
		g_pszCmd = 0;
	}

	g_pszCmd = new WCHAR[endIndex + 2];
	if (!g_pszCmd) {
		return FALSE;
	}

	for (i = 0; i <= endIndex; ++i) {
		g_pszCmd[i] = pCommand[i];
	}

	g_pszCmd[endIndex + 1] = '\0';

	return TRUE;
}