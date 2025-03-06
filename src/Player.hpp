struct player_t
{
	object_t Self;
};

enum user_cmd_type_t
{
	user_cmd_None = 0,
	user_cmd_Move,
};

struct user_cmd_t
{
	user_cmd_type_t Type;
	object_t User;
	point_t Dest;
};

static void BindPlayer(player_t *Player, object_t Object);
static user_cmd_t SendPlayerCommands(map_t *Map, player_t *Player, const input_t *Input);