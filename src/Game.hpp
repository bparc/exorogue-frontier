
struct action_t
{
	object_t User;
	point_t Source;
	point_t Dest;
	float_t Time;

	bool Fence;
};

struct active_unit_t
{
	object_t Self;
	bool Bot;
};

struct game_state_t
{
	const content_t *Content;
	float_t ElapsedTime;
	memory_t Memory;

	player_t Player;
	camera_t Camera;
	map_t Map;

	//

	bool AwaitUserCmds;
	array_t <action_t, 256>ActionQueue;
	array_t <active_unit_t, 32>Units;
	//

	command_buffer_t CommandBuffers[1];
};

static object_t Spawn(game_state_t *State, point_t At, object_type_t Type);
static active_unit_t *FindActiveUnit(game_state_t *State, object_t Object);
static void Join(game_state_t *State, object_t Object, bool Bot);

// ...


static void InitializeGame(game_state_t *State, const content_t *Content);
static void RunFrame(game_state_t *State, const input_t *Input, graphics_device_t *Device, const camera_t *DebugCamera);

static camera_t GetActiveCamera(game_state_t *State);

//

static void AcceptUserInput(game_state_t *State, user_cmd_t Cmd);
static void PlayActions(game_state_t *State, float_t DeltaTime);

static void ScheduleAI(game_state_t *State);
static void QueryAction(game_state_t *State, object_t User, point_t Dest, bool Fence = false);