static void BindPlayer(player_t *Player, object_t Object)
{
	memset(Player, 0, sizeof(*Player));
	Player->Self = Object;
}

static user_cmd_t SendPlayerCommands(map_t *Map, player_t *Player, const input_t *Input)
{
	const controller_t *Controller;
	const map_object_t *Entity;

	user_cmd_t Result = {};
	
	Controller = &Input->Controllers[0];

	Entity = GetMapObject(Map, Player->Self);
	if (IsZero(Controller->DPad) == false)
	{
		memset(&Result, 0, sizeof(Result));
		Result.Type = user_cmd_Move;
		Result.User = Player->Self;
		Result.Dest = (Entity->OccupiedTile + Controller->DPad);
	}

	return Result;
}