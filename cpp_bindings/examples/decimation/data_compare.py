import numpy as np
import matplotlib.pyplot as plt
import argparse

def main():
    parser = argparse.ArgumentParser(description='Verify Decimation Filter Output')
    parser.add_argument('--channels', type=int, required=True)
    parser.add_argument('--taps', type=int, required=True)
    parser.add_argument('--decimation', type=int, required=True)
    parser.add_argument('--input', type=str, required=True)
    parser.add_argument('--output', type=str, required=True)
    parser.add_argument('--check', type=str, nargs='+',
                        help='Other output files to check for an exact match against --output')
    args = parser.parse_args()

    # Load the binary file for the input data
    # Format: [TimeSteps, Channels, SamplesPerChunk]
    raw_in = np.fromfile(args.input, dtype=np.float32)
    in_matrix = raw_in.reshape(-1, args.channels, args.taps)

    # Load the binary file for the output data
    out_chunk_size = args.taps // args.decimation
    raw_out = np.fromfile(args.output, dtype=np.float32)
    out_matrix = raw_out.reshape(-1, args.channels, out_chunk_size)

    # Check that the other benchmark path outputs match exactly
    if args.check:
        all_match = True
        for filename in args.check:
            raw_check = np.fromfile(filename, dtype=np.float32)
            match = np.array_equal(raw_out, raw_check)
            all_match &= match
            print(f"{'PASS' if match else 'FAIL'}: {filename} matches {args.output}")
        raise SystemExit(0 if all_match else 1)

    # 3. Extract Channel 0 for visual check
    ch0_in = in_matrix[:, 0, :].flatten()
    ch0_out = out_matrix[:, 0, :].flatten()

    # 4. Plotting
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6))
    
    ax1.plot(ch0_in, label='Original (16kHz)')
    ax1.set_title(f"Channel 0 Input - {args.channels} Channels Total")
    ax1.legend()

    # Scale x-axis for output to match timing
    time_out = np.arange(len(ch0_out)) * args.decimation
    ax2.plot(time_out, ch0_out, color='red', label=f'Decimated (/{args.decimation})')
    ax2.set_title("Channel 0 Filtered Output")
    ax2.legend()

    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()
