## x86-to-C Interface Programming Project

Submitted by:

PLURAD, Venice Raeka [S25B] RAMIREZ, Diana Angela [S25B]

# Average execution time and short analysis of the performance

QueryPerformanceCounter was used to time the assembly kernel only (imgCvtGrayFloatToInt), averaging over 30 runs per size and excluding I/O and memory allocation:

| Image Size | Pixels | Avg Time |   | Fastest (ms) Slowest (ms) Time / Pixel |   |
| --- | --- | --- | --- | --- | --- |
|   |   | (ms) |   |   | (ns) |
| 10 x 10 | 100 | 0.000090 | 0.000000 | 0.000200 | 0.900000 |
| 100 x 100 | 10000 | 0.004873 | 0.003500 | 0.044100 | 0.487 |
| 1000 x 1000 1000000 |   | 0.426397 | 0.350800 | 2.150000 | 0.426 |

*Table 1. Execution time of the assembly kernel across input sizes (n = 30 trials per size)*

As the size of the input increased, mean execution time monotonically increased from 0.000090ms at 100 pixels to 0.426397ms at 1,000,000 pixels, as would be expected from an algorithm that does not vary the amount of work done per pixel.

However, this was not the case with the normalized time per pixel metric. It was about 0.90 ns at the smallest size (10x10) and reduced to a fairly stable range of 0.43 to 0.49 ns at the largest two sizes (100x100 and 1000x1000). A possible reason for this is that the overhead of making each call including the overhead of the timing mechanism is a higher percentage of the total time when there are only a few calls in the total workload, therefore falsely raising the per-pixel cost when n=100. This explanation is consistent with the observed pattern but it has not been directly isolated in the present experimental design (e.g. by using a dedicated near zero workload control), and is thus presented as a probable, not a confirmed interpretation of the data.

The timing results were checked independently for correctness. The results for the three image sizes tested are compared element-wise with a reference implementation in C, with the same conversion formula. In all three trials, there was 100% match for all pixels, which means that any observed differences in execution times are only characteristic of the performance of the trials, and not from computational errors in the implementation of the assemblies.


#Screenshot of the program output with the correctness check
<img width="483" height="428" alt="Screenshot 2026-08-07 160736" src="https://github.com/user-attachments/assets/f9191267-3bcd-4c03-a799-3e045dd7bf11" />
*Figure 1. Manual Image Entry and Correctness Verification*

<img width="556" height="589" alt="Screenshot 2026-08-07 160752" src="https://github.com/user-attachments/assets/3dc97b69-a5a4-4afa-91ea-74729b6f85e7" />
*Figure 2. Performance Test Results — 10×10, 100×100, and 1000×1000 (30 Runs Each)*
