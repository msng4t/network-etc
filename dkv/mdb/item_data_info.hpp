/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * this structure holds meta info of item
 *
 * Version: $Id: item_data_info.hpp 938 2012-06-15 08:01:55Z yexiang $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - initial release
 *
 */
#ifndef TAIR_ITEM_DATA_INFO
#define TAIR_ITEM_DATA_INFO
#include "Log.h"
#include "Memblock.h"
using namespace mina;
namespace tair {
   typedef struct _item_meta {
      uint16_t magic;
      uint16_t checksum;
      uint16_t keysize; // key size max: 64KB
      uint16_t version;
      uint32_t padsize; // padding size: 64KB
      uint32_t valsize : 24; // value size
      uint8_t flag; // for extends
      uint32_t cdate; // item create time
      uint32_t mdate; // item last modified time
      uint32_t edate; // expire date

      void encode(mina::CMemblock *output) const
     {
         // 29 bytes 
         output->Write(magic);
         output->Write(checksum);
         output->Write(keysize);
         output->Write(version);
         output->Write(padsize);
         output->Write(valsize);
         output->Write(flag);
         output->Write(cdate);
         output->Write(mdate);
         output->Write(edate);
      }

      void decode(mina::CMemblock *input)
      {
         uint32_t luVal;
         input->Read(magic);
         input->Read(checksum);
         input->Read(keysize);
         input->Read(version);
         input->Read(padsize);
         input->Read(luVal);
         valsize = luVal & 0xFFFFFF;
         input->Read(flag);
         input->Read(cdate);
         input->Read(mdate);
         input->Read(edate);
      }

      _item_meta& operator=(const _item_meta &rhs) {
        if (this != &rhs) {
          memcpy(this, &rhs, sizeof(*this));
        }
        return *this;
      }

      void log_self()
      {
         LOG_DEBUG("meta info of data: keysize[%d], valsize[%d], padsize[%d], version[%d], flag[%d], cdate[%d], mdate[%d], edate[%d]", keysize, valsize, padsize, version, flag, cdate, mdate, edate);
      }
   } item_meta_info;

   static const size_t ITEM_HEADER_LEN = sizeof(item_meta_info);

   typedef struct _ItemData{
      item_meta_info header;
      char m_data[0];
   }item_data_info;
}
#endif
