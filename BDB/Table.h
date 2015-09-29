#ifndef TABLE_H
#define TABLE_H

#include "Database.h"
#include "Cursor.h"
#include <stdarg.h>
#include <math.h>
#include "Define.h"
#include "Memblock.h"

class CTableVisitor
{
public:
	virtual	~CTableVisitor() {}
	virtual bool Accept(const mina::CMemblock &key, const mina::CMemblock &value) = 0;
	virtual mina::CMemblock* GetStartKey() { return 0; }
	virtual void OnComplete() {}
	virtual bool IsForward() { return true; }
};

class CTable
{
	friend class CTransaction;
	friend class CDatabase;

public:
	CTable(CDatabase* database, const char *name, int pageSize = 0);
	~CTable();

	bool		Iterate(CTransaction* tx, CCursor& cursor);

	bool		Get(CTransaction* tx, mina::CMemblock &key, mina::CMemblock &value);
	bool		Get(CTransaction* tx, const char* key, mina::CMemblock &value);
	bool		Get(CTransaction* tx, const char* key, uint64_t &value);

	bool		Set(CTransaction* tx, mina::CMemblock &key, mina::CMemblock &value);
	bool		Set(CTransaction* tx, const char* key, mina::CMemblock &value);
	bool		Set(CTransaction* tx, const char* key, const char* value);
	bool		Set(CTransaction* tx, mina::CMemblock &key, uint64_t value);

	bool		Delete(CTransaction* tx, mina::CMemblock &key);
	bool		Prune(CTransaction* tx, mina::CMemblock &prefix, bool pruneExpiries = false);
	bool		Truncate(CTransaction* tx = NULL);

	bool		Visit(CTableVisitor &tv);

private:
	CDatabase*	database;
	Db*			db;

	bool		VisitBackward(CTableVisitor &tv);
};


#endif
