
static void Inventory(game_state_t *State, interface_t *UI)
{
	window_settings_t WindowSettings = {};
	WindowSettings.ElementSpacing = 100.0f;
	WindowSettings.PaddingX = 200.0f;
	WindowSettings.PaddingY = 290.0f;
	WindowSettings.Title = "Inventory";
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

static void Camera(camera_t *Camera, vec2_t Target, float_t DeltaTime)
{
	Camera->Center = Mix(Camera->Center, ScrToIso(Target), DeltaTime);
}

static void HostPlayer(game_state_t *State)
{
	controller_t *Con = &State->Input.Controllers[0];
	interface_t *GUI = &State->GUI;

	map_object_t *Player = GetMapObject(&State->Map, State->Player);
	if (Player)
	{
		vec2_t CameraTarget = RectCenter(GetTileBounds(&State->Map, Player->OccupiedTile));
		Camera(&State->Camera, CameraTarget, State->Input.DeltaTime);

		if (!IsZero(Con->DPad))
		{
			Translate(&State->Map, Player, Player->OccupiedTile + Con->DPad);
		}
	}

	Inventory(State, GUI);
}