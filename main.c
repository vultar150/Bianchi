#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define INVALID_ARGS -1
#define EPS 1e-6

const char * output_file = "../output.txt";
const char * file_mode = "a";

// const uint64_t cwmin = 15; // 2^4-1
// const uint64_t cwmax = 1023; // 2^10-1
// const uint64_t retry = 7; // number of tries allowed for transmission
const uint64_t cwmin = 16; // 2^4-1
const uint64_t cwmax = 1024; // 2^10-1
const uint64_t retry = 6; // number of tries allowed for transmission
const uint64_t Ts = 2870; // success duration = TData + SIFS + TAck
const uint64_t Tc = Ts; // no RTS/CTS, collision duration = success duration
const uint64_t Te = 9; // duration of empty slot 9 us
const uint64_t L = 2000; // packet size 2000 bytes

double tau(double p);

/**
 * Returns number of stations or -1 if args invalid
 */
int64_t parse_args(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }
    int64_t res = strtoll(argv[1], NULL, 10);
    return  res ? res : INVALID_ARGS;
}

double tau(double p)
{
    double numerator = 0;
    double denominator = 0;

    for (uint64_t i = 0; i <= retry; i++) {
        double p_pow_i = pow(p, i);
        numerator += p_pow_i;

        uint64_t w_i = cwmin * (1 << i); // cwmin * 2^i
        if (w_i > cwmax) { w_i = cwmax; }
        denominator += p_pow_i * (double) (w_i + 1) / 2.;
    }

    return numerator / denominator;
}

double get_p(double tau, int64_t n)
{
    return 1. - pow(1. - tau, n - 1);
}

double solve_eq(double p, int64_t n)
{
    double new_p = get_p(tau(p), n);
    return p - new_p;
}

double get_tau(int64_t n)
{
    /* Bisection method */
    double p_l = 0;
    double p_r = 0;

    while (p_r - p_l > EPS) {
        double p_c = (p_r + p_l) / 2.;
        if (solve_eq(p_l, n) * solve_eq(p_c, n) < 0) {
            p_r = p_c;
        } else {
            p_l = p_c;
        }
    }
    return tau((p_l + p_r) / 2.);
}

double getThroughput(int64_t n)
{
    double tau = get_tau(n);
    double pe = pow(1. - tau, n);
    double ps = tau * n * pow(1. - tau, n - 1); 
    double pc = 1. - pe - ps;
    double Tavg = Te * pe + Ts * ps + Tc * pc;

    double S = 8. * L * ps / Tavg;
    return S;
}

int main(int argc, char **argv)
{
    int64_t n;
    n = parse_args(argc, argv);
    if (n == INVALID_ARGS) {
        perror("invalid arguments!\n");
        return -1;
    }
    FILE * fp = fopen(output_file, file_mode);

    if (!fp) {
        perror("Can't open file!\n");
        return -1;
    }

    double throughput = getThroughput(n);
    fprintf(fp, "n = %ld, throughput = %f Mb/s\n", n, throughput);
    fclose(fp);
    return 0;
}