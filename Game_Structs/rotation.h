#pragma once
struct Rotation {
	float yawa, pitcha;
	//float rolla, rollb;
	//float pitchb, yawb;
};

static void Rotate(Rotation* r, float yaw, float pitch) {
	pitch = fmod(++pitch, 2.f);
	yaw = fmod(++yaw, 2.f);
	yaw--;
	pitch--;

	Rotation a;
	a.yawa = yaw;
	a.pitcha = pitch;
	//a.pitchb = -pitch;
	//a.yawb = yaw;
	*r = a;
}