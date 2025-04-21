static void InitializeGame(game_state_t *State, const content_t *Content)
{
	memset(State, 0, sizeof(*State));
	State->Content = Content;

	int32_t MemoryBudget = MB(16);
	SetupMemory(&State->Memory, (uint8_t *)malloc(MemoryBudget), MemoryBudget);

	//

	Setup(&State->CommandBuffers[0], &State->Memory, MB(4));

	//

	State->Camera = CreateCamera({128.0f, 128.0f});
	State->Camera.Center = V2(80.0f, 64.0f);

	#define X 64
	#define Y 64
	CreateMap(&State->Map, X, Y, PushArray(&State->Memory, map_tile_t, X * Y), 8);
	#undef X
	#undef Y

	LoadAutoMapInput(&State->Map, "Assets/Map_001.tga");

	object_t Player;

	Spawn(State, {7, 4}, ObjectType_Slime);
	Spawn(State, {7, 8}, ObjectType_Slime);
	Spawn(State, {8, 11}, ObjectType_Slime);
	Spawn(State, {12, 11}, ObjectType_Bonfire);

	Player = Spawn(State, {4, 6}, ObjectType_Player);
	
	Assert(Player);
	BindPlayer(&State->Player, Player);

	for (int32_t Index = 0; Index < State->Map.Objects.Count; Index++)
	{
		map_object_t *Obj = &State->Map.Objects[Index];

		bool Bot = false;
		if (Obj->Type->Tag == Tag_Hostile)
		{
			Bot = true;
		}

		Join(State, Obj->Self, Bot);
	}

	State->AwaitUserCmds = true;
}

static void RunFrame(game_state_t *State, const input_t *Input, graphics_device_t *Device, const camera_t *DebugCamera)
{
	const camera_t *Camera;
	transform_t Transform;
	rect_t View;

	State->ElapsedTime += Input->DeltaTime;
		
	//

	Camera = &State->Camera;
	if (DebugCamera)
	{
		Camera = DebugCamera;
	}

	TrackObject(&State->Camera, GetObjectCenter(GetMapObject(&State->Map, State->Player.Self)), Input->DeltaTime * 10.0f);

	Transform = GetCameraTransform(Camera);
	View = MaintainAspectRatio(Rect({}, Input->Viewport), Transform.Size.x / Transform.Size.y);

	BeginVisualDebugMode(V4(View), Transform);

	// ...

	user_cmd_t UserCmd = SendPlayerCommands(&State->Map, &State->Player, Input);
	if ((UserCmd.Type > 0) && State->AwaitUserCmds)
	{
		AcceptUserInput(State, UserCmd);
	}

	if (!State->AwaitUserCmds)
	{
		PlayActions(State, Input->DeltaTime);

		if (State->ActionQueue.Count == 0)
		{
			State->AwaitUserCmds = true;
		}
	}

	// ...

	Clear(State->CommandBuffers, Len(State->CommandBuffers));

	{
	render_output_t Out = RenderTo(&State->CommandBuffers[0], State->Content->Assets);
	RenderView(&State->Map, GetCameraBounds(Camera), &Out, State->Content, State->ElapsedTime);

	Dispatch(Device, &State->CommandBuffers[0], V4(View), Transform, Dispatch_Filter_NearestNeighbour);
	}
	
	EndVisualDebugMode();
}