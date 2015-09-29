#include "RBTree.h"
#ifndef __RBTREETEST_H_INCLUDE__
#define __RBTREETEST_H_INCLUDE__
int RBTreeTest()
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
//    sleep(10000);
    return 0;
}
#endif
