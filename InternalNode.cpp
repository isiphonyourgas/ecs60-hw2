#include <iostream>
#include <climits>
#include "InternalNode.h"

using namespace std;

InternalNode::InternalNode(int ISize, int LSize,
  InternalNode *p, BTreeNode *left, BTreeNode *right) :
  BTreeNode(LSize, p, left, right), internalSize(ISize)
{
  keys = new int[internalSize]; // keys[i] is the minimum of children[i]
  children = new BTreeNode* [ISize];
} // InternalNode::InternalNode()

BTreeNode* InternalNode::addPtr(BTreeNode *ptr, int pos)
{ // called when original was full, pos is where the node belongs.
  if(pos == internalSize)
    return ptr;

  BTreeNode *last = children[count - 1];

  for(int i = count - 2; i >= pos; i--)
  {
    children[i + 1] = children[i];
    keys[i + 1] = keys[i];
  } // shift things to right to make room for ptr, i can be -1!

  children[pos] = ptr;  // i will end up being the position that it is inserted
  keys[pos] = ptr->getMinimum();
  ptr->setParent(this);
  return last;
} // InternalNode:: addPtr()


void InternalNode::addToLeft(BTreeNode *last)
{
  ((InternalNode*)leftSibling)->insert(children[0]);

  for(int i = 0; i < count - 1; i++)
  {
    children[i] = children[i + 1];
    keys[i] = keys[i + 1];
  }

  children[count - 1] = last;
  keys[count - 1] = last->getMinimum();
  last->setParent(this);
  if(parent)
    parent->resetMinimum(this);
} // InternalNode::ToLeft()

void InternalNode::addToRight(BTreeNode *ptr, BTreeNode *last)
{
  ((InternalNode*) rightSibling)->insert(last);
  if(ptr == children[0] && parent)
    parent->resetMinimum(this);
} // InternalNode::addToRight()



void InternalNode::addToThis(BTreeNode *ptr, int pos)
{  // pos is where the ptr should go, guaranteed count < internalSize at start
  int i;

  for(i = count - 1; i >= pos; i--)
  {
      children[i + 1] = children[i];
      keys[i + 1] = keys[i];
  } // shift to the right to make room at pos

  children[pos] = ptr;
  keys[pos] = ptr->getMinimum();
  count++;
  ptr->setParent(this);

  if(pos == 0 && parent)
    parent->resetMinimum(this);
} // InternalNode::addToThis()


int InternalNode::getMaximum() const
{
  if(count > 0) // should always be the case
    return children[count - 1]->getMaximum();
  else
    return INT_MAX;
}  // getMaximum();


int InternalNode::getMinimum()const
{
  if(count > 0)   // should always be the case
    return children[0]->getMinimum();
  else
    return 0;
} // InternalNode::getMinimum()


InternalNode* InternalNode::insert(int value)
{  // count must always be >= 2 for an internal node
  int pos; // will be where value belongs

  for(pos = count - 1; pos > 0 && keys[pos] > value; pos--);

  BTreeNode *last, *ptr = children[pos]->insert(value);
  if(!ptr)  // child had room.
    return NULL;

  if(count < internalSize)
  {
    addToThis(ptr, pos + 1);
    return NULL;
  } // if room for value

  last = addPtr(ptr, pos + 1);

  if(leftSibling && leftSibling->getCount() < internalSize)
  {
    addToLeft(last);
    return NULL;
  }
  else // left sibling full or non-existent
    if(rightSibling && rightSibling->getCount() < internalSize)
    {
      addToRight(ptr, last);
      return NULL;
    }
    else // both siblings full or non-existent
      return split(last);
} // InternalNode::insert()

void InternalNode::insert(BTreeNode *oldRoot, BTreeNode *node2)
{ // Node must be the root, and node1
  children[0] = oldRoot;
  children[1] = node2;
  keys[0] = oldRoot->getMinimum();
  keys[1] = node2->getMinimum();
  count = 2;
  children[0]->setLeftSibling(NULL);
  children[0]->setRightSibling(children[1]);
  children[1]->setLeftSibling(children[0]);
  children[1]->setRightSibling(NULL);
  oldRoot->setParent(this);
  node2->setParent(this);
} // InternalNode::insert()

void InternalNode::insert(BTreeNode *newNode)
{ // called by sibling so either at beginning or end
  int pos;

  if(newNode->getMinimum() <= keys[0]) // from left sibling
    pos = 0;
  else // from right sibling
    pos = count;

  addToThis(newNode, pos);
} // InternalNode::insert(BTreeNode *newNode)

void InternalNode::print(Queue <BTreeNode*> &queue)
{
  int i;

  cout << "Internal: ";
  for (i = 0; i < count; i++)
    cout << keys[i] << ' ';
  cout << endl;

  for(i = 0; i < count; i++)
    queue.enqueue(children[i]);

} // InternalNode::print()


