/*
 * (C) 2007-2010 Alibaba Group Holding Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * wait object used for async communication
 *
 * Version: $miID: CWaitObject.hpp 983 2012-07-18 07:40:51Z ganyu.hfl@taobao.com $
 *
 * Authors:
 *   ruohai <ruohai@taobao.com>
 *     - Initial release
 *
 */
#ifndef TAIR_CWaitObject
#define TAIR_CWaitObject

#include <ext/hash_map>
#include "Synch.h"
#include "Define.h"
#include "Log.h"

using namespace std;
using namespace __gnu_cxx;

MINA_NAMESPACE_START
class CPasePacket
{
public:
  int ii;
  bool isRegularPacket(){return true;}
  int getPCode() { return 0; }
};

typedef int CodeType;
const CodeType NULL_PCODE = -1;


class CWaitObject {
  public:
    friend class CWaitManager;
    CWaitObject()
    {
      Init();
    }
    CWaitObject(const CodeType& aExecptCode)
      :miExceptCode(aExecptCode)
    {
      Init();
    }
    CWaitObject(const set<CodeType>& except_set)
      :miExceptCode_set(except_set)
    {
      Init();
    }

    CWaitObject (int aiCmd,void (*apfFunc)(int retcode, void* apoArg),void *apoArg)
    {
      Init();
      mCallback = apfFunc;
      mpoArg = apoArg;
      miCmd = aiCmd;
    }
    ~CWaitObject()
    {
      TSmartLock<CLock> loGuaid(moLock);
      if (mpoRespList != NULL) {
        for (uint32_t i = 0; i < mpoRespList->size(); i++) {
          if(mpoRespList->at(i) != NULL && mpoRespList->at(i)->isRegularPacket())
            delete mpoRespList->at(i);
        }
        delete mpoRespList;
        mpoRespList = NULL;
      }
      if (mpoResp != NULL && mpoResp->isRegularPacket()) {
        delete mpoResp;
        mpoResp = NULL;
      }
      miExceptCode_set.clear();
    }
    bool WaitDone(int aiCount, int aiTimeout = 0)
    {
      moWaitLock.Lock();
      while (miDownCount < aiCount) {
        if (moCond.Wait(moWaitLock, aiTimeout) == false) {
          moWaitLock.Unlock();
          return false;
        }
      }
      moWaitLock.Unlock();
      return true;
    }
    bool InsertPacket(CPasePacket* apoPack)
    {
      if (apoPack != NULL)
      {
        TSmartLock<CLock> loGuaid(moLock);
        if (miExceptCode != NULL_PCODE && miExceptCode == apoPack->getPCode())
          return false;
        if (!miExceptCode_set.empty() &&
            miExceptCode_set.find(apoPack->getPCode()) != miExceptCode_set.end())
          return false;
        if (mpoRespList != NULL) {
          mpoRespList->push_back(apoPack);
        } else if (mpoResp != NULL) {
          mpoRespList = new vector<CPasePacket*>();
          mpoRespList->push_back(mpoResp);
          mpoResp = NULL;
          mpoRespList->push_back(apoPack);
        } else {
          mpoResp = apoPack;
        }
      }
      Done();
      return true;
    }
    CPasePacket *get_packet(int index=0)
    {
      TSmartLock<CLock> loGuaid(moLock);
      if (mpoRespList != NULL) {
        if((size_t)index < mpoRespList->size())
          return mpoRespList->at(index);
        return NULL;
      } else {
        return mpoResp;
      }
    }
    int GetPackCount()
    {
      TSmartLock<CLock> loGuaid(moLock);
      if (mpoRespList != NULL) return mpoRespList->size();
      if (mpoResp != NULL) return 1;
      return 0;
    }
    int GetID() const
    {
      return miID;
    }
    void SetNoFree()
    {
      mpoResp = NULL;
      mpoRespList = NULL;
    }
    bool mbIsAsync() { return NULL != mCallback; }
    int AsyncResponse(int error_code)
    {
      if(mCallback)
      {
        mCallback(error_code,mpoArg);
      }
      return 0;
    }
    int GetCMD(){return miCmd;}
  private:
    int miID;
    void Init(){
      miDownCount = 0;
      miID = 0;
      mpoResp = NULL;
      mpoRespList = NULL;
      miExceptCode = NULL_PCODE;

      mCallback = NULL;
      mpoArg = NULL;
      miCmd = 0;
    }
    void Done() {
      moWaitLock.Lock();
      miDownCount ++;
      moCond.Signal();
      moWaitLock.Unlock();
    }

