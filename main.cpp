// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     main.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Contains the WinMain function
//     Initializes the System classes
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "engen.h"

// The WinMain function called at startup
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd) {
	EnGen* engen;
	bool result;

	// Attempt to create the engine
	engen = new EnGen;
	if (!engen) {
		return -1;
	}

	// Initialize the graphical interface
	result = engen->Initialize();
	if (!result) {
		return -1;
	}

	// Run the game until the user is done
	engen->Run();

	// Delete the graphical interface and the engine object
	engen->Shutdown();
	delete engen;
	engen = 0;
	
	return 0;
}