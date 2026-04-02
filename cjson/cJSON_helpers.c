#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Function to read full file into one string */
char *cJSON_read_file(const char *filename) {
	/* Open the file */
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		return NULL;
	}
	/* Find the total file length */
	fseek(f, 0, SEEK_END);
	uint64_t length = ftell(f);
	/* Reset to the start of the file and malloc the required space */
	fseek(f, 0, SEEK_SET);
	char *data = (char *)malloc(length + 1);
	/* Now read the file into the string and add the termination character */
	fread(data, 1, length, f);
	data[length] = '\0';
	/* Close the file and return the string */
	fclose(f);
	return data;
}
