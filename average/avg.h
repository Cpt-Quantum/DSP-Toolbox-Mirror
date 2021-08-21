#ifndef AVG_H
#define AVG_H

#include <stdint.h>
#include <math.h>

typedef struct
{
	float *data_buff;
	uint32_t data_buff_length;
	float current_avg;
	uint32_t head;
	uint32_t tail;
	bool first_cycle;
	uint32_t avg_counter;
} moving_avg_t;

float average(float *data_in, uint32_t data_length);
float power_average(float *data_in, uint32_t data_length);

#endif /* AVG_G */