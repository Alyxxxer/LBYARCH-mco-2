#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

extern void imgCvtGrayFloatToInt(int height, int width,
    float* fImg, unsigned char* iImg);

static void refCvtGrayFloatToInt(int height, int width,
    const float* fImg, unsigned char* iImg)
{
    long long n = (long long)height * (long long)width;
    long long k;
    for (k = 0; k < n; k++)
        iImg[k] = (unsigned char)nearbyintf(fImg[k] * 255.0f);
}

static void printFloatImage(int height, int width, const float* img)
{
    int r, c;
    for (r = 0; r < height; r++) {
        for (c = 0; c < width; c++)
            printf("%.2f%s", img[(long long)r * width + c],
                (c == width - 1) ? "" : ", ");
        printf("\n");
    }
}

static void printIntImage(int height, int width, const unsigned char* img)
{
    int r, c;
    for (r = 0; r < height; r++) {
        for (c = 0; c < width; c++)
            printf("%3u%s", img[(long long)r * width + c],
                (c == width - 1) ? "" : ", ");
        printf("\n");
    }
}

static long long checkCorrectness(int height, int width, const float* fImg,
    const unsigned char* asmOut, int verbose)
{
    long long n = (long long)height * (long long)width;
    long long k, bad = 0;
    unsigned char* ref = (unsigned char*)malloc((size_t)n);
    if (!ref) { printf("  [check] out of memory\n"); return -1; }

    refCvtGrayFloatToInt(height, width, fImg, ref);

    for (k = 0; k < n; k++) {
        if (ref[k] != asmOut[k]) {
            bad++;
            if (verbose && bad <= 10)
                printf("  mismatch at index %lld: asm=%u ref=%u (f=%.9f)\n",
                    k, asmOut[k], ref[k], fImg[k]);
        }
    }
    free(ref);

    printf("  CORRECTNESS: %lld / %lld pixels match --> %s\n",
        n - bad, n, (bad == 0) ? "PASSED" : "FAILED");
    return bad;
}

static void runManual(void)
{
    int height = 0, width = 0;
    long long n, k;
    float* in;
    unsigned char* out;

    printf("\nEnter height and width: ");
    if (scanf("%d %d", &height, &width) != 2 || height <= 0 || width <= 0) {
        printf("Invalid dimensions.\n");
        return;
    }

    n = (long long)height * (long long)width;
    in = (float*)malloc((size_t)n * sizeof(float));
    out = (unsigned char*)malloc((size_t)n);
    if (!in || !out) { printf("Out of memory.\n"); free(in); free(out); return; }

    printf("Enter %lld float pixel values (0.0 - 1.0):\n", n);
    for (k = 0; k < n; k++) {
        if (scanf("%f", &in[k]) != 1) {
            printf("Invalid pixel value at index %lld.\n", k);
            free(in); free(out);
            return;
        }
    }

    imgCvtGrayFloatToInt(height, width, in, out);

    printf("\nOutput:\n");
    printIntImage(height, width, out);
    printf("\n");
    checkCorrectness(height, width, in, out, 1);

    free(in);
    free(out);
}

static void benchmarkOne(int height, int width, int runs)
{
    long long n = (long long)height * (long long)width;
    long long k;
    int r;
    float* in;
    unsigned char* out;
    LARGE_INTEGER freq, t0, t1;
    double total = 0.0, best = 1e300, worst = 0.0, ms;

    in = (float*)malloc((size_t)n * sizeof(float));
    out = (unsigned char*)malloc((size_t)n);
    if (!in || !out) {
        printf("Out of memory for %dx%d.\n", height, width);
        free(in); free(out); return;
    }

    for (k = 0; k < n; k++)
        in[k] = (float)rand() / (float)RAND_MAX;  

    QueryPerformanceFrequency(&freq);

    imgCvtGrayFloatToInt(height, width, in, out);  

    for (r = 0; r < runs; r++) {
        QueryPerformanceCounter(&t0);
        imgCvtGrayFloatToInt(height, width, in, out);
        QueryPerformanceCounter(&t1);

        ms = (double)(t1.QuadPart - t0.QuadPart) * 1000.0 / (double)freq.QuadPart;
        total += ms;
        if (ms < best)  best = ms;
        if (ms > worst) worst = ms;
    }

    printf("\n%d x %d  (%lld pixels, %d runs)\n", height, width, n, runs);
    printf("  average : %10.6f ms\n", total / runs);
    printf("  fastest : %10.6f ms\n", best);
    printf("  slowest : %10.6f ms\n", worst);
    printf("  per px  : %10.6f ns\n", (total / runs) * 1.0e6 / (double)n);
    checkCorrectness(height, width, in, out, 0);

    free(in);
    free(out);
}

static void runBenchmark(void)
{
    const int RUNS = 30;
    printf("\n=== PERFORMANCE TEST (x86-64 assembly kernel only) ===\n");
    benchmarkOne(10, 10, RUNS);
    benchmarkOne(100, 100, RUNS);
    benchmarkOne(1000, 1000, RUNS);
    printf("\n");
}

int main(void)
{
    int choice = 0;

    srand(12345);   

    for (;;) {
        printf("=========================================\n");
        printf(" LBYARCH MP2 - imgCvtGrayFloatToInt\n");
        printf("=========================================\n");
        printf(" [1] Enter an image manually\n");
        printf(" [2] Performance test (10x10, 100x100, 1000x1000)\n");
        printf(" [0] Exit\n");
        printf("Choice: ");

        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
        case 1: runManual();    break;
        case 2: runBenchmark(); break;
        case 0: return 0;
        default: printf("\nInvalid choice.\n\n"); break;
        }
    }
    return 0;
}