#pragma once
#define TABLE_SIZE 1024
#define SAMPLE_RATE 48000

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float clamp_SR(float freq); // SR = sample rate
float clamp_unit(float value);
float scale_unit(float value, float min, float max);