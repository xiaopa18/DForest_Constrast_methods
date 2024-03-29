//--------------------------------------------
// this file contains necessary random number
// generators
//--------------------------------------------
#include <math.h>
#include <stdlib.h>
#include "rand.h"

/************************************************************
***  Given a mean and a standard deviation, gaussian       **
**   generates a normally distributed random number        **
**   Algorithm:  Polar Method, p.  104, Knuth, vol. 2      **
************************************************************/

double gaussian (double mean, double sigma)
{
   double v1, v2;
   double s;
   double x;

   do
   {
      v1 = 2*uniform(0, 1) - 1;
      v2 = 2*uniform(0, 1) - 1;
      s = v1*v1 + v2*v2;
   }
   while (s >= 1.);

   x = v1 * (double) sqrt ( -2. * log (s) / s);
   
   /*  x is normally distributed with mean 0 and sigma 1.  */
   x = x * sigma + mean;

   return (x);
}

/************************************************************
** Generates a random number between _min and _max         **
** uniformly                                               **
************************************************************/

double uniform(double _min, double _max)
{
	long long int_r = rand();
	long base = RAND_MAX-1;
	double f_r  = ((double) int_r) / base;
	return (_max - _min) * f_r + _min;
}

/*************************************************************/
/*  zipf generates a random number that follows Zipf         **
**  distribution and lies between x1 and x2.                 **
**  original code by Christos Faloutsos, 1995

**  The original node outputs discrete data only. The current**
**  function remedies the problem.			                 **
**************************************************************/
double zipf(double x1, double x2, double p)
{

   double x;
   double i;
   double r, HsubV, sum;
   long long V = 100;

   /* calculate the V-th harmonic number HsubV. WARNING: V>1 */ 
   HsubV = 0.0;
   for(i=1; i<=V; i++) 
      HsubV += 1.0/pow( (double)i, p);

   r = uniform(0., 1.)*HsubV;
   sum = 1.0; i=uniform(0,1);
   while( sum<r){
	  i+=uniform(1, 2);
      sum += 1.0/pow( (double)i, p);
   }

   x = ( (double) i - (double) 1. ) / ( (double) V - (double) 1. );
   x = (x2 - x1) * x + x1;
   
   return(x);
}

double new_uniform(long long _d_num)
{
	double base=1;
	double sum=0; 
	for (long long i=0; i<_d_num; i++)
	{
		long long digit = (long long) uniform(0, 10);
		if (digit==10) digit=9;
		sum+=base*digit;
		base*=10;
	}
	return sum;
}

double new_uniform(double _min, double _max)
{
	double ran_base = (double) 9999999999;
	double ran = new_uniform(10);
	return ran / ran_base * (_max - _min) + _min;
}

/*****************************************************************
calculates the pdf of the guassian distribution

para:
- x:
- u: mean
- sigma: standard devication

return:
- pdf(x)
*****************************************************************/

double normal_pdf(double _x, double _u, double _sigma)
{
	double PI = 3.14159265;

	double ret = exp(-(_x - _u) * (_x - _u) / (2 * _sigma * _sigma));
	ret /= _sigma * sqrt(2 * PI);

	return ret;
}

/*****************************************************************
calculates the cdf of the normal distribution (mean 0, var 1)

para:
- x:
- step: controls the precision of the result. recommended value:
  0.001.

return:
- cdf(x)
*****************************************************************/

double normal_cdf(double _x, double _step)
{
	double ret = 0;

	for (double i = -10; i < _x; i += _step)
	{
		ret += _step * normal_pdf((double) i, 0.0f, 1.0f);
	}

	return ret;
}