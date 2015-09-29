#ifndef TESTCONFIG_H
#define TESTCONFIG_H

class TestConfig
{
public:
	enum Type
	{
		GET,
		DIRTYGET,
		SET,
		LIST,
		LISTP,
		SUITE,
		API,
		FAILOVER
	};
	
	int					type;
	const char*			typeString;
	int					keySize;
	int					valueSize;
	int					datasetTotal;
    std::string		key;
    std::string		value;
    std::string		padding;
	bool				rndkey;
	
	TestConfig()
	{
        datasetTotal = 10000;
        valueSize = 10;
		rndkey = false;
	}
	
	void SetKeySize(int keySize_)
	{
		keySize = keySize_;

		for (int i = 0; i < keySize - 10; i++)
		{
			char c = (char) (i % 10) + '0';
			padding.append(&c, 1);
		}
	}
	
	void SetValueSize(int valueSize_, bool fill = true)
	{
		valueSize = valueSize_;

        value.clear();
        for (int i = 0; i < valueSize; i++)
        {
            char c = (char) (i % 10) + '0';
            value.append(&c, 1);
        }
    }
	
	void SetType(const char* s)
	{
		typeString = s;
		if (strcmp(s, "get") == 0)
		{
			type = GET;
			return;
		}
		if (strcmp(s, "dirtyget") == 0)
		{
			type = DIRTYGET;
			return;
		}
		if (strcmp(s, "set") == 0)
		{
			type = SET;
			return;
		}
		if (strcmp(s, "rndset") == 0)
		{
			type = SET;
			rndkey = true;
			return;
		}
		if (strcmp(s, "list") == 0)
		{
			type = LIST;
			return;
		}
		if (strcmp(s, "listp") == 0)
		{
			type = LISTP;
			return;
		}
		if (strcmp(s, "suite") == 0)
		{
			type = SUITE;
			return;
		}
		if (strcmp(s, "api") == 0)
		{
			type = API;
			return;
		}
		if (strcmp(s, "failover") == 0)
		{
			type = FAILOVER;
			return;
		}
	}
};


#endif
