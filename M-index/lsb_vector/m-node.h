#ifndef _M_Node
#define _M_Node
#include"m-entry.h"
#include <vector>
using namespace std;
//-----------------------------------------------

class M_Entry;

class M_Node
{
public:
		//--==disk residential variables==--
	int level;
	int num_entries;
	M_Entry **entries;

	//--==others==--
	bool dirty;
	int block;


	//--==functions==--
	M_Node()
	{
		level = num_entries = 0;
		block = -1;
		dirty = false;
		entries = NULL;
	}

	~M_Node()
	{
		if (entries != NULL)
		{
			for (int i = 0; i < num_entries; i++)
			{
				delete entries[i];
				entries[i] = NULL;
			}
			delete[] entries;
		}	
	}

	M_Node(const M_Node& m)
	{
		level = m.level;
		num_entries = m.num_entries;
		dirty = m.dirty;
		entries = new M_Entry *[num_entries];
		for (int i = 0; i < m.num_entries; i++)
		{
			entries[i] = new M_Entry((*m.entries[i]));
		}
	}
	int get_header_size()
	{
		return sizeof(int) * 2;
	}


	void read_from_buffer(char *_buf)
	{
		int i;
		memcpy(&level, _buf, sizeof(level));
		i = sizeof(level);


		memcpy(&num_entries, &_buf[i], sizeof(num_entries));
		i += sizeof(num_entries);

		entries = new M_Entry * [num_entries];
		for (int j = 0; j < num_entries; j++)
		{
			entries[j] = new M_Entry();
			entries[j]->read_from_buffer(&_buf[i]);
			i += entries[j]->get_size();
		}
	}

	void write_to_buffer(char *_buf)
	{
		int i;
		memcpy(_buf, &level, sizeof(level));
		i = sizeof(level);
		memcpy(&_buf[i], &num_entries, sizeof(num_entries));
		i += sizeof(num_entries);

		for (int j = 0; j < num_entries; j++)
		{
			entries[j]->write_to_buffer(&_buf[i]);
			i += entries[j]->get_size();
		}
	}	

	void init(BlockFile *_M_Tree, int _block)
	{
		dirty = false;
		
		int b_len = _M_Tree->get_blocklength();

		block = _block;
		char *blk = new char[b_len];

		_M_Tree->read_block(blk, block);
		if (block == 0)
			_M_Tree->set_header(blk);

		read_from_buffer(blk);
		delete[] blk;
	}
	
};

//-----------------------------------------------

#endif