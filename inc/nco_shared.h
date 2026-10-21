#ifndef NCO_SHARED_H
#define NCO_SHARED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Status returned by nco_init_*() */
typedef enum {
	NCO_OK,
	NCO_ERR_NULL_POINTER,
	/* The sample rate is 0 */
	NCO_ERR_INVALID_SAMPLE_RATE,
	/* FUNCTION_IMPULSE and FUNCTION_STEP aren't periodic, so can't be
	 * generated from a phase */
	NCO_ERR_INVALID_FUNCTION,
} NCO_STATUS_E;

#ifdef __cplusplus
}
#endif

#endif /* NCO_SHARED_H */
