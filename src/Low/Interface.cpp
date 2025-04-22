
static void InitGUI(interface_t *State)
{
	memset(State, 0, sizeof(*State));
}

static void UpdateRoot(interface_t *State, const input_t *Input)
{
	for (int32_t Index = 0; Index < State->SortBuffer.Count; Index++)
	{
		const window_t *Window = &State->Windows[State->SortBuffer[Index]];

		if (Contains(Window->ClientBounds, State->MouseCursor))
		{
			State->RootWnd = Window->Name;

			if (Input->MouseButtons[0].Down && (Index > 0)) // Focus
			{
				memmove(
					&State->SortBuffer.Values[1],
					&State->SortBuffer.Values[0],
					(Index) * sizeof(*State->SortBuffer.Values));

				State->SortBuffer[0] = (uint16_t)Window->Name;
			}

			break;	
		}
	}
}

static void BeginGUI(interface_t *State, const input_t *Input, assets_t *Assets)
{
	Assert(!State->RenderOutputInvalid);
	State->RenderOutputInvalid = true;

	Setup(&State->CommandBuffer, State->Vertices, Len(State->Vertices), State->Commands, Len(State->Commands));
	State->Out = RenderTo(&State->CommandBuffer, Assets);

	State->WindowCommands.Clear();

	State->MouseCursor = Input->MouseCursor;
	State->Interact[0] = Input->MouseButtons[0].State;
	State->Interact[1] = Input->MouseButtons[1].State;

	State->Hovered = 0;
	State->RootWnd = UINT32_MAX;

	UpdateRoot(State, Input);

	State->InterceptInputs = (State->RootWnd != UINT32_MAX) || (State->Active);
}

static void EndGUI(interface_t *State)
{
	Assert(State->RenderOutputInvalid);
	State->RenderOutputInvalid = false;
}

static void _RenderDrawList(graphics_device_t *Device, command_buffer_t *CommandBuffer, interface_draw_command_t *Head)
{
	interface_draw_command_t *Cmd = Head;
	while (Cmd)
	{
		Dispatch(Device, CommandBuffer, Cmd->First, Cmd->Last, {}, {});
		Cmd = Cmd->Next;
	}
}

static void RenderGUI(interface_t *State, graphics_device_t *Device)
{
	Assert(!State->RenderOutputInvalid);

	//Dispatch(Device, &State->CommandBuffer, {}, {});
	for (int32_t Index = (State->SortBuffer.Count - 1); Index >= 0; Index--)
	//for (int32_t Index = 0; Index < 1; /*State->SortBuffer.Count;*/ Index++)
	{
		window_t *Window = &State->Windows[State->SortBuffer[Index]];

		_RenderDrawList(Device, &State->CommandBuffer, Window->DrawListHead);

		Window->DrawListHead = NULL;
		Window->DrawListBack = NULL;
	}
}

// Sets up the rendering state so that all draw commands drawn to interface_t::Out
// are rendered to the specified window.

static inline void CreateRenderTarget(interface_t *State, window_t *Window)
{
	interface_draw_command_t *Cmd;
	render_command_t *NewCommand;

	Assert(!State->Wnd);

	// Allocate a new draw command for the specified window.

	Cmd = State->WindowCommands.Push();
	memset(Cmd, 0, sizeof(*Cmd));

	NewCommand = AppendCommand(&State->CommandBuffer, Primitive_Quad, GetTextureCacheHandle(State->Out.Assets));
	Assert(NewCommand);

	Cmd->First = ((uint16_t)State->CommandBuffer.CmdCount - 1);

	// Append the new command to the window's linked list.

	if (!Window->DrawListHead)
	{
		Window->DrawListBack = Window->DrawListHead = Cmd;
	}
	else
	{
		Window->DrawListBack->Next = Cmd;
		Window->DrawListBack = Cmd;
	}

	BindWindow(State, Window);
}

static void DeleteRenderTarget(interface_t *State)
{
	Assert(State->Wnd);
	State->Wnd->DrawListBack->Last = ((uint16_t)State->CommandBuffer.CmdCount - 1);

	BindWindow(State, 0);
}

