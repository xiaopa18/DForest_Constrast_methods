#ifndef __M_Entry
#define __M_Entry
#include "..\gadget\gadget.h"
#include <memory.h>
//-----------------------------------------------

extern int MAXLEVEL;

class M_Node;

class M_Entry
{
public:

	int level;
	int num_piv;
	int num;
	double minkey;
	double maxkey;
	double mkey;
	double mxkey;
	double* min;
	double * max;
	int * pivots;
	int ptr;
	bool isleaf;

	//--==functions==--
	M_Entry();

	M_Entry(const M_Entry &m)
	{
		level = m.level;
		num_piv = m.num_piv;
		isleaf = m.isleaf;
		mxkey = m.mxkey;
		minkey = m.minkey;
		num= m.num;
		maxkey = m.maxkey;
		mkey = m.mkey;
		ptr = m.ptr;

		if (min != NULL)
		{
			delete[] min;
			min = NULL;
		}

		if (m.min != NULL)
		{
			min = new double[num_piv];
			memcpy(min, m.min, num_piv * sizeof(double));
		}

		if (max != NULL)
		{
			delete[] max;
			max = NULL;
		}

		if (m.max != NULL)
		{
			max = new double[num_piv];
			memcpy(max, m.max, num_piv * sizeof(double));
		}

		if (pivots != NULL)
		{
			delete[] pivots;
			pivots = NULL;
		}
		if (m.pivots != NULL)
		{
			pivots = new int[MAXLEVEL];
			memcpy(pivots, m.pivots, MAXLEVEL * sizeof(int));
		}
	}

	~M_Entry() ;

	int get_size()
	{
		return 4 * sizeof(int) + 4 * sizeof(double) + 2 * num_piv * sizeof(double) + MAXLEVEL*sizeof(int) + sizeof(bool);
	}

	int read_from_buffer(char *_buf);

	int write_to_buffer(char *_buf);
};

//-----------------------------------------------

#endif