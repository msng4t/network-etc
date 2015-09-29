#include "Queue.h"

class Tester: public CQueueNode
{
public:
  int miValue;
};


//int main(int args, char* argv[])
//{
//  CQueueNode* header = new CQueueNode;
//  Tester *node1 = new Tester;
//  Tester *node2 = new Tester;
//  Tester *node3 = new Tester;
//  Tester *node4 = new Tester;
//  node1->miValue = 1;
//  node2->miValue = 2;
//  node3->miValue = 3;
//  node4->miValue = 4;
//
//  header->Init();
//  printf("empty:%d\n", header->Empty());
//  header->InsertAM(node1);
//  header->InsertBM(node2);
//  node2->InsertAM(node3);
//  header->InsertBM(node4);
//
//  CQueueNode *lpoNode = header->Next();
//  Tester* lpoTester;
//  while(lpoNode != header)
//  {
//    lpoTester = lpoNode->Data<Tester>();
//    printf("value:%d\n", lpoTester->miValue);
//    lpoNode = lpoNode->Next();
//  }
//
//}
//
