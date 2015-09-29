#include "Database.h"
#include "Table.h"
#include "Transaction.h"
//#include "Application/Keyspace/Client/KeyspaceClient.h"
#include "Config.h"
#include "TestConfig.h"

void GenRandomString(CMemblock& aoBlock);

int DatabaseSetup();
int DatabaseSetTest(TestConfig& conf);

int main(int argc, char* argv[])
{
	TestConfig loTest;
    loTest.SetKeySize(10);
    loTest.SetValueSize(10);
    loTest.SetType("set");
    DatabaseSetup();
    DatabaseSetTest(loTest);
    return 0;
}

int DatabaseSetup()
{
	DatabaseConfig dbConfig;
	dbConfig.dir = Config::GetValue("database.dir", DATABASE_CONFIG_DIR);
	dbConfig.pageSize = Config::GetIntValue("database.pageSize", DATABASE_CONFIG_PAGE_SIZE);
	dbConfig.cacheSize = Config::GetIntValue("database.cacheSize", DATABASE_CONFIG_CACHE_SIZE);
	dbConfig.logBufferSize = Config::GetIntValue("database.logBufferSize", DATABASE_CONFIG_LOG_BUFFER_SIZE);
	dbConfig.checkpointTimeout = Config::GetIntValue("database.checkpointTimeout", DATABASE_CONFIG_CHECKPOINT_TIMEOUT);
	dbConfig.verbose = Config::GetBoolValue("database.verbose", DATABASE_CONFIG_VERBOSE);

	if (!database.Init(dbConfig))
		return 1;

	return 0;
}

int DatabaseSetTest(TestConfig& conf)
{
	int			numTest;
	Table*		table;
	Transaction* tx;
	bool		ret;
	int			limit = 16*KB;
	int			sum;
    char        lacKee[20];

	if (DatabaseSetup())
	{
		//Log_Message("Cannot initialize database!", 1);
		return 1;
	}

	table = database.GetTable("keyspace");
	if (!table)
	{
		//Log_Message("Cannot initialize table!", 1);
		return 1;
	}

	tx = NULL;
	tx = new Transaction(table);
	tx->Begin();

	sum = 0;
	numTest = conf.datasetTotal / conf.valueSize;
	for (int i = 0; i < numTest; i++)
	{
//##		if (conf.rndkey)
//##			GenRandomString(conf.key, conf.keySize);
//##		else
        conf.key.append("key");
        snprintf(lacKee, sizeof(lacKee), "%d", conf.padding.length());
        conf.key.append(lacKee);
        conf.key.append(conf.padding.c_str());
        snprintf(lacKee, sizeof(lacKee), "%d", i);
        conf.key.append(lacKee);

//			conf.key.Writef("key%B:%d", conf.padding.length, conf.padding.buffer, i);

		ret = table->Set(tx, conf.key.c_str(), conf.value.c_str());
		if (!ret)
		{
			//Log_Message("Test failed, ret = %s (%s failed after %d)", ret ? "true" : "false", conf.typeString, i);
			return 1;
		}

		sum += conf.keySize + conf.valueSize;
		if (sum > limit)
		{
			tx->Commit();


			tx->Begin();

			sum = 0;
		}
	}

	tx->Commit();


	return 0;
}
