#ifndef FILTER_SHARED_H
#define FILTER_SHARED_H

#include <stdint.h>

/* Maximum number of new input samples the FIR and direct form IIR filters
 * process at once. Longer inputs are split into multiple blocks. If this is
 * overridden it must be defined the same for every file in the build, as it
 * sets the size of the filter history buffers */
#ifndef FILTER_BLOCK_SIZE
#define FILTER_BLOCK_SIZE 128
#endif

/* Number of elements needed for each FIR/IIR history buffer: the previous
 * taps - 1 input (or output) samples followed by a block of new samples */
#define FILTER_STATE_SIZE(taps) ((uint32_t)(taps) - 1u + FILTER_BLOCK_SIZE)

#endif /* FILTER_SHARED_H */
