
//

static camera_t GetActiveCamera(game_state_t *State)
{
	return State->Camera;
}

static object_t Spawn(game_state_t *State, point_t At, object_type_t Type)
{
	map_object_t *Object = CreateMapObject(&State->Map, At, &State->Content->ObjectTypes[Type]);
	return Object ? Object->Self : 0;
}

static active_unit_t *FindActiveUnit(game_state_t *State, object_t Object)
{
	active_unit_t *Result = 0;
	for (int32_t Index = 0; Index < State->Units.Count; Index++)
	{
		active_unit_t *Match = &State->Units[Index];
		if (Match->Self == Object)
		{
			Result = Match;
			break;
		}
	}
	return Result;
}

static void Join(game_state_t *State, object_t Object, bool Bot)
{
	active_unit_t *Unit = FindActiveUnit(State, Object);
	if (Unit == NULL)
	{
		Unit = State->Units.Push();
		if (Unit)
		{
			memset(Unit, 0, sizeof(*Unit));	
			Unit->Self = Object;
			Unit->Bot = Bot;
		}
	}
}

// ...

static void QueryAction(game_state_t *State, object_t User, point_t Dest, bool Fence)
{
	action_t *Action = 0;
	map_object_t *Entity = GetMapObject(&State->Map, User);

	if (Entity && IsPassable(&State->Map, Dest))
	{
		Action = State->ActionQueue.Push();
	}

	if (Action)
	{
		memset(Action, 0, sizeof(*Action));
		Action->User = User;
		Action->Dest = Dest;
		Action->Fence = Fence;
		Action->Source = Entity->OccupiedTile;
	}
}

static void ScheduleAI(game_state_t *State)
{
	for (int32_t Index = 0; Index < State->Units.Count; Index++)
	{
		active_unit_t *Unit = &State->Units[Index];
		map_object_t *Entity = GetMapObject(&State->Map, Unit->Self);
	
		if (Entity && Unit->Bot)
		{
			point_t RandomDir = {};
			RandomDir.x = rand() % 2;
			RandomDir.y = rand() % 2;

			QueryAction(State, Entity->Self, Entity->OccupiedTile + RandomDir);
		}
	}
}

static void AcceptUserInput(game_state_t *State, user_cmd_t Cmd)
{
	if (State->AwaitUserCmds && (State->ActionQueue.Count == 0))
	{
		State->ActionQueue.Clear();

		QueryAction(State, Cmd.User, Cmd.Dest, false);

		ScheduleAI(State);

		State->AwaitUserCmds = false;
	}
}

static void PlayActions(game_state_t *State, float_t DeltaTime)
{
	for (int32_t Index = 0; Index < State->ActionQueue.Count; Index++)
	{
		action_t *Action = &State->ActionQueue[Index];
		Action->Time += DeltaTime * 2.5f;
		
		map_object_t *Object = GetMapObject(&State->Map, Action->User);
		if (Object)
		{
			float_t MoveDuration = 0.8f;

			vec2_t Source = GetTileCenter(&State->Map, Action->Source);
			vec2_t Target = GetTileCenter(&State->Map, Action->Dest);

			float_t Lerp = Min(Action->Time / MoveDuration, 1.0f);
			
			float_t Height = Sin(Lerp * 0.5f) * 2.0f;
			Animate(Object, Mix(Source, Target, Lerp), Height);
		}

		if (Action->Time >= 1.0f)
		{
			Translate(&State->Map, Action->User, Action->Dest);
			State->ActionQueue.Remove(Index--);
		}

		if (Action->Fence)
		{
			break;
		}
	}
}

// ...

#include "Game_Main.cpp"