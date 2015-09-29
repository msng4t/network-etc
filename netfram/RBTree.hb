#ifndef _MINA_RBTREE_H_INCLUDE
#define _MINA_RBTREE_H_INCLUDE

#include <inttypes.h>
#include "Define.h"
#include "Any.h"
MINA_NAMESPACE_START

typedef uint64_t UKey;
typedef int IKey;
typedef unsigned char uchar_t;

struct CRBTreeNode
{
    UKey muKey;
    CRBTreeNode* mpoLeft;
    CRBTreeNode* mpoRight;
    CRBTreeNode* mpoParent;
    uchar_t mcColor;
    CAny value;
};


typedef void (*InsertFunc)(CRBTreeNode* mpoRoot,
        CRBTreeNode* apoNode,
        CRBTreeNode* apoSentinel);

struct CRBTree
{
    CRBTreeNode* mpoRoot;
    CRBTreeNode* mpoSentinel;
    InsertFunc Insert;
};

#define MINA_RBTREE_INIT(tree, s, i) \
    MINA_RBTREE_SENTINEL_INIT(s);    \
    (tree)->mpoRoot = s;           \
    (tree)->mpoSentinel = s;       \
    (tree)->Insert = i;

void RBTreeInsert(volatile CRBTree* apoTree, CRBTreeNode* apoNode);
void RBTreeDelete(volatile CRBTree* apoTree, CRBTreeNode* apoNode);
void RBTreeInsertValue(CRBTreeNode* mpoRoot, CRBTreeNode* apoNode, CRBTreeNode* apoSentinel);

#define RBT_RED(apoNode) ((apoNode)->mcColor = 1);
#define RBT_BLACK(apoNode) ((apoNode)->mcColor = 0)
#define RBT_IS_RED(apoNode) ((apoNode)->mcColor)
#define RBT_IS_BLACK(apoNode) (!RBT_IS_RED(apoNode))
#define RBT_COPY_COLOR(node1, node2) ((node1)->mcColor = (node2)->mcColor)
#define MINA_RBTREE_SENTINEL_INIT(apoNode) RBT_BLACK(apoNode)

static inline CRBTreeNode* 
RBTreeMin(CRBTreeNode* apoNode, CRBTreeNode* apoSentinel)
{
    while(apoNode->mpoLeft != apoSentinel)
    {
        apoNode = apoNode->mpoLeft;
    }
    return apoNode;
}

void MinaMidTrival(CRBTreeNode* apoNode, CRBTreeNode* apoSentinel);
void MinaPreTrival(CRBTreeNode* apoNode, CRBTreeNode* apoSentinel);

MINA_NAMESPACE_END
#endif
