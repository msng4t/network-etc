#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#ifndef _SKIPLIST_INCLUDE_
#define _SKIPLIST_INCLUDE_


#define FIXLEVEL                                \
    while ( this->miLevel > 1 && this->mpoHead->mapoNexts[this->miLevel - 1] == NULL) \
                this->miLevel--;

#define FIXLENGTH(node)                         \
    {                                           \
        struct Node* temp = node;              \
            while (temp) {                      \
                temp = temp->mapoNexts[0];      \
                this->miLength--;               \
            }                                   \
    }



template <typename K, typename V, int MAX_LVL = 10>
class CSkipList
{
public:
    struct Node
    {
        K             mtKey;
        int           miLevel;
        V            mtValue;
        struct Node* mpoPrev;
        struct Node* mapoNexts[];
    };

CSkipList()
{
    Init();
}

public:

Node* DeleteAfter(K aKey)
{
    struct Node* lpoNode;
    struct Node* mapoUpdate[100];

    lpoNode = Find(aKey, mapoUpdate);
    if (lpoNode != NULL)
    {
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
            mapoUpdate[i]->mapoNexts[i] = NULL;
        }

        this->mpoTail = lpoNode->mpoPrev;
    //    FIXLENGTH(lpoNode);
    {
        struct Node* temp = lpoNode;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
        FIXLEVEL;
    }
    return lpoNode;
}

void Init()
{
    this->mpoHead = GetMNode();
    assert(this->mpoHead != NULL);
    this->miLevel = 1;
    this->miLength = 0;
    this->mpoHead->mpoPrev = NULL;
    this->mpoHead->mtKey = 0;
    for(int i = 0; i < MAX_LVL; i++)
    {
        this->mpoHead->mapoNexts[i] = NULL;
    }
    this->mpoTail = NULL;
}

void Release(void)
{
    Node* lpoNode = this->mpoHead->mapoNexts[0];
    Node* lpoNext;

    while(lpoNode)
    {
        lpoNext = lpoNode->mapoNexts[0];
        free(lpoNode);
        lpoNode = lpoNext;
    }
}

void Insert(K aKey, V& aValue)
{
    struct Node* lpoNode = this->mpoHead;

    lpoNode = GetNode();
    lpoNode->mtKey = aKey;
    lpoNode->mtValue = aValue;
    Insert(*lpoNode);
}

void Insert(Node& aNode)
{
    struct Node* mapoUpdate[100];
    struct Node* lpoNode = this->mpoHead;
    int liLevel = aNode.miLevel;
    K   lKey = aNode.mtKey;

    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        while(lpoNode->mapoNexts[i] && lpoNode->mapoNexts[i]->mtKey > lKey)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
        mapoUpdate[i] = lpoNode;
    }

    lpoNode = &aNode;
    if (liLevel > this->miLevel)
    {
        for (int i = this->miLevel; i < liLevel; i++)
        {
            mapoUpdate[i] = this->mpoHead;
        }
        this->miLevel = liLevel;
    }

    for (int i = 0; i < liLevel; i++)
    {
        lpoNode->mapoNexts[i] = mapoUpdate[i]->mapoNexts[i];
        mapoUpdate[i]->mapoNexts[i] = lpoNode;
    }

    lpoNode->mpoPrev = (mapoUpdate[0] == this->mpoHead) ? NULL : mapoUpdate[0];
    if (lpoNode->mapoNexts[0])
    {
        lpoNode->mapoNexts[0]->mpoPrev = lpoNode;
    }
    else
    {
        this->mpoTail = lpoNode;
    }
    this->miLength++;

}

Node* DeleteLE(K aKey)
{
    struct Node* lpoNode;
    struct Node* mapoUpdate[100];

    if (this->miLength == 0)
    {
        return NULL;
    }

    lpoNode = FindLE(aKey, mapoUpdate, true);
    if (lpoNode != NULL)
    {
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
            mapoUpdate[i]->mapoNexts[i] = NULL;
        }

        this->mpoTail = lpoNode->mpoPrev;
    //    FIXLENGTH(lpoNode);
    {
        struct Node* temp = lpoNode;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
        FIXLEVEL;
    }
    return lpoNode;
}

