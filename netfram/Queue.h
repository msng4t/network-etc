#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>


#define GENVAR(var) \
    inline CQueueNode*& var(void)  { return mpo##var;}\
    void inline var(CQueueNode* value) { this->mpo##var = value;}
#define GENBOOL(var) \
    virtual bool inline var(void)  { return mb##var;}\
    virtual void inline var(bool value) { this->mb##var = value;}

class CQueueNode
{
public:
    GENBOOL(In);
    GENVAR(Next);
    GENVAR(Prev);
    void Init()
    {
      Prev(this);
      Next(this);
      mbIn = false;
    }


    bool Empty()
    {
      return this == Prev();
    }

    // a -> this-> b
    // a -> this -> n -> b
    void InsertAM(CQueueNode* apoNode)
    {
      apoNode->Next(Next());
      apoNode->Prev(this);
      Next()->Prev(apoNode);
      Next(apoNode);
      apoNode->In(true);
    }

    // a -> this-> b
    // a -> n -> this -> b
    void InsertBM(CQueueNode* apoNode)
    {
      apoNode->Next(this);
      apoNode->Prev(Prev());
      Prev()->Next(apoNode);
      Prev(apoNode);
      apoNode->In(true);
    }

    // a -> n -> b
    // a -> b
    static bool Remove(CQueueNode* apoNode)
    {
      if (apoNode->In())
      {
        apoNode->Next()->Prev(apoNode->Prev());
        apoNode->Prev()->Next(apoNode->Next());
        apoNode->Next(NULL);
        apoNode->Prev(NULL);
        apoNode->In(false);
        return true;
      }
      return false;
    }

    void Add(CQueueNode* apoNode)
    {
      InsertAM(apoNode);
    }

    // apoNode as header node, self as header makes self tobe header
    // this-> apoPose -> b ->c, N-> d -> e -> f
    //    ||
    //    VV
    // this-> apoPose -> b ->c -> d -> e -> f, N
    void Join(CQueueNode* apoNode)
    {
      Prev()->Next(apoNode->Next());
      apoNode->Next()->Prev(Prev());
      Prev(apoNode->Prev());
      Prev()->Next(this);
      apoNode->Init();
    }

    CQueueNode* Head()
    {
      return Next();
    }


    CQueueNode* Tail()
    {
      return Prev();
    }

    CQueueNode* Sintinel()
    {
      return this;
    }

    //split S to S and D at p
    // S->a->b->c->p->d->e
    //     |
    //     V
    // S->a->b->c, D->p->d->e
    //
    void inline Split(CQueueNode *apoSource,CQueueNode *apoPose,CQueueNode *apoDest)
    {
      apoDest->Prev(apoSource->Prev());              
      apoDest->Prev()->Next(apoDest);                
      apoDest->Next(apoPose);                      
      apoSource->Prev(apoPose->Prev());              
      apoSource->Prev()->Next(apoSource);                
      apoPose->Prev(apoDest);
    }
    template <class T>
    T* Data()
    {
      return dynamic_cast<T*>(this);
    }
private:
    CQueueNode *mpoNext;
    CQueueNode *mpoPrev;
    bool        mbIn;
};


#endif
