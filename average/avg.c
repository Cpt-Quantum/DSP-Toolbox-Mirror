#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include "avg.h"

float average(float *data_in, uint32_t data_length)
{
	float avg = 0;
	for (uint32_t i = 0; i < data_length; i++)
	{
		avg += data_in[i];
	}
	avg = avg / data_length;
	return avg;
}

float power_average(float *data_in, uint32_t data_length)
{
	float power = 0;
	for (uint32_t i = 0; i < data_length; i++)
	{
		power += data_in[i] * data_in[i];
	}
	return (power / data_length);
}

float moving_average(moving_avg_t *moving_avg_buffer, float new_samp)
{
	if (moving_avg_buffer->first_cycle == true)
	{
		/* Add new sample to the sample buffer */

		/* Compute the moving average and add it to the struct */

		/* Check if the number of averages performed have reached the total */
		if (moving_avg_buffer->avg_counter >= moving_avg_buffer->data_buff_length)
		{
			moving_avg_buffer->first_cycle == false;
		}
	}
	else
	{
		/* Remove the oldest sample from the average total */
		moving_avg_buffer->current_avg -= moving_avg_buffer->data_buff[moving_avg_buffer->tail];
		moving_avg_buffer->tail = (moving_avg_buffer->tail + 1) % moving_avg_buffer->data_buff_length;

		/* Add the new sample to the current average */
		moving_avg_buffer->current_avg += new_samp / moving_avg_buffer->data_buff_length;

		/* Add the new average to the buffer */
		moving_avg_buffer->data_buff[moving_avg_buffer->head] += new_samp;
		moving_avg_buffer->head++;
	}

	/* Return the average */
	return moving_avg_buffer->current_avg;
}