#include "Transaction.h"

CTransaction::CTransaction()
{
	active = false;
	database = NULL;
}

CTransaction::CTransaction(CDatabase* database_)
{
	active = false;
	database = database_;
}

CTransaction::CTransaction(CTable* table)
{
	active = false;
	database = table->database;
}

void CTransaction::Set(CDatabase* database_)
{
	database = database_;
}

void CTransaction::Set(CTable* table)
{
	database = table->database;
}

bool CTransaction::IsActive()
{
	return active;
}

bool CTransaction::Begin()
{
	// Log_Trace();

	if (database->env->txn_begin(NULL, &txn, 0) != 0)
		return false;

	active = true;
	return true;
}

bool CTransaction::Commit()
{
	// Log_Trace();
	
	active = false;
	
	if (txn->commit(0) != 0)
		return false;
	
	return true;
}

bool CTransaction::Rollback()
{
	// Log_Trace();
	
	active = false;
	
	if (txn->abort() != 0)
		return false;
	
	return true;
}
