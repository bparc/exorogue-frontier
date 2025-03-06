
// Log

static void LogLn(log_t *Log, const char *Format, va_list List, int32_t Level)
{
	log_line_t *Line = &Log->Lines[Log->LineCount % Len(Log->Lines)];
	Line->TimeStamp = Log->ElapsedTime;
	Line->Level = (uint8_t)Level;
	Log->LineCount++;
	vsnprintf(Line->Text, Len(Line->Text), Format, List);
}

static void MessageLog(render_output_t *Out, log_t *Log, vec2_t At, float DeltaTime)
{
	const float_t MessageLifeTime = 4.0f;
	const float_t LineHeight = 25.0f;

	int32_t _Max = Min(Log->LineCount - 1, Len(Log->Lines) - 1);
	int32_t _Min = Max(0, Log->LineCount - Len(Log->Lines));

	vec2_t LineAt = At;

	for (int32_t Index = _Max; Index >= 0; Index--)
	{
		log_line_t *Line = &Log->Lines[(_Min + Index) % Len(Log->Lines)];

		float_t Time = (float_t)(Log->ElapsedTime - Line->TimeStamp);
		if (Time <= MessageLifeTime)
		{;
			float_t Lerp = Step(0.8f, 1.0f, Time / MessageLifeTime);

			vec4_t BackColor = (Line->Level == 1) ? ColorRed : ColorPink * 0.75f; //RGB(10, 10, 10);
			vec4_t Color = ColorWhite;
			Color = Color * (1.0f - Lerp);
			BackColor = BackColor * Color.w;

			float_t X = (At.x + (Lerp * 20.0f));
			DrawRect(Out, {X, LineAt.y}, V2(800.0f, 25.0f), BackColor);
			DrawString(Out, {X + 6, LineAt.y}, Line->Text, Color);
			LineAt.y -= LineHeight;

			//DebugPrint("%f", T0);

			continue;
		}

		break;
	}

	Log->ElapsedTime += DeltaTime;
}

// ...

static void BeginDebugFrame(client_t *Client, const input_t *Input)
{
	if (!Debug.Inited)
	{
		Debug.Inited = true;
		SetupMemory(&Debug.Memory, Debug.DebugMemory, Len(Debug.DebugMemory));
		Setup(&Debug.Cmds[0], &Debug.Memory, MB(4));
		Setup(&Debug.Cmds[1], &Debug.Memory, MB(4));
	}
	
	Debug.Device = GetGraphicsDevice(Client);
	Debug.Assets = GetAssets(Client);
	Debug.DebugOutput = RenderTo(&Debug.Cmds[0], Debug.Assets);

	Debug.Foreground = RenderTo(&Debug.Cmds[1], Debug.Assets);

	Debug.PrintAt = V2(15, 8);
	Debug.DeltaTime = Input->DeltaTime;
	Debug.Viewport = Input->Viewport;
}

static void EndDebugFrame(void)
{
	if (Debug.EnableDebugRenderer)
	{
		Dispatch(Debug.Device, &Debug.Cmds[0], Debug.DebugView, Debug.DebugCamera);
	}
	
	Debug.EnableDebugRenderer = false;

	#ifdef _DEBUG
	MessageLog(&Debug.Foreground, &Debug.Log, V2(20.0f, Debug.Viewport.y - 45.0f), Debug.DeltaTime);
	#endif

	Dispatch(Debug.Device, &Debug.Cmds[1], {}, {});

	Clear(Debug.Cmds, Len(Debug.Cmds));
}

static void BeginVisualDebugMode(vec4_t DebugView, transform_t DebugTrasform)
{
	Debug.DebugCamera = DebugTrasform;
	Debug.DebugView = DebugView;
	Debug.EnableDebugRenderer = true;
}

static void EndVisualDebugMode(void)
{

}

static void _DebugRect(vec2_t Offset, vec2_t Size, vec4_t Color)
{
	DrawRectOutline(&Debug.DebugOutput, Offset, Size, Color);
}

static void _DebugBounds(bounds_t Bounds, vec4_t Color)
{
	_DebugRect(Bounds.Min, Bounds.Max - Bounds.Min, Color);
}

static void _DebugLine(vec2_t A, vec2_t B, vec4_t Color)
{
	DrawLine(&Debug.DebugOutput, A, B, Color);
}

static void _DebugCircle(vec2_t Offset, float_t Radius, vec4_t Color)
{
	DrawCircleOutline(&Debug.DebugOutput, Offset, Radius, Color);
}

static void _DebugPoint(vec2_t Offset, vec4_t Color)
{
	DebugRect(Offset - V2(0.5f), V2(1.0f), Color);
}

static void _DebugString(vec2_t Offset, const char *String, vec4_t Color)
{
	Assert(0);
	//DrawString(&Debug.Foreground, Offset, String, Color);
}

static void DebugPrint(const char *Format, ...)
{
	char Message[256] = "";

	va_list List = {};
	va_start(List, Format);
	vsnprintf(Message, Len(Message), Format, List);
	va_end(List);

	DrawString(&Debug.Foreground, Debug.PrintAt, Message, ColorYellow);
	Debug.PrintAt.y += 25.0f;
}

static void _DebugLog(const char *Format, ...)
{
	va_list List = {};
	va_start(List, Format);
	LogLn(&Debug.Log, Format, List, 0);
	va_end(List);
}