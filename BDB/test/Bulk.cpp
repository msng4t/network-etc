/*Berkeley DB 批量插入更新与删除用法示例
  2010-01-17 22:24
  在Berkeley DB 4.8之前，我们可以执行的唯一的批量数据库操作是批量读取。
  从Berkeley DB 4.8开始，Berkeley DB支持批量插入/更新/删除，并且用法也与批量读取相似。
  批量插入/更新/删除对Berkeley DB的更新性能提升非常大，是一个值得认真学习的新功能。
  本文就以一个示例程序展示批量插入和批量删除的用法。
  */
#include <db.h>
#include <string.h>
#include <stdlib.h>

#define NUM_KEY_INT 100
#define DATA_SIZE 100
#define KEY_SIZE 100
#define  xx x


DB_ENV *envp;
DB *dbp;
DB_TXN *tid;
char* progname = "TTTT";

DB_ENV * env_init( char *home, char *prefix, int cachesize);
int db_init( DB_ENV *dbenv, DB **dbpp, int dups, int pagesize);
/* 批量插入示例函数。*/
void *
run_bulk_insert()
{
    int raw_key[NUM_KEY_INT];
    char raw_data[DATA_SIZE];
    int bulk_size = 100;
    DBT key, data;
    int *insert_load;
    int insert_count, i, ret, op_flag;
    insert_count = 10000;

    char *key_buf, *data_buf;
    void *p;
    int j;

    /* Initialize structs and arrays */
    memset(raw_key, '1', KEY_SIZE);
    memset(raw_data, '1', DATA_SIZE);
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    tid = NULL;

    /* Initialize bulk insertion buffers */
    key_buf = (char*) malloc(KEY_SIZE * bulk_size * 2);
    data_buf = (char*) malloc(DATA_SIZE * bulk_size * 2);
    memset(key_buf, 0, KEY_SIZE * bulk_size * 2);
    memset(data_buf, 0, DATA_SIZE * bulk_size * 2);

    /* 
     * 初始化Bulk buffer.使用批量操作(bulk operations) 也就是
     * 批量插入/删除/更新/读取的时候，必须使用用户提供的内存。
     * 所以需要设置DBT对象的flags为DB_DBT_USERMEM，并且设置ulen成员而不是size成员。
     */
    key.data = key_buf;
    key.ulen = KEY_SIZE * bulk_size * 2;
    key.flags = DB_DBT_USERMEM;
    data.data = data_buf;
    data.ulen = DATA_SIZE * bulk_size * 2;
    data.flags = DB_DBT_USERMEM;

    op_flag = DB_MULTIPLE;/* 这个flag给put/get/del 表示执行批量插入/更新/读取/删除。 */

    /* 
     * 填充一个bulk buffer DBT 对象. 先调用DB_MULTIPLE_WRITE_INIT初始化该
     * DBT。必须传入一个工作指针p和data buffer DBT 对象。
     */
    DB_MULTIPLE_WRITE_INIT(p, &data);
    for (i = 0; i < bulk_size; i++) {
        /* 
         * 调用DB_MULTIPLE_WRITE_NEXT这个宏来向bulk buffer当中插入数据。
         * 需要确保bulk buffer足够大，否则会出现内存访问越界错误。
         *
         * 各参数说明：
         * p: 是这个宏内部使用的工作变量，由DB_MULTIPLE_WRITE_INIT初始化，并且必须在此处一直使用。
         * data: 是data buffer DBT对象。
         * raw_data: 是一个数据项所在的内存地址。你需要把你要装入的数据项传入这个参数。每个数据项
         * 可以含有任意长度的字节，长度限制是一个DBT的总长度限制，也就是2的32次方。
         * DATA_SIZE: 是本次宏调用的数据项长度。本例当中所有数据项长度相同，只是特例。完全可以
         * 使用变长的数据项。
         *
         * 循环结束后填充完成，这个data buffer当中有bulk_size个data，
         */
        raw_data[20] = i;
        DB_MULTIPLE_WRITE_NEXT(p, &data, raw_data, DATA_SIZE);
    }

    /* 
     * 批量插入insert_count条key/data pairs, 每一批插入bulk_size条key/data pairs. 
     * 本例当中我们只准备了一批数据，所以最终插入的数据是重复的，不过这不影响示例本身。
     */
    for (i = 0; i < insert_count / bulk_size; ) {
        /* 
         * 填充key buffer。填好后，这个key buffer当中有bulk_size个key，
         * 并且第i个key与data buffer 当中的第i个data做为一对key/data pair 
         * 被插入数据库当中(i = 0, 1, 2, ... bulk_size).
         */
        DB_MULTIPLE_WRITE_INIT(p, &key);
        for (j = i * bulk_size; j < (i + 1) * bulk_size; j++) {
            raw_key[0] = j;
            /* 在循环当中使用DB_MULTIPLE_WRITE_NEXT依次插入每条data到data buffer当中。
             * 循环结束后填充完成。*/
            DB_MULTIPLE_WRITE_NEXT(p, &key, raw_key, KEY_SIZE);
        }

        /* 启动事务准备批量插入。 */
        if ((ret = envp->txn_begin(envp, NULL, &tid, 0)) != 0) {
            envp->err(envp, ret, "[insert] DB_ENV->txn_begin");
            exit(EXIT_FAILURE);
        }

        /* 
         * 执行批量插入。key和data DBT 对象分别是key buffer和data buffer, 
         * 其中必然含有相同书目的key和data items,key buffer当中的第i个
         * key item与data buffer当中的第i个data item 作为一个Key/data pair
         * 被插入数据库中。(i = 0, 1, 2, ... bulk_size).
         */
        switch(ret = dbp->put(dbp, tid, &key, &data, op_flag)) {
            case 0: /* 批量插入操作成功，提交事务。*/
                if ((ret = tid->commit(tid, 0)) != 0) {
                    envp->err(envp, ret, "[insert] DB_TXN->commit");
                    exit(EXIT_FAILURE);
                }
                break;
            case DB_LOCK_DEADLOCK:
                /* 如果数据库操作发生死锁，那么必须abort事务。然后，可以选择重新执行该操作。*/
                if ((ret = tid->abort(tid)) != 0) {
                    envp->err(envp, ret, "[insert] DB_TXN->abort");
                    exit(EXIT_FAILURE);
                }
                continue;
            default:
                envp->err(envp, ret, "[insert] DB->put ([%d]%d)", i, insert_load[i]);
                exit(EXIT_FAILURE);
        }

        i++;
    }

    (void)free(key_buf);
    (void)free(data_buf);

    return (NULL);
}

