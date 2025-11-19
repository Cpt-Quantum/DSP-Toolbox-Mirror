#ifndef WAVE_EXPORT_H
#define WAVE_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

float * create_buffer(uint32_t length);
void csv_export(char *filename, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif //WAVE_EXPORT_H