Node* DeleteGE(K aKey)
{
    struct Node* lpoNode;
    struct Node* lpoNodePrev;
    struct Node* lpoNodeRet = this->mpoHead->mapoNexts[0];
    if (this->miLength == 0)
    {
        return NULL;
    }

    if (lpoNodeRet == NULL || lpoNodeRet->mtKey < aKey)
    {
        return NULL;
    }
    lpoNode = this->mpoHead->mapoNexts[0];
    lpoNodePrev = lpoNode;

    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        lpoNode = lpoNodePrev;
        while (lpoNode && lpoNode->mtKey >= aKey)
        {
            lpoNodePrev = lpoNode;
            lpoNode = lpoNode->mapoNexts[lpoNode->miLevel - 1];
        }
        if (i == 0 && lpoNode != NULL && lpoNode != mpoHead->mapoNexts[0])
        {
            lpoNode->mpoPrev->mapoNexts[0] = NULL;
            lpoNode->mpoPrev = NULL;
        }
        this->mpoHead->mapoNexts[i] = lpoNode;
    }

    //FIXLENGTH(lpoNodeRet);
    {
        struct Node* temp = lpoNodeRet;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
    FIXLEVEL;
    return lpoNodeRet;
}

int Compare(const K& a, const K& b)
{
    if (a < b) {
        return -1;
    } else if (a > b) {
        return +1;
    } else {
        return 0;
    }
}

Node* DeleteIn(K aiStart, K aiEnd)
{
    Node* lpoNode;
    Node* lpoNodeRet;
    struct Node* mapoUpdate[100];
//    int liMid;

    if (this->miLength == 0)
    {
        return NULL;
    }

    if (Compare(aiStart, aiEnd) < 0)
    {
        int aiTemp = aiStart;
        aiStart = aiEnd;
        aiEnd = aiTemp;
    }
    else if (!Compare(aiStart, aiEnd))
    {
        return this->Delete(aiStart);
    }

//    liMid = (this->mpoHead->mapoNexts[0]->mtKey + this->mpoTail->mtKey) >> (this->miLevel - 1);
//    lpoNode = FindLE(aiStart, mapoUpdate, liMid < aiStart);
    lpoNode = FindLE(aiStart, mapoUpdate, false);
    lpoNodeRet = lpoNode;
    // need some change ?
    if (lpoNodeRet == NULL || Compare(lpoNodeRet->mtKey, aiEnd) < 0)
    {
        return NULL;
    }
    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        lpoNode = mapoUpdate[i]->mapoNexts[i];
        while(lpoNode && Compare(lpoNode->mtKey, aiEnd) >= 0)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
        mapoUpdate[i]->mapoNexts[i] = lpoNode;
        if (i == 0 && lpoNode != NULL)
        {
            lpoNode->mpoPrev->mapoNexts[0] = NULL;
        }
    }

    //FIXLENGTH(lpoNodeRet);
    {
        struct Node* temp = lpoNodeRet;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
    FIXLEVEL;
    return lpoNodeRet;
}

Node* Delete(K aKey)
{
    struct Node* lpoNode;
    struct Node* mapoUpdate[100];

    if (this->miLength == 0)
    {
        return NULL;
    }

    lpoNode = Find(aKey, mapoUpdate);
    if (lpoNode != NULL)
    {
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
            if (mapoUpdate[i]->mapoNexts[i] == lpoNode)
            {
                mapoUpdate[i]->mapoNexts[i] = lpoNode->mapoNexts[i];
            }
        }

        if (lpoNode->mapoNexts[0] != NULL)
        {
            lpoNode->mapoNexts[0]->mpoPrev = lpoNode->mpoPrev;
        }
        else
        {
            this->mpoTail = lpoNode->mpoPrev;
        }
        lpoNode->mapoNexts[0] = NULL;

    //    FIXLENGTH(lpoNode);
    {
        struct Node* temp = lpoNode;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
        FIXLEVEL;
    }
    return lpoNode;
}

