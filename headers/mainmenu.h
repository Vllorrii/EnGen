#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#include <d2d1_1.h>

class MainMenu {
public:
	MainMenu();
	MainMenu(const MainMenu& other);
	~MainMenu();

	bool Initialize(IDXGISwapChain* pSwapChain);

private:
	
};

#endif