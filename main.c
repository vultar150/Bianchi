#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#define INVALID_ARGS -1

const double EPS = 1e-6;
const char * output_file = "../output2.txt";
const char * file_mode = "a";

const uint32_t cwmin = 15; // 2^4-1
const uint32_t cwmax = 1023; // 2^10-1
const uint32_t retry = 7; // number of tries allowed for transmission
const uint32_t Ts = 2870; // success duration = TData + SIFS + TAck
const uint32_t Tc = Ts; // no RTS/CTS, collision duration = success duration
const uint32_t Te = 9; // duration of empty slot 9 us
const uint32_t L = 2000; // packet size 2000 bytes

/**
 * Returns number of stations or -1 if args invalid
 */
int32_t parse_args(int argc, char **argv)
{
    if (argc != 2) {
        return -1;
    }
    int32_t res = strtol(argv[1], NULL, 10);
    return  res ? res : INVALID_ARGS;
}

double tau(double p)
{
    double numerator = (1. - pow(p, retry)) / (1 - p);
    double denominator = 0;

    for (uint32_t i = 0; i <= retry - 1; i++) {
        uint32_t w_i = (cwmin + 1) * (1 << i); // cwmin * 2^i
        if (w_i > (cwmax + 1)) { w_i = cwmax + 1; }
        denominator += pow(p, i) * ((w_i + 1.0) / 2.0);
    }
    return numerator / denominator;
}

double get_p(double tau, int32_t n)
{
    return 1. - pow(1. - tau, n - 1);
}

double solve_eq(double p, int32_t n)
{
    double new_p = get_p(tau(p), n);
    return p - new_p;
}

double get_tau(int32_t n)
{
    /* Bisection method */
    double p_l = 0;
    double p_r = 1.;

    while (p_r - p_l > EPS) {
        double p_c = (p_r + p_l) / 2.;
        double eq_l = solve_eq(p_l, n);
        double eq_c = solve_eq(p_c, n);
        if (eq_l > -EPS && eq_l < EPS) {
            return tau(p_l);
        } 
        if (eq_l * eq_c < 0) {
            p_r = p_c;
        } else {
            p_l = p_c;
        }
    }
    return tau((p_l + p_r) / 2.);
}

double getThroughput(int32_t n)
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
    int32_t n;
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
    fprintf(fp, "%d\t%f\n", n, throughput);
    fclose(fp);
    return 0;
}