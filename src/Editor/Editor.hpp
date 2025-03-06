
struct editor_t
{
	bool Inited;

	int32_t TimeSampleCount;
	float_t TimeSamples;
	float_t TimeAvarage;

	bool EditMode;

	camera_t Camera;
};

static void Editor(game_state_t *State, editor_t *Editor, const input_t *Input, interface_t *GUI);

// Camera.

static void EditorCamera(camera_t *Camera, bool JustActivated, const input_t *Input, camera_t GameCamera);

// Returns NULL if there aren't any active cameras.
static const camera_t *GetActiveEditorCamera(editor_t *Editor);

//