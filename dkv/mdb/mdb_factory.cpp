/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Version: $Id: mdb_factory.cpp 998 2012-07-20 05:45:55Z zongdai $
 *
 * Authors:
 *   MaoQi <maoqi@taobao.com>
 *
 */
#include "mdb_factory.hpp"
#include "mdb_manager.hpp"
#include "mdb_define.hpp"
using namespace mina;
namespace tair {
  storage::storage_manager * mdb_factory::create_mdb_manager(void)
  {
    bool use_share_mem = true;
    if (strcmp(mdb_param::mdb_type, "mdb_shm") == 0)
    {
      use_share_mem = true;
    }
    else if (strcmp(mdb_param::mdb_type, "mdb") == 0)
    {
      use_share_mem = false;
    }
    else
    {
      LOG_ERROR("invalid mdb use memory type: %s. only support mdb_shm or mdb.", mdb_param::mdb_type);
      return NULL;
    }

    mdb_param::size *= (1 << 20);        //in MB

    assert(mdb_param::size > 0);
    assert(mdb_param::page_size > 0);

    // assert total mdb size is rounding to page_size.
    if (mdb_param::size % mdb_param::page_size != 0) {
       mdb_param::size = mdb_param::page_size * (mdb_param::size / mdb_param::page_size + 1); //round up
    }
    assert((mdb_param::factor - 1.0) > 0.0);

    LOG_DEBUG( "size:%lu,page_size:%d,m_factor:%f,m_hash_shift:%d",
              mdb_param::size, mdb_param::page_size, mdb_param::factor,
              mdb_param::hash_shift);

    storage::storage_manager * manager = 0;

    manager = new mdb_manager();
    if(manager != 0) {
      if(dynamic_cast<mdb_manager *>(manager)->initialize(use_share_mem)) {
        return manager;
      }
    }
    //FAIL
    if(manager != 0)
      delete manager;
    return 0;
  }

  storage::storage_manager * mdb_factory::create_embedded_mdb()
  {
    storage::storage_manager * manager = create_mdb_manager();
    if (manager == 0){
            return 0;
    }
    for(int i=0; i<TAIR_MAX_AREA_COUNT;++i) {
            manager->set_area_quota(i,0); // default clear all area quota, user should set area quota themselves
    }

    LOG_WARN("create embedded mdb, mem type: %s, cache size: %llu, page size: %d, factor: %f, hash_shift: %d.",
             mdb_param::mdb_type, mdb_param::size, mdb_param::page_size, mdb_param::factor, mdb_param::hash_shift);
    return manager;

  }

  void mdb_factory::parse_area_capacity_list(std::vector<const char *>&a_c)
  {
    for(std::vector<const char *>::iterator it = a_c.begin();
        it != a_c.end(); it++) {
      LOG_DEBUG("parse area capacity list: %s", *it);
      std::vector<char *>info;
      char tmp_str[strlen(*it) + 1];
      strcpy(tmp_str, *it);

      info.push_back(tmp_str);
      while(*tmp_str != '\0')
      {
          if (*tmp_str == ';')
          {
              *tmp_str = '\0';
              info.push_back(tmp_str + 1);
          }
      }
      for(uint32_t i = 0; i < info.size(); i++) {
        char *p = strchr(info[i], ',');
        if(p) {
          uint32_t area = strtoll(info[i], NULL, 10);
          uint64_t capacity = strtoll(p + 1, NULL, 10);
          mdb_param::default_area_capacity[area] = capacity;
          LOG_DEBUG("area %u capacity %llu", area, capacity);
        }
      }
    }
  }
}

/* tair */
