#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include "BTree.h"

using namespace std;

int main(int argc, char *argv[])
{
  bool first = true;
  BTree tree(atoi(argv[2]), atoi(argv[3]));
  int value;
  char s[80], command;

  ifstream inf(argv[1]);

  inf.getline(s, 80);

  while(inf >> command >> value)
  {
    if(command == 'i')
      tree.insert(value);
    else
    {
      if(first)
      {
        cout << "After all insertions.\n";
        tree.print();
        first = false;
      } // if first

      cout << "Deleting " << value <<  ". \n";
      tree.remove(value);
      tree.print();
     // fgets(s, 80, stdin);
    } // else deletion
  } // while

  return 0;
}  // main

