#include "main_inc.h"
#include "../cjson/cJSON.h"
#include "../cjson/cJSON_helpers.h"
#include "json_to_filt_float.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Define the global variables */
float f_s;
struct sos_filter_float_t sos_filt;
struct iir_float_t iir_filt;
struct fir_float_t fir_filt;
struct fir_float_t fir_decimation_filt;

void filter_init(const char *filename) {
	/* Parse the file into the cJSON object */
	char *json_string = cJSON_read_file(filename);
	if (json_string == NULL) {
		printf("Error: Failed to read file.\n");
		exit(EXIT_FAILURE);
	}
	cJSON *root = cJSON_Parse(json_string);
	free(json_string);
	if (root == NULL) {
		printf("Error: Failed to parse JSON from file.\n");
		exit(EXIT_FAILURE);
	}
	/* Parse the nodes in the JSON file */
	cJSON *f_s_node = cJSON_GetObjectItem(root, "f_s");
	cJSON *fir_node = cJSON_GetObjectItem(root, "fir");
	cJSON *iir_node = cJSON_GetObjectItem(root, "iir_base");
	cJSON *sos_node = cJSON_GetObjectItem(root, "iir_sos");
	/* Retrieve the sample rate */
	f_s = get_sample_rate_from_json_float(f_s_node);
	/* Create the filters from each node */
	init_iir_from_json_float(&iir_filt, iir_node);
	init_sos_from_json_float(&sos_filt, sos_node);
	init_fir_from_json_float(&fir_filt, fir_node);
	init_fir_from_json_float(&fir_decimation_filt, fir_node);
	/* Cleanup the cJSON objects */
	cJSON_Delete(root);
}

void filter_deinit() {
	free_iir_float(&iir_filt);
	free_sos_float(&sos_filt);
	free_fir_float(&fir_filt);
	free_fir_float(&fir_decimation_filt);
}
