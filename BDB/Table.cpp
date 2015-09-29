#include <db_cxx.h>

#include <math.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/param.h>
#include <sys/epoll.h>
#include "Table.h"
#include "Database.h"
#include "Transaction.h"

//#ifndef DB_READ_UNCOMMITTED
//#error Minimum BerkeleyDB version 4.6 is required for DB_READ_UNCOMMITTED!
//#endif

CTable::CTable(CDatabase* database, const char *name, int pageSize) :
database(database)
{
	DbTxn *txnid = NULL;
	const char *filename = name;
	const char *dbname = NULL;
	DBTYPE type = DB_BTREE;
	u_int32_t flags = DB_CREATE | DB_AUTO_COMMIT |
	DB_NOMMAP;// | DB_READ_UNCOMMITTED;

    flags = DB_CREATE          |
        DB_READ_UNCOMMITTED    | // Allow uncommitted reads   
        DB_AUTO_COMMIT         | // Allow autocommit
        DB_NOMMAP; // Cause the database to be free-threaded
	int mode = 0644;
	
	db = new Db(database->env, 0);
//	if (pageSize != 0)
//		db->set_pagesize(pageSize);
//		
	//Log_Trace();
	if (db->open(txnid, filename, dbname, type, flags, mode) != 0)
	{
		db->close(0);
		if (IsFolder(filename))
		{
			STOP_FAIL(rprintf(
					  "Could not create database file '%s' "
					  "because a folder '%s' exists",
					  filename, filename), 1);
		}
		STOP_FAIL("Could not open database", 1);
	}
	//Log_Trace();
}

CTable::~CTable()
{
	db->close(0);

	// Bug #222: On Windows deleting the BDB database object causes crash
	// On other platforms it works and causes a memleak if not deleted.
#ifndef PLATFORM_WINDOWS
	delete db;
#endif
}

bool CTable::Iterate(CTransaction* tx, CCursor& cursor)
{
	DbTxn* txn = NULL;

	if (tx)
		txn = tx->txn;
	
	if (db->cursor(txn, &cursor.mpoCursor, 0) == 0)
		return true;
	else
		return false;
}

bool CTable::Get(CTransaction* tx,
				mina::CMemblock &key,
				mina::CMemblock &value)
{
	Dbt dbtKey;
	Dbt dbtValue;
	DbTxn* txn = NULL;
	int ret;
	
	if (tx)
		txn = tx->txn;

	dbtKey.set_flags(DB_DBT_USERMEM);
	dbtKey.set_data(key.GetRead());
	dbtKey.set_ulen(key.GetSize());
	dbtKey.set_size(key.GetSize());
	
	dbtValue.set_flags(DB_DBT_USERMEM);
	dbtValue.set_data(value.GetRead());
	dbtValue.set_ulen(value.GetCapacity());
	
	ret = db->get(txn, &dbtKey, &dbtValue, 0);

	// DB_PAGE_NOTFOUND can occur with parallel PRUNE and GET operations
	// probably because we have DB_READ_UNCOMMITED turned on
	if (ret == DB_KEYEMPTY || ret == DB_NOTFOUND || ret == DB_PAGE_NOTFOUND)
		return false;
	
	if (dbtValue.get_size() > value.GetCapacity())
		return false;

	value.Write((char*)dbtValue.get_data(), dbtValue.get_size());
	
	return true;
}

bool CTable::Get(CTransaction* tx, const char* key, mina::CMemblock &value)
{
	int len;
	
	len = strlen(key);
	
	mina::CMemblock bsKey;
    bsKey.Init(len, (char*)key, key, len, MEM_DONT_DELETE);
	
	return CTable::Get(tx, bsKey, value);
}

bool CTable::Get(CTransaction* tx, const char* key, uint64_t& value)
{
	mina::CMemblock ba(32);
	unsigned nread;
	
	if (!Get(tx, key, ba))
		return false;
	
	value = strntouint64(ba.GetRead(), ba.GetSize(), &nread);
	
	if (nread != ba.GetSize())
		return false;
	
	return true;
}

