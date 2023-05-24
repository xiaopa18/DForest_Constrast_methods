#include"m-entry.h"

extern double MAXDIST;

M_Entry::M_Entry()
{
	level = -1;
	num_piv = -1;
	mkey = -1;
	minkey = MAXDIST;
	maxkey = -1;
	ptr = -1;
	min = NULL;
	max = NULL;
	num = 0;
	pivots = NULL;
}

M_Entry::~M_Entry()
{
	if (pivots != NULL)
		delete[] pivots;
	pivots = NULL;
	if (max != NULL)
		delete[] max;
	max = NULL;
	if (min != NULL)
		delete[] min;
	min = NULL;
}

int M_Entry::read_from_buffer(char* _buf)
{
	int i;

	memcpy(&level, _buf, sizeof(int));
	i = sizeof(int);

	memcpy(&isleaf, &_buf[i], sizeof(bool));
	i += sizeof(bool);

	memcpy(&num_piv, &_buf[i], sizeof(int));
	i += sizeof(int);

	memcpy(&ptr, &_buf[i], sizeof(int));
	i += sizeof(int);


	memcpy(&num, &_buf[i], sizeof(int));
	i += sizeof(int);

	memcpy(&mkey, &_buf[i], sizeof(double));
	i += sizeof(double);

	memcpy(&mxkey, &_buf[i], sizeof(double));
	i += sizeof(double);

	memcpy(&minkey, &_buf[i], sizeof(double));
	i += sizeof(double);

	memcpy(&maxkey, &_buf[i], sizeof(double));
	i += sizeof(double);

	min = new double[num_piv];
	memcpy(min, &_buf[i], num_piv * sizeof(double));
	i += num_piv * sizeof(double);

	max = new double[num_piv];
	memcpy(max, &_buf[i], num_piv * sizeof(double));
	i += num_piv * sizeof(double);

	pivots = new int[MAXLEVEL];
	memcpy(pivots, &_buf[i], MAXLEVEL * sizeof(int));
	i += MAXLEVEL * sizeof(int);

	return i;
}

int M_Entry::write_to_buffer(char*_buf)
{
	int i;
	memcpy(_buf, &level, sizeof(int));
	i = sizeof(int);


	memcpy(&_buf[i], &isleaf, sizeof(bool));
	i += sizeof(bool);

	memcpy(&_buf[i], &num_piv, sizeof(int));
	i += sizeof(int);


	memcpy(&_buf[i], &ptr, sizeof(int));
	i += sizeof(int);

	memcpy(&_buf[i], &num, sizeof(int));
	i += sizeof(int);

	memcpy(&_buf[i], &mkey, sizeof(double));
	i += sizeof(double);

	memcpy(&_buf[i], &mxkey, sizeof(double));
	i += sizeof(double);

	memcpy(&_buf[i], &minkey, sizeof(double));
	i += sizeof(double);

	memcpy(&_buf[i], &maxkey, sizeof(double));
	i += sizeof(double);

	memcpy(&_buf[i], min, num_piv * sizeof(double));
	i += num_piv * sizeof(double);

	memcpy(&_buf[i], max, num_piv * sizeof(double));
	i += num_piv * sizeof(double);

	memcpy(&_buf[i], pivots, MAXLEVEL * sizeof(int));
	i += MAXLEVEL * sizeof(int);

	return i;
}