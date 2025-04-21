#include "Low/Client.h"
#include "Low/Platform_Win32.cpp"

#include "Low/Interface.h"
#include "Low/Interface.cpp"

#include "Map.hpp"
#include "Map.cpp"
#include "Content.cpp"

#if _DEBUG
//#include "Debug.cpp"
#endif

#include "Automapping.cpp"

#include "Camera.hpp"
#include "Camera.cpp"
#include "Render.cpp"

#include "Navigation.hpp"
#include "Navigation.cpp"

#include "Player.hpp"

#include "Game.hpp"
#include "Game.cpp"

#include "Player.cpp"

static client_t *Client;
static assets_t *Assets;
static graphics_device_t *Device;

static content_t Content;
static game_state_t GameState;
static interface_t UserInterface;

#define COMPILE_EDITOR

#ifdef COMPILE_EDITOR
#include "Editor/Editor.hpp"
#include "Editor/Editor.cpp"

static editor_t EditorState;
#endif

extern int main(void)
{
	Client = CreateClient("Project 3");

//	SetVideoMode(Client, 640, 420);
//	SetVideoMode(Client, 1600, 900);
	SetVideoMode(Client, 1280, 720);

	Assets = GetAssets(Client);
	Device = GetGraphicsDevice(Client);

	MountDirectory(Assets, "Assets/");
	SetDefaultFont(Assets, LoadFontFromDirectory(Assets, "inconsolata.fnt"));

	LoadGameContent(Assets, &Content);
	//

	InitializeUserInterface(&UserInterface);
	InitializeGame(&GameState, &Content);

	while (1)
	{
		// ...
		input_t Input = {};
		int32_t ExitCode = Host(Client, &Input);
		Input.DeltaTime = Clamp(Input.DeltaTime, 0.0f, 1.0f);

		if (!ExitCode)
			break;

		// Begin

		BeginUserInterface(&UserInterface, &Input, Assets);

		
		// ...
		const camera_t *DebugCamera = 0;

		#ifdef COMPILE_EDITOR
		Editor(&GameState, &EditorState, &Input, &UserInterface);
		DebugCamera = GetActiveEditorCamera(&EditorState);
		#endif

		RunFrame(&GameState, &Input, Device, DebugCamera);

		BeginWindow(&UserInterface, 0, {}, {400.0f, 400.0f}, "Window 1");
		Button(&UserInterface, "Button 1");
		Text(&UserInterface, "Test test test test test test test test test ttttttttttttttttttt test", Point(50,50), V4(5), 1);

		EndWindow(&UserInterface);

		EndUserInterface(&UserInterface);

		Present(Client);

		RenderGUI(&UserInterface, GetGraphicsDevice(Client));

		Display(Client);
	}
	return (0);
}