static void UpdateWindowFrame(interface_t *State, window_t *Window)
{
	const char *Title = Window->Title;
	rect_t ClientArena = Window->ClientBounds;
	rect_t Bar = Rect(ClientArena.Offset, {ClientArena.Width, 20.0f});\
	vec4_t BarColor = ColorRed;

	// Dragging

	hash_t BarID = HashName(State, Title);
	Interact(State, Bar, BarID);
	if (IsActive(State, BarID))
	{
		Window->ClientBounds.Offset = (State->MouseCursor + State->ClickOffset);
		BarColor = ColorGreen;
	}

	//
		
	Window->LayoutCursor = V2(Bar.Offset.x, Bar.y + Bar.Height);

	DrawRect(&State->Out, Stretch(ClientArena, 1.0f), ColorBlack); // Outline
	DrawRect(&State->Out, ClientArena, ColorWhite);

	DrawRect(&State->Out, Bar, BarColor);
	DrawString(&State->Out, Bar.Offset + V2(4.0f, 0.0f), Title);
}

static void CreateModalWindow(interface_t *State, hash_t WindowIndex, rect_t ClientArena, const char *Title)
{
	window_t *Window = &State->Windows[WindowIndex];

	Assert(!Window->Inited);
	Window->Inited = true;
	Window->Title = Title;

	Window->ClientBounds = ClientArena;
	Window->Name = WindowIndex;

	State->SortBuffer.Push((uint16_t)WindowIndex);
}

static bool BeginWindow(interface_t *State, hash_t WindowIndex, vec2_t Offset, vec2_t Size, const char *Title)
{
	window_t *Window;

	Assert(WindowIndex < Len(State->Windows));
	Window = &State->Windows[WindowIndex];

	if (!Window->Inited)
	{
		CreateModalWindow(State, WindowIndex, Rect(Offset, Size), Title);	
	}

	CreateRenderTarget(State, Window);

	// Everything is set up - from now on we can draw widgets to the window.

	if (Window->DrawListBack == Window->DrawListHead) // Make sure we're calling UpdateWindowFrame() only once per frame.
	{
		UpdateWindowFrame(State, Window);
	}

	return (true);
}

static void EndWindow(interface_t *State)
{
	DeleteRenderTarget(State);
}

static bool IsHovered(const interface_t *State, rect_t Bb)
{
	bool Result = Contains(Bb, State->MouseCursor) && (State->Wnd->Name == State->RootWnd);
	return Result;
}

static bool IsHovered(const interface_t *State, hash_t ID)
{
	bool Result = (State->Hovered == ID);
	return Result;
}

static bool IsActive(const interface_t *State, hash_t ID)
{
	bool Result = (State->Active == ID);
	return Result;
}

static void BindWindow(interface_t *State, window_t *Window)
{
	if (Window)
	{
		Assert(!State->Wnd);
		State->Wnd = Window;	
	}
	else
	{
		Assert(State->Wnd);
		State->Wnd = Window;
	}
	
}

static hash_t HashName(const interface_t *State, const char *Name)
{
	hash_t Result = 0;

	MurmurHash3_x86_32(Name, (uint32_t)strlen(Name), State->Wnd->Name, &Result);

	return Result;
}

static bool Interact(interface_t *State, rect_t Bb, hash_t ID)
{
	bool Result = false;

	if ((State->Active == 0) && IsHovered(State, Bb))
	{
		State->Hovered = ID;

		if (State->Interact[0])
		{
			State->Active = ID;
			State->ClickOffset = Bb.Offset - State->MouseCursor;
		}
	}
	
	if (State->Active == ID)
	{
		if (!State->Interact[0])
		{
			if (IsHovered(State, Bb))
			{
				Result = true;
			}

			State->Active = 0;
		}
	}

	return Result;
}

static void Text(interface_t *State, char* Text, point_t Pos, vec4_t Color, float_t Scale)
{
	vec2_t FinalPos = {
		Pos.x + State->Wnd->ClientBounds.x,
		Pos.y + State->Wnd->ClientBounds.y
	};

	const render_output_t *Out = &State->Out;
	DrawString(&State->Out, FinalPos, Text, State->Wnd->ClientBounds, Color, Scale);
}

static bool Button(interface_t *State, const char *Name)
{
	hash_t ID = HashName(State, Name);
	rect_t Bb = Rect(State->Wnd->LayoutCursor, V2(100.0f, 20.0f));

	bool Result = Interact(State, Bb, ID);

	vec4_t Color = ColorBlue;

	if (IsHovered(State, ID))
	{
		Color.x *= 0.9f;
		Color.y *= 0.9f;
		Color.z *= 0.9f;
	}

	if (State->Active == ID)
	{
		Color = ColorOrange;
	}

	DrawRect(&State->Out, Bb, Color);
	DrawString(&State->Out, Bb.Offset + V2(5.0f, 0.0f), Name);

	State->Wnd->LayoutCursor.y += 20.0f;

	return Result;
}