#ifndef FFT_SHARED_H
#define FFT_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Status returned by the fft_plan_init_*() functions, in the order the errors
 * are checked */
typedef enum {
	FFT_OK,
	FFT_ERR_NULL_POINTER,
	FFT_ERR_INVALID_SIZE,
	FFT_ERR_WORK_TOO_SMALL,
	FFT_ERR_WORK_MISALIGNED,
} FFT_STATUS_E;

#ifdef __cplusplus
}
#endif

#endif /* FFT_SHARED_H */
