#include "TSKipList.h"

/*
    template <typename K, typename V>
typename CSkipList<K, V>::Node* CSkipList<K, V>::DeleteAfter(K aKey)
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

    template <typename K, typename V>
void CSkipList<K, V>::Init(int aiMaxLevel)
{
    this->mpoHead = GetNode(aiMaxLevel);
    assert(this->mpoHead != NULL);
    this->miLevel = 1;
    this->miLength = 0;
    this->miMaxLevel = aiMaxLevel;
    this->mpoHead->mpoPrev = NULL;
    this->mpoHead->mtKey = 0;
    for(int i = 0; i < aiMaxLevel; i++)
    {
        this->mpoHead->mapoNexts[i] = NULL;
    }
    this->mpoTail = NULL;
}

    template <typename K, typename V>
void CSkipList<K, V>::Release(void)
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::GetNode()
{
    int liLevel = rand() % this->miMaxLevel;
    Node* lpoNode = (Node*) malloc(sizeof(Node) + liLevel * sizeof(struct Node*));
    assert(lpoNode != NULL);
    return lpoNode;
}

    template <typename K, typename V>
void CSkipList<K, V>::Insert(K aKey, V& aValue)
{
    struct Node* lpoNode = this->mpoHead;
    int liLevel = rand() % this->miMaxLevel;
    if (liLevel == 0)
    {
        liLevel = this->miMaxLevel;
    }

    lpoNode = GetNode(liLevel);
    lpoNode->mtKey = aKey;
    lpoNode->miLevel = liLevel;
    lpoNode->mtValue = aValue;
    Insert(*lpoNode);
*//*
    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        while(lpoNode->mapoNexts[i] && lpoNode->mapoNexts[i]->mtKey > aKey)
        {
            lpoNode = lpoNode->mapoNexts[i];
        }
        mapoUpdate[i] = lpoNode;
    }

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
  *//*
}

template <typename K, typename V>
void CSkipList<K, V>::Insert(Node& aNode)
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

    template <typename K, typename V>
typename CSkipList<K, V>::Node* CSkipList<K, V>::DeleteLE(K aKey)
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

    template <typename K, typename V>
typename CSkipList<K, V>::Node* CSkipList<K, V>::DeleteGE(K aKey)
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
    for (int i = this->miLevel - 1; i >= 0; i--)
    {
        lpoNode = lpoNodePrev;
        while (lpoNode && lpoNode->mtKey >= aKey)
        {
            lpoNodePrev = lpoNode;
            lpoNode = lpoNode->mapoNexts[i];
        }
        this->mpoHead->mapoNexts[i] = lpoNode;
        if (i == 0 && lpoNode != NULL && lpoNode != mpoHead->mapoNexts[0])
        {
            lpoNode->mpoPrev->mapoNexts[0] = NULL;
            lpoNode->mpoPrev = NULL;
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

    template <typename K, typename V>
int CSkipList<K, V>::Compare(const K& a, const K& b)
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
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::DeleteIn(K aiStart, K aiEnd)
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::Delete(K aKey)
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::Find(K aKey, struct Node* aapoUpdate[])
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::Find(K aKey)
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::FindLE(K aKey, struct Node* aapoUpdate[], bool abReverse)
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

    template <typename K, typename V>
void CSkipList<K, V>::Print(void)
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

template <typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::GetNode(int aiLevel)
{
    Node* lpoNode = (Node*) malloc(sizeof(Node) + aiLevel * sizeof(struct Node*));
    assert(lpoNode != NULL);
    return lpoNode;
}

    template <typename K, typename V>
int CSkipList<K, V>::GetLength(void)
{
    return miLength;
}


template<typename K, typename V>
inline CSkipList<K, V>::Iterator::Iterator(const CSkipList* list) {
  list_ = list;
  node_ = NULL;
}

template<typename K, typename V>
inline bool CSkipList<K, V>::Iterator::Valid() const {
  return node_ != NULL;
}

template<typename K, typename V>
inline const K& CSkipList<K, V>::Iterator::Key() const {
  assert(Valid());
  return node_->mtKey;
}

template<typename K, typename V>
inline const V& CSkipList<K, V>::Iterator::Value() const {
  assert(Valid());
  return node_->mtValue;
}

template<typename K, typename V>
inline void CSkipList<K, V>::Iterator::Next() {
  assert(Valid());
  node_ = node_->mapoNexts[0];
}

template<typename K, typename V>
inline void CSkipList<K, V>::Iterator::Prev() {
  // Instead of using explicit "prev" links, we just search for the
  // last node that falls before key.
  assert(Valid());
  node_ = list_->FindLE(node_->mtKey);
  if (node_ == list_->mpoHead) {
    node_ = NULL;
  }
}

template<typename K, typename V>
inline void CSkipList<K, V>::Iterator::Seek(const K& target) {
  node_ = list_->FindGE(target, NULL);
}

template<typename K, typename V>
inline void CSkipList<K, V>::Iterator::SeekToFirst() {
  node_ = list_->mpoHead->mapoNexts[0];
}

template<typename K, typename V>
inline void CSkipList<K, V>::Iterator::SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->mpoHead) {
    node_ = NULL;
  }
}

template<typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::Delete(Node* apoNode)
{
    struct Node* mapoUpdate[100];
    if (apoNode != NULL)
    {
        for (int i = this->miLevel - 1; i >= 0; i--)
        {
            if (mapoUpdate[i]->mapoNexts[i] == apoNode)
            {
                mapoUpdate[i]->mapoNexts[i] = apoNode->mapoNexts[i];
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

template<typename K, typename V>
    typename CSkipList<K, V>::Node*
CSkipList<K, V>::Delete(Iterator& aoIter)
{
    struct Node* lpoNode;

    if (!aoIter.Valid())
    {
        return NULL;
    }

    return this->Delete(aoIter.node_);
}


*//*
int main() {
    srand((unsigned)time(0));
    int count = 200, i;

    printf("### Function Test ###\n");

    printf("=== Init Skip List ===\n");
    CSkipList<int, int > sl;
    sl.Init(10);
    time_t llStart = time(NULL);
    CSkipList<int, int> ::Node* lpoNode;

    lpoNode = (CSkipList<int, int> ::Node*) malloc ( 5* sizeof(CSkipList<int, int> ::Node*));
    printf("sizeof5 %lu\n", sizeof(*lpoNode));
    free(lpoNode);

    lpoNode = (CSkipList<int, int> ::Node*) malloc ( 1* sizeof(CSkipList<int, int>::Node*));
    printf("sizeof5 %lu\n", sizeof(*lpoNode));
    free(lpoNode);
    int vvvv = 100;
    for (int re = 0; re < 1000; re++)
    {
        for ( i = 0; i < count; i++) {
            sl.Insert(i, vvvv);
        }
        printf("Now Length %d\n", sl.GetLength());
        printf("Spends:%ld\n", time(NULL) - llStart);
        printf("=== Print Skip List ===\n");
        //            sl.Print();

        printf("=== Search Skip List ===\n");
        for (i = 0; i < count; i++) {
            int value = rand()%50000;
            lpoNode = sl.Find(value);
            if (lpoNode != NULL)
                if (value == lpoNode->mtKey)
                    printf("Found %d \n", lpoNode->mtKey);
                else
                    printf("ERROR %d \n", lpoNode->mtKey);
            else
                printf("not Found %d\n", value);
        }
        printf("=== Delete Skip List ===\n");
        for (i = 0; i < 10; i+=2) {
            printf("DeleteAfter[%d]: %s\n", i + 10, sl.DeleteAfter(i + 10)?"SUCCESS":"NOT FOUND");
            printf("DeleteLE[%d]: %s\n", i + 10, sl.DeleteLE(i + 10)?"SUCCESS":"NOT FOUND");
            printf("DeleteGE[%d]: %s\n", 100 - i - 5, sl.DeleteGE(100 -(i + 5))?"SUCCESS":"NOT FOUND");
            printf("DeleteIn[%d, %d]: %s\n", i + 5, i + 10, sl.DeleteIn(i+5, i+ 10)?"SUCCESS":"NOT FOUND");
            printf("Delete[%d]: %s\n", i + 20, sl.Delete(i + 10)?"SUCCESS":"NOT FOUND");
            //                sl.Print();
        }

        CSkipList<int, int> ::Iterator loIter(&sl);

        loIter.SeekToFirst();
        for(; loIter.Valid(); loIter.Next())
        {
            printf("%d \n", loIter.Key());
        }

        int liStart = 0;
        llStart = time(NULL);
        while (sl.GetLength() != 0)
        {
            liStart += rand() % 10;
            printf("Now Length %d d%d\n", sl.GetLength(), liStart);
            //            sl.Print();
            sl.DeleteLE(liStart);
            if (sl.GetLength() == 0)
                break;
        }
    }
    printf("Spends:%ld\n", time(NULL) - llStart);
    sleep(10000);
    sl.Release();
}
  */