/* 批量插入示例函数。*/
    void *
run_bulk_delete()
{
    int raw_key[NUM_KEY_INT];
    DBT key;
    DB_ENV *envp;
    int bulk_size = 100;
    DB *dbp;
    DB_TXN *tid;
    int *delete_load;
    int delete_count, i, ret, op_flag;

    char *key_buf;
    void *p;
    int j;

    /* Initialize structs and arrays */
    memset(raw_key, 0, KEY_SIZE);
    memset(&key, 0, sizeof(DBT));
    tid = NULL;

    /* 
     * 初始化批量删除使用的key buffer。由于批量删除不需要data，
     * 所以只需要初始化和填充key buffer。我们同样需要使用自己分配的内存。
     */
    key_buf = (char*) malloc(KEY_SIZE * bulk_size * 2);
    memset(key_buf, 0, KEY_SIZE * bulk_size * 2);

    /* 初始化key buffer DBT 对象，设置正确的flags和ulen成员。 */
    key.data = key_buf;
    key.ulen = KEY_SIZE * bulk_size * 2;
    key.flags = DB_DBT_USERMEM;
    op_flag = DB_MULTIPLE; /* 批量删除同样需要这个flag。*/

    /* 
     * 批量删除所有的数据。每一批删除由key buffer DBT 当中的key
     * 指定的bulk_size条key/data pair. 这两个宏的详细用法见上文。
     */
    for (i = 0; i < delete_count / bulk_size; ) {
        /* 为批量删除初始化并填充一个key buffer DBT 对象。 */
        DB_MULTIPLE_WRITE_INIT(p, &key);
        for (j = i * bulk_size; j < (i + 1) * bulk_size; j++) {
            raw_key[0] = delete_load[j];
            DB_MULTIPLE_WRITE_NEXT(p, &key, raw_key, KEY_SIZE);
        }
        /* 启动事务。*/
        if ((ret = envp->txn_begin(envp, NULL, &tid, 0)) != 0) {
            envp->err(envp, ret, "[delete] DB_ENV->txn_begin");
            exit(EXIT_FAILURE);
        }

        /* 
         * 执行批量删除。key buffer DBT
         * 当中的bulk_size条key指定的key/data pairs会被从数据库当中删除。 
         */
        switch(ret = dbp->del(dbp, tid, &key, op_flag)) {
            case 0: /* 批量删除操作成功，提交事务。*/
                if ((ret = tid->commit(tid, 0)) != 0) {
                    envp->err(envp, ret, "[delete] DB_TXN->commit");
                    exit(EXIT_FAILURE);
                }
                break;
            case DB_LOCK_DEADLOCK:
                /* 如果数据库操作发生死锁，那么必须abort事务。然后，可以选择重新执行该操作。*/
                if ((ret = tid->abort(tid)) != 0) {
                    envp->err(envp, ret, "[delete] DB_TXN->abort");
                    exit(EXIT_FAILURE);
                }
                continue;
            default:
                envp->err(envp, ret, "[delete] DB->del ([%d]%d)", i, delete_load[i]);
                exit(EXIT_FAILURE);
        }
        i++;
    }

    (void)free(key_buf);

    return (NULL);
}


