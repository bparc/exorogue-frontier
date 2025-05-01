
//
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

//

static camera_t GetActiveCamera(game_state_t *State)
{
	return State->Camera;
}

// ...

// ...

#include "Game_Main.cpp"