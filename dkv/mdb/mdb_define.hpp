/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: mdb_define.hpp 952 2012-06-27 04:25:09Z mobing $
 *
 * Authors:
 *   MaoQi <maoqi@taobao.com>
 *
 */
#ifndef MDB_DEFINE_H_
#define MDB_DEFINE_H_
#include <stdint.h>
#define TAIR_SLAB_LARGEST            100
#define TAIR_SLAB_BLOCK              1048576
#define TAIR_SLAB_ALIGN_BYTES        8
#define TAIR_SLAB_HASH_MAXDEPTH      8

#include <map>


#define LIKELY(x) (__builtin_expect(!!(x),1))
#define UNLIKELY(x) (__builtin_expect(!!(x),0))
#define IS_ADDCOUNT_TYPE(flag) (((flag) & TAIR_ITEM_FLAG_ADDCOUNT) == TAIR_ITEM_FLAG_ADDCOUNT)
#define IS_ITEM_TYPE(flag) ( ((flag) & TAIR_ITEM_FLAG_ITEM) )
#define IS_DELETED(flag) ( ((flag) & TAIR_ITEM_FLAG_DELETED) )

enum {
    TAIR_ITEM_FLAG_ADDCOUNT = 1,
    TAIR_ITEM_FLAG_DELETED = 2,
    TAIR_ITEM_FLAG_ITEM = 4,
    TAIR_ITEM_FLAG_LOCKED=8,
    TAIR_ITEM_FLAG_SET,
};

enum {
   TAIR_RETURN_SUCCESS = 0,
   TAIR_DUP_WAIT_RSP = 133,

   TAIR_RETURN_NOT_SUPPORTED= -4001,
   TAIR_RETURN_PROXYED = -4000,
   TAIR_RETURN_FAILED = -3999,
   TAIR_RETURN_DATA_NOT_EXIST = -3998,
   TAIR_RETURN_VERSION_ERROR = -3997,
   TAIR_RETURN_TYPE_NOT_MATCH = -3996,

   TAIR_RETURN_PLUGIN_ERROR = -3995,
   TAIR_RETURN_SERIALIZE_ERROR = -3994,
   TAIR_RETURN_ITEM_EMPTY = -3993,
   TAIR_RETURN_OUT_OF_RANGE = -3992,
   TAIR_RETURN_ITEMSIZE_ERROR = -3991,

   TAIR_RETURN_SEND_FAILED = -3990,
   TAIR_RETURN_TIMEOUT = -3989,
   TAIR_RETURN_DATA_EXPIRED = -3988,
   TAIR_RETURN_SERVER_CAN_NOT_WORK = -3987,
   TAIR_RETURN_WRITE_NOT_ON_MASTER = -3986,

   TAIR_RETURN_DUPLICATE_BUSY = -3985,
   TAIR_RETURN_MIGRATE_BUSY = -3984,

   TAIR_RETURN_PARTIAL_SUCCESS = -3983,
   TAIR_RETURN_INVALID_ARGUMENT = -3982,
   TAIR_RETURN_CANNOT_OVERRIDE = -3981,

   TAIR_RETURN_DEC_BOUNDS= -3980,
   TAIR_RETURN_DEC_ZERO= -3979,
   TAIR_RETURN_DEC_NOTFOUND= -3978,

   TAIR_RETURN_PROXYED_ERROR = -3977,

   TAIR_RETURN_INVAL_CONN_ERROR = -3970,
   TAIR_RETURN_HIDDEN = -3969,
   TAIR_RETURN_QUEUE_OVERFLOWED = -3968,
   TAIR_RETURN_SHOULD_PROXY = -3967,

   // for lock
   TAIR_RETURN_LOCK_EXIST = -3975,
   TAIR_RETURN_LOCK_NOT_EXIST = -3974,

   TAIR_RETURN_REMOVE_NOT_ON_MASTER= -4101,
   TAIR_RETURN_REMOVE_ONE_FAILED= -4102,

   TAIR_RETURN_DUPLICATE_IDMIXED= -5001,
   TAIR_RETURN_DUPLICATE_DELAY= -5002,
   TAIR_RETURN_DUPLICATE_REACK= -5003,
   TAIR_RETURN_DUPLICATE_ACK_WAIT= -5004,
   TAIR_RETURN_DUPLICATE_ACK_TIMEOUT= -5005,
   TAIR_RETURN_DUPLICATE_SEND_ERROR= -5006,

   TAIR_RETURN_REMOTE_NOTINITED= -5106,
   TAIR_RETURN_REMOTE_SLOW= -5107,
   TAIR_RETURN_REMOTE_NOTINIT= -5108,
   TAIR_RETURN_REMOTE_DISKSAVE_FAILED=-5109,
   TAIR_RETURN_REMOTE_MISS=-5110,
   TAIR_RETURN_REMOTE_RSP_FAILED=-5111,
   TAIR_RETURN_REMOTE_NOLOCAL=-5112,
};

const int TAIR_OPERATION_VERSION   = 1;
const int TAIR_OPERATION_DUPLICATE = 2;
const int TAIR_OPERATION_REMOTE    = 4;
const int TAIR_OPERATION_UNLOCK    = 8;
const int TAIR_DUPLICATE_BUSY_RETRY_COUNT = 10;

struct mdb_param
{
  static const char *mdb_type;
  static const char *mdb_path;
  static int64_t size;
  static int slab_base_size;
  static int page_size;
  static double factor;
  static int hash_shift;

  static int chkexprd_time_low;
  static int chkexprd_time_high;

  static int chkslab_time_low;
  static int chkslab_time_high;

  static std::map<uint32_t, uint64_t> default_area_capacity;
};

bool hour_range(int min, int max);


#endif /*MDB_DEFINE_H_ */
