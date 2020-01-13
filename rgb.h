#pragma once
#include "pch.h"
#include "math.h"

// niv: 205 205 205 128 205 205 205 17
// zam: 006EBB

// copy pasted from : https://gist.github.com/kuathadianto/200148f53616cbd226d993b400214a7f

typedef struct {
	unsigned char r;      
	unsigned char g;      
	unsigned char b;
	unsigned char a;
} rgba;

typedef struct {
	int h;
	float s;
	float v;
} hsv;

static void hsvrgb(hsv* hsv, rgba* rgb) {
	float C = hsv->s * hsv->v;
	float X = C * (1 - fabs(fmod(hsv->h / 60.0, 2) - 1));
	float m = hsv->v - C;
	float Rs, Gs, Bs;
	if (hsv->v >= 0 && hsv->h < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (hsv->h >= 60 && hsv->h < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (hsv->h >= 120 && hsv->h < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (hsv->h >= 180 && hsv->h < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (hsv->h >= 240 && hsv->h < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}
	rgb->r = (unsigned char)((Rs + m) * 255);
	rgb->g = (unsigned char)((Gs + m) * 255);
	rgb->b = (unsigned char)((Bs + m) * 255);
}