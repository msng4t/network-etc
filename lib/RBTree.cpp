#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include "RBTree.h"
/*

    template <typename K, typename V>
int CRBTree<K, V>::Compare(const K& a, const K& b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return +1;
    } else {
        return 0;
    }
}

    template <typename K, typename V>
void CRBTree<K, V>::Insert(Node* apoNode)
{
    Node*  lpoTemp;

//    PreTrival(this->mpoRoot);
//    MidTrival(this->mpoRoot);
    if (this->mpoRoot == &this->moSentinel)
    {
        apoNode->mpoParent = NULL;
        apoNode->mpoLeft = &this->moSentinel;
        apoNode->mpoRight = &this->moSentinel;
        RBT_BLACK(apoNode);

        this->mpoRoot = apoNode;
        return;
    }

//    PreTrival(this->mpoRoot);
//    MidTrival(this->mpoRoot);
    assert(this->mpoRoot != NULL);
    this->Insert(this->mpoRoot, apoNode);
//    PreTrival(this->mpoRoot);
//    MidTrival(this->mpoRoot);

    while (apoNode != this->mpoRoot && RBT_IS_RED(apoNode->mpoParent))
    {
        if (apoNode->mpoParent == apoNode->mpoParent->mpoParent->mpoLeft)
        {
            lpoTemp = apoNode->mpoParent->mpoParent->mpoRight;

            if (RBT_IS_RED(lpoTemp))
            {
                RBT_BLACK(apoNode->mpoParent);
                RBT_BLACK(lpoTemp);
                RBT_RED(apoNode->mpoParent->mpoParent);
                apoNode = apoNode->mpoParent->mpoParent;
            }
            else
            {
                if (apoNode == apoNode->mpoParent->mpoRight)
                {
                    apoNode = apoNode->mpoParent;
                    LeftRotate(apoNode);
                }

                RBT_BLACK(apoNode->mpoParent);
                RBT_RED(apoNode->mpoParent->mpoParent);
                RightRotate(apoNode->mpoParent->mpoParent);
            }
        }
        else
        {
            lpoTemp = apoNode->mpoParent->mpoParent->mpoLeft;

            if (RBT_IS_RED(lpoTemp))
            {
                RBT_BLACK(apoNode->mpoParent);
                RBT_BLACK(lpoTemp);
                RBT_RED(apoNode->mpoParent->mpoParent);
                apoNode = apoNode->mpoParent->mpoParent;
            }
            else
            {
                if (apoNode == apoNode->mpoParent->mpoLeft)
                {
                    apoNode = apoNode->mpoParent;
                    RightRotate(apoNode);
                }

                RBT_BLACK(apoNode->mpoParent);
                RBT_RED(apoNode->mpoParent->mpoParent);
                LeftRotate(apoNode->mpoParent->mpoParent);
            }
        }
    }

    RBT_BLACK(this->mpoRoot);
//    PreTrival(this->mpoRoot, &this->moSentinel);
//    MidTrival(this->mpoRoot, &this->moSentinel);

}

    template <typename K, typename V>
void CRBTree<K, V>::Delete(Node* apoNode)
{
    Node* subst;
    Node* lpoTemp;
    Node* tempw;
    UKey red;

//    PreTrival(this->mpoRoot, &this->moSentinel);
//    MidTrival(this->mpoRoot, &this->moSentinel);

    if (apoNode->mpoLeft == NULL || apoNode->mpoRight == NULL)
    {
        return;
    }

    if (apoNode->mpoLeft == &this->moSentinel)
    {
        lpoTemp = apoNode->mpoRight;
        subst = apoNode;
    }
    else if (apoNode->mpoRight == &this->moSentinel)
    {
        lpoTemp = apoNode->mpoLeft;
        subst = apoNode;
    }
    else
    {
        subst = Min(apoNode->mpoRight);

        if (subst->mpoLeft != &this->moSentinel)  *//*never reach *//*
        {
            lpoTemp = subst->mpoLeft;
        }
        else
        {
            lpoTemp = subst->mpoRight;
        }
    }

    if (subst == this->mpoRoot)
    {
        this->mpoRoot = lpoTemp;
        RBT_BLACK(lpoTemp);
        apoNode->mpoLeft = NULL;
        apoNode->mpoRight = NULL;
        apoNode->mpoParent = NULL;
        return;
    }

    red = RBT_IS_RED(subst);

    if (subst == subst->mpoParent->mpoLeft)
    {
        subst->mpoParent->mpoLeft = lpoTemp;
    }
    else
    {
        subst->mpoParent->mpoRight = lpoTemp;
    }

    if (subst == apoNode)
    {
        lpoTemp->mpoParent = subst->mpoParent;
    }
    else
    {
        if (subst->mpoParent == apoNode)
        {
            lpoTemp->mpoParent = subst;
        }
        else
        {
            lpoTemp->mpoParent = subst->mpoParent;
        }

        subst->mpoLeft = apoNode->mpoLeft;
        subst->mpoRight = apoNode->mpoRight;
        subst->mpoParent = apoNode->mpoParent;
        RBT_COPY_COLOR(subst, apoNode);

        if (apoNode == this->mpoRoot)
        {
            this->mpoRoot = subst;
        }
        else
        {
            if (apoNode == apoNode->mpoParent->mpoLeft)
            {
                apoNode->mpoParent->mpoLeft = subst;
            }
            else
            {
                apoNode->mpoParent->mpoRight = subst;
            }
        }

        if (subst->mpoLeft != &this->moSentinel)
        {
            subst->mpoLeft->mpoParent = subst;
        }

        if (subst->mpoRight != &this->moSentinel)
        {
            subst->mpoRight->mpoParent = subst;
        }
    }

    apoNode->mpoLeft = NULL;
    apoNode->mpoRight = NULL;
    apoNode->mpoParent = NULL;

    if (red)
    {
        return;
    }

    while (lpoTemp != this->mpoRoot && RBT_IS_BLACK(lpoTemp))
    {
        if (lpoTemp == lpoTemp->mpoParent->mpoLeft)
        {
            tempw = lpoTemp->mpoParent->mpoRight;
            if (RBT_IS_RED(tempw))
            {
                RBT_BLACK(tempw);
                RBT_RED(lpoTemp->mpoParent);
                LeftRotate(lpoTemp->mpoParent);
                tempw= lpoTemp->mpoParent->mpoRight;
            }

            if (RBT_IS_BLACK(tempw->mpoLeft) && RBT_IS_BLACK(tempw->mpoRight))
            {
                RBT_RED(tempw);
                lpoTemp = lpoTemp->mpoParent;
            }
            else
            {
                if (RBT_IS_BLACK(tempw->mpoRight))
                {
                    RBT_BLACK(tempw->mpoLeft);
                    RBT_RED(tempw);
                    RightRotate(tempw);
                    tempw = lpoTemp->mpoParent->mpoRight;
                }

                RBT_COPY_COLOR(tempw, lpoTemp->mpoParent);
                RBT_BLACK(lpoTemp->mpoParent);
                RBT_BLACK(tempw->mpoRight);
                LeftRotate(lpoTemp->mpoParent);
                lpoTemp = this->mpoRoot;
            }
        }
        else
        {
            tempw = lpoTemp->mpoParent->mpoLeft;

            if (RBT_IS_RED(tempw))
            {
                RBT_BLACK(tempw);
                RBT_RED(lpoTemp->mpoParent);
                RightRotate(lpoTemp->mpoParent);
                tempw = lpoTemp->mpoParent->mpoLeft;
            }

            if (RBT_IS_BLACK(tempw->mpoLeft) && RBT_IS_BLACK(tempw->mpoRight))
            {
                RBT_RED(tempw);
                lpoTemp = lpoTemp->mpoParent;
            }
            else
            {
                if (RBT_IS_BLACK(tempw->mpoLeft))
                {
                    RBT_BLACK(tempw->mpoRight);
                    RBT_RED(tempw);
                    LeftRotate(tempw);
                    tempw = lpoTemp->mpoParent->mpoLeft;
                }

                RBT_COPY_COLOR(tempw, lpoTemp->mpoParent);
                RBT_BLACK(lpoTemp->mpoParent);
                RBT_BLACK(tempw->mpoLeft);
                RightRotate(lpoTemp->mpoParent);
                lpoTemp = this->mpoRoot;
            }
        }

    }

    RBT_BLACK(lpoTemp);
}





    template <typename K, typename V>
void CRBTree<K, V>::Insert(Node* apoTemp, Node* apoNode)
{
    Node** nodep;

    for(;;)
    {
        nodep = ((IKey)(apoNode->mtKey - apoTemp->mtKey) < 0)
            ? &apoTemp->mpoLeft : &apoTemp->mpoRight;

        if (*nodep == &this->moSentinel)
        {
            break;
        }
        apoTemp = *nodep;
    }

    *nodep = apoNode;
    apoNode->mpoParent = apoTemp;
    apoNode->mpoLeft = &this->moSentinel;
    apoNode->mpoRight = &this->moSentinel;
    RBT_RED(apoNode);

}

    template <typename K, typename V>
void CRBTree<K, V>::LeftRotate(Node* apoNode)
{
    Node* lpoTemp;

    lpoTemp = apoNode->mpoRight;
    apoNode->mpoRight = lpoTemp->mpoLeft;
    //PreTrival(*this->mpoRoot, this->moSentinel);
    //MidTrival(*this->mpoRoot, this->moSentinel);

    if(lpoTemp->mpoLeft != &this->moSentinel)
    {
        lpoTemp->mpoLeft->mpoParent = apoNode;
    }

    lpoTemp->mpoParent = apoNode->mpoParent;

    if (apoNode == this->mpoRoot)
    {
        this->mpoRoot = lpoTemp;
    }
    else if (apoNode == apoNode->mpoParent->mpoLeft)
    {
        apoNode->mpoParent->mpoLeft = lpoTemp;
    }
    else
    {
        apoNode->mpoParent->mpoRight = lpoTemp;
    }

    lpoTemp->mpoLeft = apoNode;
    apoNode->mpoParent = lpoTemp;
    //PreTrival(*this->mpoRoot, this->moSentinel);
    //MidTrival(*this->mpoRoot, this->moSentinel);
}

    template <typename K, typename V>
void CRBTree<K, V>::RightRotate(Node* apoNode)
{
    Node* lpoTemp;

    lpoTemp = apoNode->mpoLeft;
    apoNode->mpoLeft = lpoTemp->mpoRight;
    //PreTrival(*this->mpoRoot, spSentinel);
    //MidTrival(*this->mpoRoot, spSentinel);

    if (lpoTemp->mpoRight != &this->moSentinel)
    {
        lpoTemp->mpoRight->mpoParent = apoNode;
    }

    lpoTemp->mpoParent = apoNode->mpoParent;

    if (apoNode == this->mpoRoot)
    {
        this->mpoRoot = lpoTemp;
    }
    else if (apoNode == apoNode->mpoParent->mpoRight)
    {
        apoNode->mpoParent->mpoRight = lpoTemp;
    }
    else
    {
        apoNode->mpoParent->mpoLeft = lpoTemp;
    }

    lpoTemp->mpoRight = apoNode;
    apoNode->mpoParent = lpoTemp;
    //PreTrival(*this->mpoRoot, this->moSentinel);
    //MidTrival(*this->mpoRoot, this->moSentinel);
}


    template <typename K, typename V>
void CRBTree<K, V>::PreTrival(Node* apoNode)
{
    if (apoNode == &this->moSentinel) return;
//    printf("v: %lld %s a: %x\t\t l: %p-r: %p -p:%p\n", apoNode->mtKey, RBT_IS_RED(apoNode) ? "r": "b", apoNode, apoNode->mpoLeft, apoNode->mpoRight, apoNode->mpoParent);
    if (apoNode->mpoLeft != &this->moSentinel)
        PreTrival(apoNode->mpoLeft);
    if (apoNode->mpoRight != &this->moSentinel)
        PreTrival(apoNode->mpoRight);
}

    template <typename K, typename V>
void CRBTree<K, V>::MidTrival(Node* apoNode)
{
    if (apoNode == &this->moSentinel) return;
    if (apoNode->mpoLeft != &this->moSentinel)
        MidTrival(apoNode->mpoLeft);
    //printf("v: %lld %s a: %x\t\t l: %p-r: %p -p:%p\n", apoNode->mtKey, RBT_IS_RED(apoNode) ? "r": "b", apoNode, apoNode->mpoLeft, apoNode->mpoRight, apoNode->mpoParent);
    if (apoNode->mpoRight != &this->moSentinel)
        MidTrival(apoNode->mpoRight);
}

    template <typename K, typename V>
CRBTree<K, V>::CRBTree()
{

    RBT_BLACK(&this->moSentinel);
    this->mpoRoot = &this->moSentinel;
}

template<typename K, typename V>
inline CRBTree<K, V>::Iterator::Iterator(CRBTree* list) {
  list_ = list;
  node_ = &list->moSentinel;
}

template<typename K, typename V>
inline bool CRBTree<K, V>::Iterator::Valid() const {
  return node_ != &list_->moSentinel;
}

template<typename K, typename V>
inline const K& CRBTree<K, V>::Iterator::Key() const {
  assert(Valid());
  return node_->mtKey;
}

template<typename K, typename V>
inline const V& CRBTree<K, V>::Iterator::Value() const {
  assert(Valid());
  return node_->mpoValue;
}

template<typename K, typename V>
inline void CRBTree<K, V>::Iterator::Next() {
  assert(Valid());
  if (node_->mpoRight != &list_->moSentinel)
  {
      node_ = node_->mpoRight;
      while(node_->mpoLeft != &list_->moSentinel)
      {
          node_ = node_->mpoLeft;
      }
  }
  else if (node_->mpoParent != NULL && node_ == node_->mpoParent->mpoLeft)
  {
      node_ = node_->mpoParent;
  }
  else if (node_->mpoParent != NULL)
  {
      while (node_->mpoParent->mpoParent != NULL
              && node_->mpoParent == node_->mpoParent->mpoParent->mpoRight)
      {
        node_ = node_->mpoParent;
      }
      node_ = node_->mpoParent->mpoParent;
      if (node_ == NULL)
      {
          node_ = &list_->moSentinel;
      }
  }
  else
  {
      node_ = &list_->moSentinel;
  }
}

template<typename K, typename V>
inline void CRBTree<K, V>::Iterator::Prev() {
  assert(Valid());
  if (node_ ->mpoLeft != &list_->moSentinel)
  {
      node_ = node_->mpoLeft;
      while(node_->mpoRight != &list_->moSentinel)
          node_ = node_->mpoRight;
  }
  else if (node_->mpoParent != NULL && node_ == node_->mpoParent->mpoRight)
  {
      node_ = node_->mpoParent;
  }
  else if (node_->mpoParent != NULL)
  {
      while (node_->mpoParent->mpoParent != NULL && node_->mpoParent == node_->mpoParent->mpoParent->mpoLeft)
      {
        node_ = node_->mpoParent;
      }
      node_ = node_->mpoParent->mpoParent;
      if (node_ == NULL)
      {
          node_ = &list_->moSentinel;
      }
  }
  else
  {
      node_ = &list_->moSentinel;
  }
}

template<typename K, typename V>
inline void CRBTree<K, V>::Iterator::Seek(const K& target) {
    node_ = list_->mpoRoot;
    while (node_ != &list_->moSentinel && node_->mtKey != target)
    {
        if (node_->mtKey > target)
        {
            node_ = node_->mpoLeft;
        }
        else
        {
            node_ = node_->mpoRight;
        }
    }
}

template<typename K, typename V>
inline void CRBTree<K, V>::Iterator::SeekToFirst() {
    node_ = list_->mpoRoot;
    while(node_->mpoLeft != &list_->moSentinel && node_!= &list_->moSentinel)
    {
        node_ = node_->mpoLeft;
    }
}

template<typename K, typename V>
inline void CRBTree<K, V>::Iterator::SeekToLast() {
    node_ = list_->mpoRoot;
    while(node_->mpoRight != &list_->moSentinel && node_!= &list_->moSentinel)
    {
        node_ = node_->mpoRight;
    }
}


template<typename K, typename V>
    typename CRBTree<K, V>::Node*
CRBTree<K, V>::Delete(Iterator& aoIter)
{
    struct Node* lpoNode;
}

int main()
{
    CRBTree<int, int> loTree;
    CRBTree<int, int>::Node loNode;
    CRBTree<int, int>::Node* lpoNode;
    loNode.mtKey = 10;
    loNode.mtValue = 100;

    loTree.Insert(&loNode);

    for(int re = 0; re < 2; re++)
    {
        for (int i = 0; i < 3; i++) {
            //for (int i = 0; i < 20; i++) {
            lpoNode = new CRBTree<int, int>::Node;
            lpoNode->mtKey = (rand() % 100000);
//            lpoNode->mtKey = i;
            loTree.Insert(lpoNode);
            loTree.MidTrival();
            loTree.PreTrival();
            //    printf(".");
        }


    }


    CRBTree<int, int> ::Iterator loIter(&loTree);

    loIter.SeekToFirst();
    for(; loIter.Valid(); loIter.Next())
    {
        printf("%d \n", loIter.Key());
    }

        printf("=========== \n");
    loIter.SeekToLast();
    for(; loIter.Valid(); loIter.Prev())
    {
        printf("%d \n", loIter.Key());
    }
//    printf("Spends:%d\n", time(NULL) - liStart);
    loTree.MidTrival();
    loTree.PreTrival();
///    loTree.Delete(&moTree, apoEvent->GetTimer());
    sleep(10000);
    return 0;
}  */
