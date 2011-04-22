#include <iostream>
#include <climits>
#include "LeafNode.h"
#include "InternalNode.h"
#include "QueueAr.h"

using namespace std;


LeafNode::LeafNode(int LSize, InternalNode *p,
  BTreeNode *left, BTreeNode *right) : BTreeNode(LSize, p, left, right)
{
  values = new int[LSize];
//this->setRightSibling(NULL);
//this->setLeftSibling(NULL);
}  // LeafNode()

void LeafNode::addToLeft(int value, int last)
{
  leftSibling->insert(values[0]);

  for(int i = 0; i < count - 1; i++)
    values[i] = values[i + 1];

  values[count - 1] = last;
  if(parent)
    parent->resetMinimum(this);
} // LeafNode::ToLeft()

void LeafNode::addToRight(int value, int last)
{
  rightSibling->insert(last);

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToRight()

void LeafNode::addToThis(int value)
{
  int i;

  for(i = count - 1; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];

  values[i + 1] = value;
  count++;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
} // LeafNode::addToThis()


void LeafNode::addValue(int value, int &last)
{
  int i;

  if(value > values[count - 1])
    last = value;
  else
  {
    last = values[count - 1];

    for(i = count - 2; i >= 0 && values[i] > value; i--)
      values[i + 1] = values[i];
    // i may end up at -1
    values[i + 1] = value;
  }
} // LeafNode:: addValue()


int LeafNode::getMaximum()const
{
  if(count > 0)  // should always be the case
    return values[count - 1];
  else
    return INT_MAX;
} // getMaximum()


int LeafNode::getMinimum()const
{
  if(count > 0)  // should always be the case
    return values[0];
  else
    return 0;

} // LeafNode::getMinimum()


LeafNode* LeafNode::insert(int value)
{
  int last;

  if(count < leafSize)
  {
    addToThis(value);
    return NULL;
  } // if room for value

  addValue(value, last);

  if(leftSibling && leftSibling->getCount() < leafSize)
  {
    addToLeft(value, last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < leafSize)
    {
      addToRight(value, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(value, last);
}  // LeafNode::insert()

void LeafNode::print(Queue <BTreeNode*> &queue)
{
  cout << "Leaf: ";
  for (int i = 0; i < count; i++)
    cout << values[i] << ' ';
  cout << endl;
} // LeafNode::print()


LeafNode* LeafNode::remove(int value)
{
  int pos = 0;
  int q;
  for(pos = 0; pos < count; pos++)
  {
    if(values[pos] == value)
    {

      for(int i = pos; i < count; i++)
      {
        values[i] = values[i + 1];
      }
      count--;
      break;
    }
    if(values[pos] > value)
      break;
  }


  if(count < ((leafSize+1)/2))
  {
    int transfer;
    BTreeNode *ptr2;
    LeafNode *ptr;
    int check = 0;
    int siblingCount, i;
int count1;
      //Checks left sibling

 // if(count == 0)
 //   return this;

    if(leftSibling != NULL)
    {   
//cout << "left pass \n";
      if((siblingCount = leftSibling->getCount()) > ((leafSize+1)/ 2))
      {  //borrow from left      
//cout << "problem left 1\n";
        ptr = static_cast<LeafNode*>(leftSibling);
        transfer = ptr->borrowLeft();
        this->insert(transfer);
      } else {//merge with left
//cout << "problem left 2 \n";
        ptr = static_cast<LeafNode*>(leftSibling);
//cout << "ptr pass \n";
        if(ptr->getLeftSibling() != NULL)
        {
          this->setLeftSibling(ptr->getLeftSibling());//Set new Sibling
          leftSibling->setRightSibling(this);
        }
        ptr->setRightSibling(NULL);
        ptr->setLeftSibling(NULL);
        for(i = 0; i < count; i++)
        {
//cout << "seg1\n";
          values[i + ptr->getCount()] = values[i];

        }
        for(i = ptr->getCount() - 1; i > -1; i--)
        {
//cout << "seg2\n";
          values[i] = ptr->values[i];
          count++;
        }
        return ptr;
      }
      check = 1;

    }//Left Sibling  
   
    if((check == 0) && (rightSibling != NULL))
    {//borrow from right

      if(rightSibling->getCount() > ((leafSize+1)/ 2))
      {

        ptr = static_cast<LeafNode*>(rightSibling);//borrow
        transfer = ptr->borrowRight();
        values[count] = transfer;
        count++;
      }  else {

      


        ptr = static_cast<LeafNode*>(rightSibling);//merge with right
        if(ptr->getRightSibling() != NULL)
        {
          this->setRightSibling(ptr->getRightSibling());
          rightSibling->setLeftSibling(this);
        }
        ptr->setLeftSibling(NULL);
        ptr->setRightSibling(NULL);
        for(i = 0; i < ptr->getCount(); i++)
        {
          values[count] = ptr->values[i];
          count++;
        }

        return ptr;//returns merged
      }
      check = 1;
//cout << "right\n\n";
    }//Right Sibling   
//cout << "seg\n";
  }//If not in size reqs
//cout << "no action\n";
if(count == 0)
  return this;
  return NULL;  // filler for stub
}  // LeafNode::remove()



LeafNode* LeafNode::split(int value, int last)
{
  LeafNode *ptr = new LeafNode(leafSize, parent, this, rightSibling);


  if(rightSibling)
    rightSibling->setLeftSibling(ptr);

  rightSibling = ptr;

  for(int i = (leafSize + 1) / 2; i < leafSize; i++)
    ptr->values[ptr->count++] = values[i];

  ptr->values[ptr->count++] = last;
  count = (leafSize + 1) / 2;

  if(value == values[0] && parent)
    parent->resetMinimum(this);
  return ptr;
} // LeafNode::split()

int LeafNode::borrowRight()
{
  int val = values[0];
  int i;
  count--;
  for(i = 0; i < count; i++)
  {
    values[i] = values[i + 1];
  }
  return val;
}

int LeafNode::borrowLeft()
{
  int val;
  val = values[count - 1];
  count--;
  return val;
}
