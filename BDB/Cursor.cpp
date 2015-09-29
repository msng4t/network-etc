#include "Cursor.h"

bool CCursor::Start(mina::CMemblock &aoKey)
{
	Dbt loKey, dbvalue;

	loKey.set_data(aoKey.GetRead());
	loKey.set_size(aoKey.GetSize());

	if (mpoCursor->get(&loKey, &dbvalue, DB_SET) == 0)
	{
		if (aoKey.Write((char*)loKey.get_data(), loKey.get_size()))
				return true;
		else
			return false;
	}
	return false;
}

bool CCursor::Start(mina::CMemblock &aoKey, mina::CMemblock &value)
{
	Dbt loKey, dbvalue;

	loKey.set_data(aoKey.GetRead());
	loKey.set_size(aoKey.GetSize());

	if (mpoCursor->get(&loKey, &dbvalue, DB_SET_RANGE) == 0)
	{
		if (aoKey.Write((char*)loKey.get_data(), loKey.get_size()) &&
			value.Write((char*)dbvalue.get_data(), dbvalue.get_size()))
				return true;
		else
			return false;
	}
	return false;
}

bool CCursor::Delete()
{
	return (mpoCursor->del(0) == 0);
}

bool CCursor::Next(mina::CMemblock &aoKey, mina::CMemblock &value)
{
	Dbt loKey, dbvalue;
	
	if (mpoCursor->get(&loKey, &dbvalue, DB_NEXT) == 0)
	{
		if (aoKey.Write((char*)loKey.get_data(), loKey.get_size()) &&
			value.Write((char*)dbvalue.get_data(), dbvalue.get_size()))
				return true;
		else
			return false;
	}
	else
		return false;
}

bool CCursor::Prev(mina::CMemblock &aoKey, mina::CMemblock &value)
{
	Dbt loKey, dbvalue;

	if (mpoCursor->get(&loKey, &dbvalue, DB_PREV) == 0)
	{
		if (aoKey.Write((char*)loKey.get_data(), loKey.get_size()) &&
			value.Write((char*)dbvalue.get_data(), dbvalue.get_size()))
				return true;
		else
			return false;
	}
	else
		return false;
}

bool CCursor::Close()
{
	return (mpoCursor->close() == 0);
}
