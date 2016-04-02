#ifndef _GAMEX_H_
#define _GAMEX_H_

// Includes
#include <vector>
#include <Windows.h>

#define WIN32_LEAN_AND_MEAN

#define APP_NAME    "EnGen"
#define APP_VERSION "1.0.0"

#define NGM_PLAYER_EVENT 0x8001
#define NGM_CHANGE_STATE 0x8002

extern HWND g_hwnd;

// ----------------------------------------- //
//           ..:: Command line ::..          //
//                                           //
// - Handles command line input and output   //
//                                           //
// ----------------------------------------- //

typedef unsigned int CMD_RESULT;
typedef WCHAR* CMD;
typedef WCHAR* ARG;

#define CMD_SUCCESS      0x00
#define CMD_FAIL         0x01
#define CMD_NO_INPUT     0x02
#define CMD_OUTPUT_ERROR 0x03

#define CMD_FAILED(cmdr)    (cmdr != 0)
#define CMD_SUCCEEDED(cmdr) (cmdr == 0)

#define CMD_GET_COMMAND GetCommand
#define CMD_DISPATCH ExecuteCommand
#define CMD_GET_ARGS GetArgument

#define CMD_HELLO_WORLD L"helloworld"
#define CMD_GAME L"game"
#define CMD_EXIT L"exit"

CMD_RESULT ExecuteCommand(
	CMD pCommand,
	WCHAR* &pOutput
	);

BOOL StrToWCS(
	std::vector<WCHAR> vString, 
	WCHAR** ppszWCS
	);

BOOL GetArgument(
	unsigned int _index,
	CMD pCommand,
	ARG &pArgument
	);

BOOL GetCommand(
	CMD pCommand
	);

extern CMD g_pszCmd;

// ----------------------------------------- //
//            ..:: Game State ::..           //
//                                           //
// - Enumeration for the game state          //
//                                           //
// ----------------------------------------- //

enum GAMESTATE {
	GS_NOTSPECIFIED = 0,
	GS_MAINMENU,
	GS_INTRO,
	GS_LOADING,
	GS_INGAME,
	GS_CLOSING,
};

extern GAMESTATE GS_CURRENT;

#endif