BTreeNode* InternalNode::remove(int value)
{  
  int pos, i, j;
  //int transfer;
  int count2;
  InternalNode *sibling;
  for(pos = count - 1; pos > 0 && keys[pos] > value; pos--);
  
  BTreeNode *ptr = children[pos]->remove(value);

  if((pos + 1) < count)
  {
    keys[pos + 1] = children[pos + 1]->getMinimum();
  }
  keys[pos] = children[pos]->getMinimum();
 
  if(leftSibling != NULL)
  {
    count2 = leftSibling->getCount();
    sibling = static_cast<InternalNode*>(leftSibling);
    if(sibling->children[count2 - 1] == ptr)
      sibling->deleteLeft();
  } 


  if(rightSibling != NULL)
  {
    sibling = static_cast<InternalNode*>(rightSibling);
    if(sibling->children[0] == ptr)
      sibling->deleteRight();
  }

  if(children[pos] == ptr)
  {
cout << "deleteing\n";
    for(i = pos; i < count - 1; i++)
    {
      children[i] = children[i + 1];
      keys[i] = children[i]->getMinimum();
    }
    count--;
  }
  for(i = 0; i < count - 1; i++)
  {
    if(children[i] == ptr)
    {
    //  delete children[i];
      for(j = i; j < count - 1; j++)
      {
        children[j] = children[j + 1];
        keys[j] = children[j]->getMinimum();
      }
      count--;
      break;
    }
  }


  int balancer = 0;
  if((internalSize % 2) == 1)
    balancer = 1;

//cout<< "internal " <<count << endl << endl << (internalSize/2) + balancer << endl;

  if(count < ((internalSize / 2) + balancer))
  {
    BTreeNode* transfer;
    InternalNode *ptr;
    int siblingCount, i;
    int check = 0;
    if((count == 0) && (parent->getCount() == 1))
      return this;
   
    if(leftSibling != NULL)
    {
      if((siblingCount = leftSibling->getCount()) > (internalSize / 2) + balancer)
      {
        ptr = static_cast<InternalNode*>(leftSibling);
        transfer = ptr->borrowLeft();
        for(i = count - 1; i > -1; i--)
        {
          children[i - 1] = children[i];
          keys[i - 1] = children[i - 1]->getMinimum();
        }
        count--;
      } else {
        ptr = static_cast<InternalNode*>(leftSibling);
        if(ptr->getLeftSibling() != NULL)
        {
          this->setLeftSibling(ptr->getLeftSibling());
          leftSibling->setRightSibling(this);
        }
        ptr->setRightSibling(NULL);
        ptr->setRightSibling(NULL);
        for(i = 0; i < count; i++)
        {
          children[i + ptr->getCount()] = children[i];
          keys[i + ptr->getCount()] = children[i + ptr->getCount()]->getMinimum();
        }
        for(i = ptr->getCount() - 1; i > -1; i--)
        {
          children[i] = ptr->children[i];
          keys[i] = children[i]->getMinimum();
          count++;
        }
        return ptr;
      }
      check = 1;
    }//Left Sibling

      if((check == 0) && (rightSibling != NULL))
      {
        if(rightSibling->getCount() > ((internalSize / 2) + balancer))
        {
          ptr = static_cast<InternalNode*>(rightSibling);
          transfer = ptr->borrowRight();
          children[count] = transfer;
          keys[count] = children[count]->getMinimum();
          count++;
        } else {
          ptr = static_cast<InternalNode*>(rightSibling);
          if(ptr->getRightSibling() != NULL)
          {
            this->setRightSibling(ptr->getRightSibling());
            rightSibling->setLeftSibling(this);
          }

          ptr->setLeftSibling(NULL);
          ptr->setRightSibling(NULL);
          for(i = 0; i < ptr->getCount(); i++)
          {
            children[count] = ptr->children[i];
            keys[count] = children[count]->getMinimum();
            count++;
          }//for
        return ptr;
       }//else
     }//right sibling
  }
// to be written by students
  return NULL; // filler for stub
} // InternalNode::remove(int value)


void InternalNode::resetMinimum(const BTreeNode* child)
{
  for(int i = 0; i < count; i++)
    if(children[i] == child)
    {
      keys[i] = children[i]->getMinimum();
      if(i == 0 && parent)
        parent->resetMinimum(this);
      break;
    }
} // InternalNode::resetMinimum()


InternalNode* InternalNode::split(BTreeNode *last)
{
  InternalNode *newptr = new InternalNode(internalSize, leafSize,
    parent, this, rightSibling);


  if(rightSibling)
    rightSibling->setLeftSibling(newptr);

  rightSibling = newptr;

  for(int i = (internalSize + 1) / 2; i < internalSize; i++)
  {
    newptr->children[newptr->count] = children[i];
    newptr->keys[newptr->count++] = keys[i];
    children[i]->setParent(newptr);
  }

  newptr->children[newptr->count] = last;
  newptr->keys[newptr->count++] = last->getMinimum();
  last->setParent(newptr);
  count = (internalSize + 1) / 2;
  return newptr;
} // split()

void InternalNode::deleteLeft()
{
  count--;
  delete children[count];
}


void InternalNode::deleteRight()
{
  int i;
  count--;
  delete children[0];
  for(i = 0; i < count; i++)
  {
    children[i] = children[i + 1];
    this->resetMinimum(children[i]);
  }
}

BTreeNode* InternalNode::borrowRight()
{
  BTreeNode *val = children[0];
  int i;
  count--;
  for(i = 0; i < count; i++)
  {
    children[i] = children[i + 1];
    keys[i] = children[i]->getMinimum();
  }
  return val;
}

BTreeNode* InternalNode::borrowLeft()
{
  BTreeNode *val;
  val = children[count - 1];
  count--;
  return val;
}
