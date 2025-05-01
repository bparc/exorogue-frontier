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

#include "Render.hpp"
#include "Render.cpp"

#include "Navigation.hpp"
#include "Navigation.cpp"

#include "Game.hpp"
#include "PlayerInterface.hpp"
#include "PlayerInterface.cpp"
#include "Game.cpp"

static client_t *Client;
static assets_t *Assets;
static graphics_device_t *Device;

static content_t Content;
static game_state_t GameState;
//static interface_t UserInterface;

#define COMPILE_EDITOR

#ifdef COMPILE_EDITOR
#include "Editor/Editor.hpp"
#include "Editor/Editor.cpp"

static editor_t EditorState;
#endif

#if 0
static void TestGUI(interface_t *UI)
{

}
#endif

extern int main(void)
{
	Client = CreateClient("Project 3");

//	SetVideoMode(Client, 640, 420);
	SetVideoMode(Client, 1600, 900);
//	SetVideoMode(Client, 1280, 720);

	Assets = GetAssets(Client);
	Device = GetGraphicsDevice(Client);

	MountDirectory(Assets, "Assets/");
	SetDefaultFont(Assets, LoadFontFromDirectory(Assets, "inconsolata.fnt"));

	LoadGameContent(Assets, &Content);
	//

	//InitGUI(&UserInterface);
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

		//BeginGUI(&UserInterface, &Input, Assets);

		
		// ...
		const camera_t *DebugCamera = 0;

		#ifdef COMPILE_EDITOR
		Editor(&GameState, &EditorState, &Input, 0);
		DebugCamera = GetActiveEditorCamera(&EditorState);
		#endif

		RunFrame(&GameState, &Input, Device, DebugCamera);

		//

		//EndGUI(&UserInterface);

		Present(Client);

		//RenderGUI(&UserInterface, GetGraphicsDevice(Client));

		Display(Client);
	}
	return (0);
}