bool CTable::Set(CTransaction* tx, mina::CMemblock &key, mina::CMemblock &value)
{
	Dbt dbtKey(key.GetRead(), key.GetSize());
	Dbt dbtValue(value.GetRead(), value.GetSize());
	DbTxn* txn = NULL;
	int ret;

	if (tx)
		txn = tx->txn;
	
	ret = db->put(txn, &dbtKey, &dbtValue, 0);
	if (ret != 0)
		return false;
	
	return true;
}

bool CTable::Set(CTransaction* tx,
				const char* key,
				mina::CMemblock &value)
{
	int len;
	
	len = strlen(key);
	
	mina::CMemblock bsKey;
    bsKey.Init(len, (char*)key, key, len, MEM_DONT_DELETE);

	return CTable::Set(tx, bsKey, value);
}

bool CTable::Set(CTransaction* tx,
				const char* key,
				const char* value)
{
	int len;
	
	len = strlen(key);
	mina::CMemblock bsKey;
    bsKey.Init(len, (char*)key, key, len, MEM_DONT_DELETE);
	
	len = strlen(value);
	mina::CMemblock bsValue;
    bsValue.Init(len, (char*) value, (char*) value, len, MEM_DONT_DELETE);
	
	return CTable::Set(tx, bsKey, bsValue);
}

bool CTable::Set(CTransaction* tx,
				mina::CMemblock &key,
				uint64_t value)
{
	mina::CMemblock ba(32);
    int liRet = 0;
	
//				n = snprintf(local, sizeof(local), "%" PRIi64 "", i64);
    liRet = snprintf(ba.GetRead(), ba.GetCapacity(), "%llu", value);
    ba.SetWrite(liRet);
	
	return Set(tx, key, ba);
}

bool CTable::Delete(CTransaction* tx, mina::CMemblock &key)
{
	Dbt dbtKey(key.GetRead(), key.GetSize());
	DbTxn* txn = NULL;
	int ret;

	if (tx)
		txn = tx->txn;
	
	ret = db->del(txn, &dbtKey, 0);
	if (ret < 0)
		return false;
	
	return true;
}

bool CTable::Prune(CTransaction* tx, mina::CMemblock &prefix, bool pruneExpiries)
{
	Dbc* cursor = NULL;
	u_int32_t flags = DB_NEXT;

	DbTxn* txn;
	
	if (tx == NULL)
		txn = NULL;
	else
		txn = tx->txn;

	if (db->cursor(txn, &cursor, 0) != 0)
		return false;
	
	Dbt key, value;
	if (prefix.GetSize() > 0)
	{
		key.set_data(prefix.GetRead());
		key.set_size(prefix.GetSize());
		flags = DB_SET_RANGE;		
	}
	
	while (cursor->get(&key, &value, flags) == 0)
	{
		if (key.get_size() < prefix.GetSize())
			break;
		
		if (strncmp(prefix.GetRead(), (char*)key.get_data(), prefix.GetSize()) != 0)
			break;

		flags = DB_NEXT;
		
		// don't delete keys starting with @@
		if (key.get_size() >= 2 &&
			((char*)key.get_data())[0] == '@' && ((char*)key.get_data())[1] == '@')
				continue;
		
		// don't delete keys starting with !!
		if (!pruneExpiries && key.get_size() >= 2 &&
			((char*)key.get_data())[0] == '!' && ((char*)key.get_data())[1] == '!')
				continue;

		cursor->del(0);
	}
	
	if (cursor)
		cursor->close();
	
	return true;
}

bool CTable::Truncate(CTransaction* tx)
{
	//Log_Trace();

	u_int32_t count;
	u_int32_t flags = 0;
	int ret;
	DbTxn* txn;
	
	txn = tx ? tx->txn : NULL;
	// TODO error handling
	if ((ret = db->truncate(txn, &count, flags)) != 0)
        return false;
		//Log_Trace("truncate() failed");
	return true;
}

