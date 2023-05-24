#ifndef __SORTENTRY
#define __SORTENTRY
#include <string>
#include <iostream>
using namespace std;


class SortEntry
{
public:
	int id;
	double key;

	SortEntry()
	{
		id=-1;
		key=-1;
	}
	SortEntry(const SortEntry & se)
	{
		id = se.id;
		key = se.key;
	}

	 bool operator < (const SortEntry &a) const 
	{
		return (key<a.key); 
	 }
};

class TEntry
{
public:
 int id;
 string key;

TEntry()
{
id =-1;
}

 TEntry(const TEntry & se)
	{
		id = se.id;
		key = se.key;
	}

 bool operator < (const TEntry &a) const 
	{
		if(key <a.key)
			return true;
		else if(key == a.key)
			return id<a.id;
		else 
			return false;
	 }
};

class HEntry
{
public:
	unsigned * key;
	int keysize;
	int id;
	HEntry(unsigned * a, int s, int i)
	{
		key = a;
		keysize = s;
		id = i;
	}

	HEntry()
	{
		key = NULL;
	}

	~HEntry()
	{
		key = NULL;
	}

	HEntry(const HEntry &a)
	{
		keysize = a.keysize;
		key = a.key;
		id = a.id;
	}

	bool operator < (const HEntry &a) const 
	{
		bool flag = true;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				return flag;
			}
			else if(key[i]>a.key[i])
			{
				flag = false;
				return flag;
			}		
		}
		return id<a.id;
	 }

	bool operator > (const HEntry &a) const 
	{
		bool flag = false;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = true;
				break;
			}
		}
		return id>a.id;
	 }

	void print()
	{
		for(int i =0;i<keysize;i++)
		{
			cout<<key[i]<<" ";
		}
	}
};

class IntArray
{
public:
	unsigned * key;
	int keysize;
	int edist;
	bool f;
	IntArray(unsigned * a, int s)
	{
		key = a;
		a = NULL;
		keysize = s;
	}

	IntArray()
	{
		key = NULL;
	}

	~IntArray()
	{
		key = NULL;
		
	}

	IntArray(const IntArray &a)
	{
		keysize = a.keysize;
		key = a.key;
		edist = a.edist;
		f = a.f;
	}

	bool operator < (const IntArray &a) const 
	{
		bool flag = true;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = false;
				break;
			}
		}
		return flag;
	 }

	bool operator > (const IntArray &a) const 
	{
		bool flag = false;
		for(int i =0;i<keysize;i++)
		{
			if(key[i] < a.key[i])
			{
				break;
			}
			if(key[i]>a.key[i])
			{
				flag = true;
				break;
			}
		}
		return flag;
	 }

	void print()
	{
		for(int i =0;i<keysize;i++)
		{
			cout<<key[i]<<" ";
		}
	}
};

#endif