#include "RBTree.h"
#ifndef __SKIPLISTTEST_H_INCLUDE__
#define __SKIPLISTTEST_H_INCLUDE__
int SkipListTest()
{
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
    for (int re = 0; re < 1; re++)
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
            printf("Delete[%d]: %s\n", i + 20, sl.Delete(i + 20)?"SUCCESS":"NOT FOUND");
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
#endif
