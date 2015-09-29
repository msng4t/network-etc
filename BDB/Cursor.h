#ifndef CURSOR_HPP
#define CURSOR_HPP

#include "Memblock.h"
#include <db_cxx.h>

class CTable; // forward

class CCursor
{
friend class CTable;

public:
	bool	Start(mina::CMemblock &key);
	bool	Start(mina::CMemblock &key, mina::CMemblock &value);
	bool	Delete();

	bool	Next(mina::CMemblock &key, mina::CMemblock &value);
	bool	Prev(mina::CMemblock &key, mina::CMemblock &value);

	bool	Close();

private:
	Dbc*	mpoCursor;
};

#endif
