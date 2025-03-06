struct camera_t
{
	vec2_t Center;
	vec2_t Size;
	float_t Zoom;
};

static camera_t CreateCamera(vec2_t Size);
static float_t GetCameraScaleFactor(const camera_t *Camera);
static transform_t GetCameraTransform(const camera_t *Camera);
static bounds_t GetCameraBounds(const camera_t *Camera);

static void TrackObject(camera_t *Camera, vec2_t Position, float_t Speed);