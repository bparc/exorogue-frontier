static camera_t CreateCamera(vec2_t Size)
{
	camera_t Result = {};
	Result.Size = Size;
	return Result;
}

static float_t GetCameraScaleFactor(const camera_t *Camera)
{
	float_t Result = 1.0f - Camera->Zoom;
	return Result;
}

static transform_t GetCameraTransform(const camera_t *Camera)
{
	transform_t Result = {};
	Result.Center = Camera->Center;
	Result.Size = Camera->Size * GetCameraScaleFactor(Camera);
	return Result;
}

static bounds_t GetCameraBounds(const camera_t *Camera)
{
	transform_t Transform = GetCameraTransform(Camera);
	vec2_t Extends = Transform.Size * 0.5f;
	bounds_t Result = Bounds(Transform.Center - Extends, Transform.Center + Extends);
	return Result;
}

static void TrackObject(camera_t *Camera, vec2_t Position, float_t Speed)
{
	Camera->Center = Mix(Camera->Center, Position, Speed);
}