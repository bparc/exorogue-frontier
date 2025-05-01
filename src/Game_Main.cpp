static void InitializeGame(game_state_t *State, const content_t *Content)
{
	memset(State, 0, sizeof(*State));
	State->Content = Content;

	int32_t MemoryBudget = MB(16);
	SetupMemory(&State->Memory, (uint8_t *)malloc(MemoryBudget), MemoryBudget);

	InitGUI(&State->GUI);

	//

	// Setup(&State->CommandBuffers[0], &State->Memory, MB(4));

	//

	State->Camera = CreateCamera({64.0f, 64.0f});
	State->Camera.Center = V2(0.0f, 0.0f);

	#define X 64
	#define Y 64
	CreateMap(&State->Map, X, Y, PushArray(&State->Memory, map_tile_t, X * Y), 8);
	#undef X
	#undef Y

	LoadAutoMapInput(&State->Map, "Assets/Map_001.tga");

	//CreateMapObject(&State->Map, {5, 0}, 0);

	CreateMapObject(&State->Map, {0, 6}, 0);
	CreateMapObject(&State->Map, {1, 6}, 0);
	CreateMapObject(&State->Map, {2, 6}, 0);
	CreateMapObject(&State->Map, {3, 6}, 0);
	CreateMapObject(&State->Map, {4, 6}, 0);
	CreateMapObject(&State->Map, {0, 6}, 0);

	CreateMapObject(&State->Map, {1, 4}, 0);
	CreateMapObject(&State->Map, {2, 4}, 0);
	
	//CreateMapObject(&State->Map, {, 4}, 0);

	map_object_t *Player = CreateMapObject(&State->Map, {16, 16}, 0);
	State->Player = Player->Self;

#if 0
	CreateMapObject(&State->Map, {1, 0}, 0);
	CreateMapObject(&State->Map, {2, 0}, 0);
	CreateMapObject(&State->Map, {3, 0}, 0);

	CreateMapObject(&State->Map, {0, 1}, 0);
	CreateMapObject(&State->Map, {1, 1}, 0);
	CreateMapObject(&State->Map, {2, 1}, 0);
	CreateMapObject(&State->Map, {3, 1}, 0);

	CreateMapObject(&State->Map, {0, 2}, 0);
	CreateMapObject(&State->Map, {1, 2}, 0);
	CreateMapObject(&State->Map, {2, 2}, 0);
	CreateMapObject(&State->Map, {3, 2}, 0);

	CreateMapObject(&State->Map, {0, 3}, 0);
	CreateMapObject(&State->Map, {1, 3}, 0);
	CreateMapObject(&State->Map, {2, 3}, 0);
	CreateMapObject(&State->Map, {3, 3}, 0);

	CreateMapObject(&State->Map, {8, 4}, 0);
#endif
}

static void Game(game_state_t *State)
{
	HostPlayer(State);
}

static void RunFrame(game_state_t *State, const input_t *Input, graphics_device_t *Device, const camera_t *DebugCamera)
{
	const camera_t *Camera;

	State->ElapsedTime += Input->DeltaTime;

	State->Input = *Input;
	Camera = (DebugCamera ? DebugCamera : &State->Camera);

	//

	renderer_data_t RenData = {};
	RenData.Content = State->Content;
	RenData.ClipPlane = GetCameraBounds(Camera);
	RenData.Transform = GetCameraTransform(Camera);	
	RenData.Viewport = MaintainAspectRatio(Input->Viewport, Ratio(RenData.Transform.Size));
	RenData.Map = &State->Map;

	//

	BeginVisualDebugMode(RenData.Viewport, RenData.Transform);
	BeginGUI(&State->GUI, Input, State->Content->Assets);

	// Logic

	Game(State);

	//

	EndGUI(&State->GUI);
	EndVisualDebugMode();

	// Render

	RenderScene(&State->Renderer, &RenData, Device);
	RenderGUI(&State->GUI, Device);
	//
}