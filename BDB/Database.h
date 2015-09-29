#ifndef DATABASE_H
#define DATABASE_H

#include <db_cxx.h>
#include "common.h"
#include "Log.h"
#include "DatabaseConfig.h"

class CTable;
class CTransaction;
#define LG_TRACE(...)      mina::Instance<mina::CLog>()->Logout(mina::LOG_LVL_TRACE, __VA_ARGS__);

class CDatabase
{
	friend class CTable;
	friend class CTransaction;

public:
	CDatabase();
	~CDatabase();
	
	bool			Init(const DatabaseConfig& config);
	void			Shutdown();
	
	CTable*			GetTable(const char* name);
	
	void			OnCheckpointTimeout();
	void			Checkpoint();

private:
	DatabaseConfig	config;
	DbEnv*			env;
	CTable*			keyspace;
	bool			running;
};

void WarmCache(char* dbPath, unsigned cacheSize);

// global
extern CDatabase database;

#endif