    CLock moLock;
    set<CodeType> miExceptCode_set;
    CodeType miExceptCode;

    CPasePacket *mpoResp;
    vector<CPasePacket*> *mpoRespList;
    CLock moWaitLock;
    CCondition moCond;
    int miDownCount;
  private: //below is callback function
    void (*mCallback)(int retcode, void* apoArg);
    void* mpoArg;
    int miCmd;
};

class CWaitManager {
  public:
    CWaitManager()
    {
      miWaitSeq = 1;
      asyncHanler = NULL;
      mpvCallArg = NULL;
      mbIsAsync = false;
      //need't start the timeout thread.
    }

    CWaitManager(int (*_call_func)(void* arg,CWaitObject *apoWaitObj),void *_caller)
    {
      miWaitSeq = 1;
      asyncHanler = _call_func;
      mpvCallArg = _caller;
      mbIsAsync = true;
    }

    ~CWaitManager()
    {
      TSmartLock<CLock> loGuaid(moLock);
      hash_map<int, CWaitObject*>::iterator it;
      for (it = moWaitMap.begin(); it != moWaitMap.end(); ++it) {
        delete it->second;
      }
    }
    CWaitObject* CreateWait()
    {
      CWaitObject *lpoWaitObj = new CWaitObject();
      AddNew(lpoWaitObj);
      return lpoWaitObj;
    }
    CWaitObject* CreateWait(const CodeType& aExecptCode)
    {
      CWaitObject *lpoWaitObj = new CWaitObject(aExecptCode);
      AddNew(lpoWaitObj);
      return lpoWaitObj;
    }
    CWaitObject* CreateWait(const set<CodeType>& except_set)
    {
      CWaitObject *lpoWaitObj = new CWaitObject(except_set);
      AddNew(lpoWaitObj);
      return lpoWaitObj;
    }
    CWaitObject* CreateWait(int cmd,void (*_pfunc)(int retcode, void* apoArg),void *apoArg,int aiTimeout = 0)
    {
      CWaitObject *lpoWaitObj = new CWaitObject(cmd,_pfunc,apoArg);
      AddNew(lpoWaitObj);
      return lpoWaitObj;
    }

    void destroy_CWaitObject(CWaitObject *apoWaitObj)
    {
      TSmartLock<CLock> loGuaid(moLock);
      moWaitMap.erase(apoWaitObj->miID);
      delete apoWaitObj;
    }
    void WakeupWait(int miID, CPasePacket *apoPack)
    {
      TSmartLock<CLock> loGuaid(moLock);
      hash_map<int, CWaitObject*>::iterator it;
      it = moWaitMap.find(miID);
      if (it != moWaitMap.end()) 
      {
        //now check the object need a async callback.so we just call the obejct's handler.
        CWaitObject *lpoWaitObj = it->second;
        if (apoPack == NULL) 
        {
          LOG_ERROR("[%d] apoPack is null.", miID);
        } else if (!lpoWaitObj->InsertPacket(apoPack))
        {
          LOG_DEBUG( "[pCode=%d] apoPack had been ignored", apoPack->getPCode());
          if (apoPack->isRegularPacket()) delete apoPack;
        }

        if(lpoWaitObj->mbIsAsync() &&asyncHanler)
        {
          moWaitMap.erase(lpoWaitObj->miID);
          LOG_DEBUG("async wakeup object apoPack,miID=%d\n",miID);
          asyncHanler(mpvCallArg,lpoWaitObj);
          return;
        }
        LOG_DEBUG("sync wakeup object apoPack,miID=%d\n",miID);
      } else {
        if (apoPack != NULL && apoPack->isRegularPacket()) delete apoPack;
        LOG_DEBUG("[%d] waitobj not found.", miID);
      }
    }

  private:
    void AddNew(CWaitObject *apoWaitObj)
    {
      TSmartLock<CLock> loGuaid(moLock);
      miWaitSeq ++;
      if (miWaitSeq == 0) miWaitSeq ++;
      apoWaitObj->miID = miWaitSeq;;
      moWaitMap[apoWaitObj->miID] = apoWaitObj;
    }

  private:
    hash_map<int, CWaitObject*> moWaitMap;
    int miWaitSeq;
    CLock moLock;
  private:
    //atomic_t packet_id_creater;
    bool mbIsAsync;
    int (*asyncHanler)(void * mpvHandler,CWaitObject * mpoObj);
    void *mpvCallArg ;
};

MINA_NAMESPACE_END
#endif
