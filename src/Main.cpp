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

static void TestGUI(interface_t *UI)
{
	window_settings_t WindowSettings = {};
	WindowSettings.ElementSpacing = 100.0f;
	WindowSettings.PaddingX = 200.0f;
	WindowSettings.PaddingY = 290.0f;
	WindowSettings.Title = "Window 1";
	WindowSettings.MaxRowHeight = 0;
	BeginWindow(UI, 0, {}, {400.0f, 400.0f}, WindowSettings);
	Button(UI, "Button 1", GetNextWidgetPos(UI, V2(100.0f, 20.0f))); // todo: remove magic numbers from button function, make button size configurable
	Button(UI, "Button 2", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 3", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 4", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 5", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 6", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 7", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Button(UI, "Button 8", GetNextWidgetPos(UI, V2(100.0f, 20.0f)));
	Text(UI, "Lorem ipsum", GetNextWidgetPos(UI, V2(100.0f, 20.0f)), V4(5), 1);
	//Text(UI, "dolor sit amet", GetNextWidgetPos(UI, V2(100.0f, 20.0f)), V4(5), 1);
	//Text(UI, "consectetur adipiscing elit.", GetNextWidgetPos(UI, V2(100.0f, 20.0f)), V4(5), 1);
	EndWindow(UI);
}

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

	InitGUI(&UserInterface);
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

		BeginGUI(&UserInterface, &Input, Assets);

		
		// ...
		const camera_t *DebugCamera = 0;

		#ifdef COMPILE_EDITOR
		Editor(&GameState, &EditorState, &Input, &UserInterface);
		DebugCamera = GetActiveEditorCamera(&EditorState);
		#endif

		RunFrame(&GameState, &Input, Device, DebugCamera);

		//
#if 1
		TestGUI(&UserInterface);
#endif
		//

		EndGUI(&UserInterface);

		Present(Client);

		RenderGUI(&UserInterface, GetGraphicsDevice(Client));

		Display(Client);
	}
	return (0);
}