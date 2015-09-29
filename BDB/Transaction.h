#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Database.h"
#include "Table.h"

class CTransaction
{
	friend class CTable;
	
public:
	CTransaction();
	CTransaction(CDatabase* database);
	CTransaction(CTable* table);
	
	void		Set(CDatabase* database);	
	void		Set(CTable* table);
	bool		IsActive();
	bool		Begin();
	bool		Commit();
	bool		Rollback();
	
private:
	CDatabase*	database;
	DbTxn*		txn;
	bool		active;
};

#endif
