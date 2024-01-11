#ifndef __GADGET_H
#define __GADGET_H

#include <stdio.h>


//----- typedefs -----

typedef FILE * FILEptr;
typedef void * voidptr;

//----- macros -----
#define FLOATZERO	1e-6
#define MAXREAL		1e20
//#define MAXINT      1000
//#define EPS 14
//#define BITS 10

//----- functions -----

void	blank_print			(long long _n);
long long		compfloats			(float _v1, float _v2);
void	error				(char *_msg, bool _exit);
void	get_leading_folder	(char *_path, char *_folder);
void	getFNameFromPath	(char *_path, char *_fname);
char	getnextChar			(char **_s);
void	getnextWord			(char **_s, char *_w);
bool	is_pow_of_2			(long long _v);
float	l2_dist_int			(long long *_p1, long long *_p2, long long _dim);
void	printINT_in_BIN		(long long _n, long long _m);





#endif
