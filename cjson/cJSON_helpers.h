#ifndef CJSON_HELPERS_H
#define CJSON_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Function to read full file into one string */
char *cJSON_read_file(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* CJSON_HELPERS_H */
