#ifndef _MINA_RBTREE_H_INCLUDE
#define _MINA_RBTREE_H_INCLUDE
#include <unistd.h>
#include <inttypes.h>
#include <assert.h>

typedef uint64_t UKey;
typedef int IKey;
typedef unsigned char uchar_t;

#define RBT_RED(apoNode) ((apoNode)->mcColor = 1);
#define RBT_BLACK(apoNode) ((apoNode)->mcColor = 0)
#define RBT_IS_RED(apoNode) ((apoNode)->mcColor)
#define RBT_IS_BLACK(apoNode) (!RBT_IS_RED(apoNode))
#define RBT_COPY_COLOR(node1, node2) ((node1)->mcColor = (node2)->mcColor)
#define MINA_RBTREE_SENTINEL_INIT(apoNode) RBT_BLACK(apoNode)


template <typename K>
class CRBTree
{
    public:

        CRBTree()
        {
            RBT_BLACK(&this->moSentinel);
            this->mpoRoot = &this->moSentinel;
        }
        virtual int Compare(const K& a, const K& b)
        {
            if (a < b) {
                return -1;
            } else if (a > b) {
                return +1;
            } else {
                return 0;
            }
        }

        struct Node
        {
            K mtKey;
            Node* mpoLeft;
            Node* mpoRight;
            Node* mpoParent;
            uchar_t mcColor;
            virtual void just(){}
//            V mtValue;
        };

        void Insert(Node* apoNode)
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

        void Delete(Node* apoNode)
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
                subst = mpoRoot;
                while(subst->mpoLeft != &moSentinel && subst!= &moSentinel)
                {
                    subst = subst->mpoLeft;
                }

                if (subst->mpoLeft != &this->moSentinel)  /*never reach */
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

        void PreTrival(Node* apoNode)
        {
            if (apoNode == &this->moSentinel) return;
            //    printf("v: %lld %s a: %x\t\t l: %p-r: %p -p:%p\n", apoNode->mtKey, RBT_IS_RED(apoNode) ? "r": "b", apoNode, apoNode->mpoLeft, apoNode->mpoRight, apoNode->mpoParent);
            if (apoNode->mpoLeft != &this->moSentinel)
                PreTrival(apoNode->mpoLeft);
            if (apoNode->mpoRight != &this->moSentinel)
                PreTrival(apoNode->mpoRight);
        }

        void MidTrival(Node* apoNode)
        {
            if (apoNode == &this->moSentinel) return;
            if (apoNode->mpoLeft != &this->moSentinel)
                MidTrival(apoNode->mpoLeft);
            //printf("v: %lld %s a: %x\t\t l: %p-r: %p -p:%p\n", apoNode->mtKey, RBT_IS_RED(apoNode) ? "r": "b", apoNode, apoNode->mpoLeft, apoNode->mpoRight, apoNode->mpoParent);
            if (apoNode->mpoRight != &this->moSentinel)
                MidTrival(apoNode->mpoRight);
        }
        void MidTrival() { MidTrival(this->mpoRoot); }
        void PreTrival() { PreTrival(this->mpoRoot); }

        class Iterator {
            public:
                Iterator(CRBTree* list) {
                    mpoTree = list;
                    mpoNode = &list->moSentinel;
                }

                bool Valid() const {
                    return mpoNode != &mpoTree->moSentinel;
                }

                const K& Key() const {
                    assert(Valid());
                    return mpoNode->mtKey;
                }

                Node* Value()
                {
                    assert(Valid());
                    return mpoNode;
                }

                void Next() {
                    assert(Valid());
                    if (mpoNode->mpoRight != &mpoTree->moSentinel)
                    {
                        mpoNode = mpoNode->mpoRight;
                        while(mpoNode->mpoLeft != &mpoTree->moSentinel)
                        {
                            mpoNode = mpoNode->mpoLeft;
                        }
                    }
                    else if (mpoNode->mpoParent != NULL && mpoNode == mpoNode->mpoParent->mpoLeft)
                    {
                        mpoNode = mpoNode->mpoParent;
                    }
                    else if (mpoNode->mpoParent != NULL)
                    {
                        while (mpoNode->mpoParent->mpoParent != NULL
                                && mpoNode->mpoParent == mpoNode->mpoParent->mpoParent->mpoRight)
                        {
                            mpoNode = mpoNode->mpoParent;
                        }
                        mpoNode = mpoNode->mpoParent->mpoParent;
                        if (mpoNode == NULL)
                        {
                            mpoNode = &mpoTree->moSentinel;
                        }
                    }
                    else
                    {
                        mpoNode = &mpoTree->moSentinel;
                    }
                }

                void Prev()
                {
                    assert(Valid());
                    if (mpoNode ->mpoLeft != &mpoTree->moSentinel)
                    {
                        mpoNode = mpoNode->mpoLeft;
                        while(mpoNode->mpoRight != &mpoTree->moSentinel)
                            mpoNode = mpoNode->mpoRight;
                    }
                    else if (mpoNode->mpoParent != NULL && mpoNode == mpoNode->mpoParent->mpoRight)
                    {
                        mpoNode = mpoNode->mpoParent;
                    }
                    else if (mpoNode->mpoParent != NULL)
                    {
                        while (mpoNode->mpoParent->mpoParent != NULL && mpoNode->mpoParent == mpoNode->mpoParent->mpoParent->mpoLeft)
                        {
                            mpoNode = mpoNode->mpoParent;
                        }
                        mpoNode = mpoNode->mpoParent->mpoParent;
                        if (mpoNode == NULL)
                        {
                            mpoNode = &mpoTree->moSentinel;
                        }
                    }
                    else
                    {
                        mpoNode = &mpoTree->moSentinel;
                    }
                }

                void Seek(const K& target)
                {
                    mpoNode = mpoTree->mpoRoot;
                    while (mpoNode != &mpoTree->moSentinel && mpoNode->mtKey != target)
                    {
                        if (mpoNode->mtKey > target)
                        {
                            mpoNode = mpoNode->mpoLeft;
                        }
                        else
                        {
                            mpoNode = mpoNode->mpoRight;
                        }
                    }
                }

                void SeekToFirst()
                {
                    mpoNode = mpoTree->mpoRoot;
                    while(mpoNode->mpoLeft != &mpoTree->moSentinel && mpoNode!= &mpoTree->moSentinel)
                    {
                        mpoNode = mpoNode->mpoLeft;
                    }
                }

                void SeekToLast()
                {
                    mpoNode = mpoTree->mpoRoot;
                    while(mpoNode->mpoRight != &mpoTree->moSentinel && mpoNode!= &mpoTree->moSentinel)
                    {
                        mpoNode = mpoNode->mpoRight;
                    }
                }
            private:
                CRBTree* mpoTree;
                Node* mpoNode;
                // Intentionally copyable
        };

        Node* Delete(Iterator& aoIter)
        {
            struct Node* lpoNode;
        }
        bool Empty() {return mpoRoot == &moSentinel; }
        /*
           void Insert(Node* apoNode);
           void Delete(Node* apoNode);
           void MidTrival(Node* apoNode);
           void PreTrival(Node* apoNode);


        // Initialize an iterator over the specified list.
        // The returned iterator is not valid.
        explicit Iterator(CRBTree* list);

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
        */
    private:
        void LeftRotate(Node* apoNode)
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

        void RightRotate(Node* apoNode)
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


        void Insert(Node* apoTemp, Node* apoNode)
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

        Node* mpoRoot;
        Node moSentinel;
};



#endif
