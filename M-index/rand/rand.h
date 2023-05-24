//--------------------------------------------
// this function contains necessary random number
// generators
//--------------------------------------------

#ifndef RAND_H
#define RAND_H
//--------------------------------------------
double gaussian(double mean, double sigma);
double new_uniform(double _min, double _max);
double normal_pdf(double _x, double _u, double _sigma);
double normal_cdf(double _x, double _step);
double uniform(double _min, double _max);
double zipf(double x1, double x2, double p);
//--------------------------------------------
#endif

