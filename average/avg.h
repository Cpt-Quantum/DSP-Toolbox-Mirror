#ifndef AVG_H
#define AVG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

typedef struct
{
	float *data_buff;
	uint32_t data_buff_length;
	float current_total;
	uint32_t head;
	uint32_t tail;
	bool first_cycle;
} moving_avg_t;

float average(float *data_in, uint32_t data_length);
float power_average(float *data_in, uint32_t data_length);
float moving_average(moving_avg_t *moving_avg_buffer, float new_samp);

#ifdef __cplusplus
}
#endif

#endif /* AVG_G */