bool CTable::Visit(CTableVisitor &tv)
{
	if (!tv.IsForward())
		return VisitBackward(tv);

	mina::CMemblock bsKey, bsValue;
	Dbc* cursor = NULL;
    char* lpcData = NULL;
    int liSize = 0;
	bool ret = true;
	u_int32_t flags = DB_NEXT;

	// TODO call tv.OnComplete() or error handling
	if (db->cursor(NULL, &cursor, 0) != 0)
		return false;
	
	Dbt key, value;
	if (tv.GetStartKey() && tv.GetStartKey()->GetSize() > 0)
	{
		key.set_data(tv.GetStartKey()->GetRead());
		key.set_size(tv.GetStartKey()->GetSize());
		flags = DB_SET_RANGE;		
	}
	
	while (cursor->get(&key, &value, flags | 0) == 0)
	{
        lpcData = (char*)key.get_data();
        liSize = key.get_size();
        bsKey.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

        lpcData = (char*)value.get_data();
        liSize = value.get_size();
        bsValue.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

		ret = tv.Accept(bsKey, bsValue);
		if (!ret)
			break;
		
		if (bsKey.GetSize() > 2 && bsKey.GetRead()[0] == '@' && bsKey.GetRead()[1] == '@')
		{
			key.set_data((void*)"@@~");
			key.set_size(3);
			flags = DB_SET_RANGE;
		}
		if (bsKey.GetSize() > 2 && bsKey.GetRead()[0] == '!' && bsKey.GetRead()[1] == '!')
		{
			key.set_data((void*)"!!~");
			key.set_size(3);
			flags = DB_SET_RANGE;
		}
        else
            flags = DB_NEXT;
	}
	
	cursor->close();	
	tv.OnComplete();
	
	return ret;
}

bool CTable::VisitBackward(CTableVisitor &tv)
{
	mina::CMemblock bsKey, bsValue;
	Dbc* cursor = NULL;
	bool ret = true;
    char* lpcData = NULL;
    int liSize = 0;
	u_int32_t flags = DB_PREV;

	// TODO call tv.OnComplete() or error handling
	if (db->cursor(NULL, &cursor, 0) != 0)
		return false;
	
	Dbt key, value;
	if (tv.GetStartKey() && tv.GetStartKey()->GetSize() > 0)
	{
		key.set_data(tv.GetStartKey()->GetRead());
		key.set_size(tv.GetStartKey()->GetSize());
		flags = DB_SET_RANGE;		

		// as DB_SET_RANGE finds the smallest key greater than or equal to the
		// specified key, in order to visit the database backwards, move to the
		// first matching elem, then move backwards
		if (cursor->get(&key, &value, flags | 0) != 0)
		{
			// end of database
			cursor->close();
			if (db->cursor(NULL, &cursor, 0) != 0)
				return false;
		}
		else
		{
			// if there is a match, call the acceptor, otherwise move to the
			// previous elem in the database
			if (memcmp(tv.GetStartKey()->GetRead(), key.get_data(),
				MIN(tv.GetStartKey()->GetSize(), key.get_size())) == 0)
			{
                lpcData = (char*)key.get_data();
                liSize = key.get_size();
                bsKey.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

                lpcData = (char*)value.get_data();
                liSize = value.get_size();
                bsKey.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

				ret = tv.Accept(bsKey, bsValue);
			}
		}
	}
		
	flags = DB_PREV;
	while (ret && cursor->get(&key, &value, flags | 0) == 0)
	{
        lpcData = (char*)key.get_data();
        liSize = key.get_size();
        bsKey.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

        lpcData = (char*)value.get_data();
        liSize = value.get_size();
        bsValue.Init(liSize, lpcData, lpcData, liSize, MEM_DONT_DELETE);

		ret = tv.Accept(bsKey, bsValue);
		if (!ret)
			break;

		flags = DB_PREV;
	}
	
	cursor->close();	
	tv.OnComplete();
	
	return ret;

}
