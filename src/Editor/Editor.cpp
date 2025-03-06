static void EditorMenu(editor_t *Editor, bool *CameraActivated, const input_t *Input)
{
	DebugPrint("%.2f MS (%i FPS) | EDIT (%s) (F1)",
		Editor->TimeAvarage * 1000.0f, (int32_t)(1.0f / Editor->TimeAvarage),
		Editor->EditMode ? "ON" : "OFF");

	if (Input->Controllers[0].DebugKeys[0].Down)
	{
		Editor->EditMode = !Editor->EditMode;
		*CameraActivated = true;
	}
}

static const camera_t *GetActiveEditorCamera(editor_t *Editor)
{
	return (Editor->EditMode ? &Editor->Camera : NULL);
}

static void EditorCamera(camera_t *Camera, bool JustActivated, const input_t *Input, camera_t GameCamera)
{
	const controller_t *Con = &Input->Controllers[0];
	vec2_t Dir = Con->Analogs[0];

	vec2_t Offset = Dir * 200.0f * Input->DeltaTime;

	Camera->Center = Camera->Center + Offset;

	if (JustActivated)
	{
		Camera->Center = GameCamera.Center;
	}

	DebugBounds(GetCameraBounds(&GameCamera), RGBA(255, 0, 255));
}

static void InitializeEditor(editor_t *Editor)
{
	
}

static void Editor(game_state_t *State, editor_t *Editor, const input_t *Input, interface_t *GUI)
{
	if (!Editor->Inited)
	{
		InitializeEditor(Editor);

		Editor->Camera = CreateCamera({800.0f / 2.0f, 600.0f / 2.0f});
		Editor->Inited = true;
	}

	float_t PerformanceCounterSampleFrequency = 2;
	Editor->TimeSampleCount++;
	Editor->TimeSamples += Input->DeltaTime;
	if (Editor->TimeSamples >= (1.0f / PerformanceCounterSampleFrequency))
	{
		Editor->TimeAvarage = Editor->TimeSamples / (float_t)Editor->TimeSampleCount;
		Editor->TimeSampleCount = 0;
		Editor->TimeSamples = 0.0f;
	}

	bool CameraJustActivated = false;
	EditorMenu(Editor, &CameraJustActivated, Input);

	if (Editor->EditMode)
	{
		EditorCamera(&Editor->Camera, CameraJustActivated, Input, GetActiveCamera(State));
	}

#if 0
	if (BeginWindow(GUI, 1, V2(10.0f, 10.0f), V2(400.0f, 400.0f), "Window 1"))
	{
		EndWindow(GUI);
	}

	if (BeginWindow(GUI, 2, V2(10.0f, 10.0f), V2(250.0f, 500.0f), "Window 2"))
	{
		EndWindow(GUI);
	}
#endif
}