Node* Find(K aKey, struct Node* aapoUpdate[])
{
    struct Node* lpoNode = this->mpoHead;
    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        //while(lpoNode->mapoNexts[i] && Comparator(lpoNode->mapoNexts[i]->mtKey, aKey) > 0)
        while(lpoNode->mapoNexts[i] && Compare(lpoNode->mapoNexts[i]->mtKey, aKey) > 0)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
        aapoUpdate[i] = lpoNode;
    }
    if (lpoNode->mapoNexts[0] && Compare(lpoNode->mapoNexts[0]->mtKey, aKey) == 0)
    {
        return lpoNode->mapoNexts[0];
    }
    else
    {
        return NULL;
    }

}

Node* Find(K aKey)
{
    struct Node* lpoNode = this->mpoHead;
    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        //while(lpoNode->mapoNexts[i] && Comparator(lpoNode->mapoNexts[i]->mtKey, aKey) > 0)
        while(lpoNode->mapoNexts[i] && Compare(lpoNode->mapoNexts[i]->mtKey, aKey) > 0)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
    }
    if (lpoNode->mapoNexts[0] && Compare(lpoNode->mapoNexts[0]->mtKey, aKey) == 0)
    {
        return lpoNode->mapoNexts[0];
    }
    else
    {
        return NULL;
    }

}

Node* FindLE(K aKey, struct Node* aapoUpdate[], bool abReverse)
{
    struct Node* lpoNode = this->mpoHead;
    if (!this->mpoTail || Compare(this->mpoTail->mtKey, aKey) > 0)
    {
        return NULL;
    }
    if ( abReverse )
    {
        lpoNode = this->mpoTail;
        while (Compare(lpoNode->mtKey, aKey) <= 0 || lpoNode->miLevel != this->miLevel)
        {
            lpoNode = lpoNode->mpoPrev;
            if (lpoNode == NULL)
            {
                lpoNode = this->mpoHead;
                break;
            }
        }
    }

    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        while(lpoNode->mapoNexts[i] && Compare(lpoNode->mapoNexts[i]->mtKey, aKey) > 0)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
        aapoUpdate[i] = lpoNode;
    }

    return lpoNode->mapoNexts[0];

}

void Print(void)
{
    Node* lpoNode;

    printf("Length[%d]\n", this->miLength);
    for(int i = 0; i < this->miLevel; i++)
    {
        printf("LEVEL[%d]: ", i);
        lpoNode = this->mpoHead->mapoNexts[i];
        while(lpoNode)
        {
            printf("%d -> ", (int)(lpoNode->mtKey));
            lpoNode = lpoNode->mapoNexts[i];
        }
        printf("NULL\n");
    }
}

static Node* GetNode(void)
{
    int liLevel = rand() % MAX_LVL;
    if (liLevel == 0)
    {
        liLevel = MAX_LVL;
    }
    Node* lpoNode = (Node*) malloc(sizeof(Node) + liLevel * sizeof(struct Node*));
    lpoNode->miLevel = liLevel;
    assert(lpoNode != NULL);
    return lpoNode;
}

Node* GetMNode(void)
{
    Node* lpoNode = (Node*) malloc(sizeof(Node) + MAX_LVL* sizeof(struct Node*));
    lpoNode->miLevel = MAX_LVL;
    assert(lpoNode != NULL);
    return lpoNode;
}

int GetLength(void)
{
    return miLength;
}

class Iterator {
    public:
    inline Iterator(const CSkipList* list) {
        list_ = list;
        node_ = NULL;
    }

bool Valid() const {
  return node_ != NULL;
}

const K& Key() const {
  assert(Valid());
  return node_->mtKey;
}

const V& Value() const {
  assert(Valid());
  return node_->mtValue;
}

void Next() {
  assert(Valid());
  node_ = node_->mapoNexts[0];
}

void Prev() {
  // Instead of using explicit "prev" links, we just search for the
  // last node that falls before key.
  assert(Valid());
  node_ = list_->FindLE(node_->mtKey);
  if (node_ == list_->mpoHead) {
    node_ = NULL;
  }
}

void Seek(const K& target) {
  node_ = list_->FindGE(target, NULL);
}

void SeekToFirst() {
  node_ = list_->mpoHead->mapoNexts[0];
}

void SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->mpoHead) {
    node_ = NULL;
  }
}

        private:
            const CSkipList* list_;
            Node* node_;
            // Intentionally copyable
    };