int main()
{
    envp = env_init(".", "xx", 1024);
    db_init(envp, &dbp, 100, 1024);
    run_bulk_insert();

    return 0;
}

/*
 * db_init --
 *	Open the database.
 */
int db_init( DB_ENV *dbenv, DB **dbpp, int dups, int pagesize)
{
	DB *dbp;
	DB_TXN *txnp;
	int ret;

	dbp = NULL;
	txnp = NULL;
	ret = 0;

	if ((ret = db_create(&dbp, dbenv, 0)) != 0) {
		fprintf(stderr,
		    "%s: db_create: %s\n", progname, db_strerror(ret));
		return (ret);
	}
	dbp->set_errfile(dbp, stderr);
	dbp->set_errpfx(dbp, progname);
	if ((ret = dbp->set_pagesize(dbp, pagesize)) != 0) {
		dbp->err(dbp, ret, "set_pagesize");
		goto err;
	}
	if (dups && (ret = dbp->set_flags(dbp, DB_DUP)) != 0) {
		dbp->err(dbp, ret, "set_flags");
		goto err;
	}

	if ((ret = dbenv->txn_begin(dbenv, NULL, &txnp, 0)) != 0)
		goto err;

	if ((ret = dbp->open(dbp, txnp, "sss.db", "primary", DB_BTREE,
	    DB_CREATE , 0664)) != 0) {
		dbp->err(dbp, ret, "%s: open", "sss.db");
		goto err;
	}
	*dbpp = dbp;

	ret = txnp->commit(txnp, 0);
	txnp = NULL;
	if (ret != 0)
		goto err;

	return (0);

err:	if (txnp != NULL)
		(void)txnp->abort(0);
	if (dbp != NULL)
		(void)dbp->close(dbp, 0);
	return (ret);
}

/*
 * env_init --
 *	Initialize the environment.
 */
DB_ENV * env_init( char *home, char *prefix, int cachesize)
{
	DB_ENV *dbenv;
	int ret;

	if ((ret = db_env_create(&dbenv, 0)) != 0) {
		dbenv->err(dbenv, ret, "db_env_create");
		return (NULL);
	}
	dbenv->set_errfile(dbenv, stderr);
	dbenv->set_errpfx(dbenv, prefix);
	if ((ret = dbenv->set_cachesize(dbenv, 0, cachesize, 0)) != 0) {
		dbenv->err(dbenv, ret, "DB_ENV->set_cachesize");
		return (NULL);
	}

	if ((ret = dbenv->open(dbenv, home, DB_CREATE | DB_INIT_MPOOL |
	    DB_INIT_TXN | DB_INIT_LOCK, 0)) != 0) {
		dbenv->err(dbenv, ret, "DB_ENV->open: %s", home);
		(void)dbenv->close(dbenv, 0);
		return (NULL);
	}
	return (dbenv);
}