Node* Delete(Node* apoNode)
{
    struct Node* mapoUpdate[100];
    struct Node* lpoNode = mpoHead;

    if (apoNode != NULL)
    {
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
          while(lpoNode->mapoNexts[i] && lpoNode->mapoNexts[i] != apoNode
              && Compare(lpoNode->mapoNexts[i]->mtKey, apoNode->mtKey) >= 0)
          {
            lpoNode = lpoNode->mapoNexts[i];
          }
          mapoUpdate[i] = lpoNode;
        }

        if (mapoUpdate[0]->mapoNexts[0] != apoNode)
        {
          return apoNode;
        }
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
            if (mapoUpdate[i]->mapoNexts[i] == lpoNode)
            {
                mapoUpdate[i]->mapoNexts[i] = lpoNode->mapoNexts[i];
            }
        }

        if (apoNode->mapoNexts[0] != NULL)
        {
            apoNode->mapoNexts[0]->mpoPrev = apoNode->mpoPrev;
        }
        else
        {
            this->mpoTail = apoNode->mpoPrev;
        }
        apoNode->mapoNexts[0] = NULL;

    //    FIXLENGTH(lpoNode);
    {
        struct Node* temp = apoNode;
            while (temp) {
                temp = temp->mapoNexts[0];
                this->miLength--;
            }
    }
        FIXLEVEL;
    }
    return apoNode;
}

Node* Delete(Iterator& aoIter)
{
    struct Node* lpoNode;

    if (!aoIter.Valid())
    {
        return NULL;
    }

    return this->Delete(aoIter.node_);
}
    void Insert(Node* apoNode) { Insert(*apoNode);}
    bool Empty(void) { return miLength == 0; }
/*
    void Init(int aiMaxLevel);
    void Release(void);
    void Insert(K aKey, V& aValue);
    void Insert(Node& aNode);
    Node* DeleteAfter(K aKey);;
    Node* DeleteLE(K aKey);
    Node* DeleteGE(K aKey);
    Node* DeleteIn(K aiStart, K aiEnd);
    Node* Delete(K aKey);
    Node* Find(K aKey, struct Node* mapoUpdate[]);
    Node* Find(K aKey);
    Node* FindLE(K aKey, struct Node* mapoUpdate[], bool abReverse = false);
    void Print(void);
    Node* GetNode();
    int GetLength(void);
    bool Empty(void) { return miLength == 0; }
    virtual int Compare(const K& a, const K& b);
    // Iteration over the contents of a skip list
    class Iterator {
        public:
            // Initialize an iterator over the specified list.
            // The returned iterator is not valid.
            explicit Iterator(const CSkipList* list);

            // Returns true iff the iterator is positioned at a valid node.
            bool Valid() const;

            // Returns the key at the current position.
            // REQUIRES: Valid()
            const K& Key() const;

            // Returns the key at the current position.
            // REQUIRES: Valid()
            const V& Value() const;

            // Advances to the next position.
            // REQUIRES: Valid()
            void Next();

            // Advances to the previous position.
            // REQUIRES: Valid()
            void Prev();

            // Advance to the first entry with a key >= target
            void Seek(const K& target);

            // Position at the first entry in list.
            // Final state of iterator is Valid() iff list is not empty.
            void SeekToFirst();

            // Position at the last entry in list.
            // Final state of iterator is Valid() iff list is not empty.
            void SeekToLast();

    Node* Delete(Iterator& aoIter);
    Node* Delete(Node* apoNode);
  */
private:
    struct Node *mpoHead;
    struct Node *mpoTail;
    int miLength;
    int miLevel;
};
#endif
