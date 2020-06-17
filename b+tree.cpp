/*

NL ----> NON LEAF
L  ----> LEAF

*/


#include <iostream>
#include <vector>
#include<cmath>

using namespace std;
int Lflag=0;

//-----------------------------------------------------------CLASS DECLARATION-----------------------------------------------------------------

class Node
{
    vector<int> keys;

    int Lmin,Lmedian,Lmax;
    int NLmin,NLmedian,NLmax;

    vector<Node*> Ch;
    bool leaf; // Is true when node is leaf, else false

public:

    Node(int ti, int tl, bool leaf);
    void traverse(int tabs);

    //INSERTION
    void insert(int key, int* val, Node*& newEntry);
    void split(int* val, Node*& newEntry);
    Node* makeNewRoot(int val, Node* newEntry);

    //DELETION
	int Searchindex(int x);
	int LSearchindex(int x); //First to search in leaf

	void Delete(int x);
	void DeleteFromLeaf(int index);
	void DeleteFromNonLeaf(int index);

	int FindPred(int index);
	int FindSucc(int index);

	void ManageLUF(int index); //Manage Leaf UnderFlow
	void ManageNLUF(int index); //Manage Non-Leaf UnderFlow

	void borrowFromPrev(int index);
	void borrowFromNext(int index);

	void merge(int index);

	friend class BpTree;
};

//-------------------------------------

class BpTree
{
    Node* root;
    int t; //Order

public:

    BpTree(int ti, int tl);
    void insert(int key);
    void print();
	void Delete(int x);
};

//-----------------------------------------------------------CLASS DECLARATION-----------------------------------------------------------------

//-----------------------------------------------------------CLASS DEF-------------------------------------------------------------------------

Node::Node(int ti, int tl, bool leaf)
{
    this->NLmax = ti-1;
    this->NLmin = ceil((float)ti/2)-1;
    this->NLmedian = ceil((float)(NLmax+1)/2) - 1;

    this->Lmax = tl-1;
    this->Lmin = ceil((float)tl/2)-1;
    this->Lmedian = ceil((float)Lmax/2);

    this->leaf = leaf;
}

//-------------------------------------

void Node::traverse(int tabs)
{
    int i;
    string s;

    for (int j = 0; j < tabs; j++)
        s += '\t';

    for (i = 0; i < keys.size(); i++)
    {
  		if (leaf == false)
	        Ch[i]->traverse(tabs + 1);

      	cout << s << keys[i] << endl;
    }

    if (leaf == false) //Last child
        Ch[i]->traverse(tabs + 1);
}

//-------------------------------------

void Node::split(int* val, Node*& newEntry)
{
    newEntry = new Node(NLmax+1, Lmax+1, false);
    *val = this->keys[NLmedian]; //median becomes parent

    // Last half entries will go to new node
    for (int i = NLmedian+1; i < NLmax+1; i++)
        newEntry->keys.push_back(this->keys[i]);

    this->keys.resize(NLmedian);

    // Last half entries will go to new node
    for (int i = NLmedian+1; i <= NLmax+1; i++)
        newEntry->Ch.push_back(this->Ch[i]);

    this->Ch.resize(NLmedian + 1);
}

//-------------------------------------

void Node::insert(int new_key, int* val, Node*& newEntry)
{
    if (leaf == false) //NON-LEAF
    {
        int i = 0;

        while (i < keys.size() && new_key > keys[i])
            i++;

        Ch[i]->insert(new_key, val, newEntry); //Inserting in left child

        if (newEntry == NULL) //To end the recursion
            return;

        if (keys.size() < NLmax)
        {
        	//cout<<"\n"<<keys.size();
            keys.insert(keys.begin() + i, *val);
            Ch.insert(Ch.begin() + i + 1, newEntry);
            newEntry = NULL;
        }

        else //node is full >> split
        {
            keys.insert(keys.begin() + i, *val);
            Ch.insert(Ch.begin() + i + 1, newEntry);
            split(val, newEntry);
        }
    }

    else //LEAF
    {
		//cout<<"\n"<<Lmax<<"\t"<<Lmin<<"\t"<<Lmedian;
		//cout<<"\n"<<NLmax<<"\t"<<NLmin<<"\t"<<NLmedian;
        vector<int>::iterator it;

        it = lower_bound(keys.begin(), keys.end(), new_key);
        keys.insert(it, new_key);

        if (keys.size() == Lmax+1)
        {
            newEntry = new Node(NLmax+1, Lmax+1 , true);
            *val = this->keys[Lmedian];

            for (int i = Lmedian; i < Lmax+1; i++)
                newEntry->keys.push_back(this->keys[i]);

            this->keys.resize(Lmedian);
        }
    }
}

//-------------------------------------

Node* Node::makeNewRoot(int val, Node* newEntry)
{
    Node* root = new Node(NLmax+1,Lmax+1, false);

    root->keys.push_back(val);
    root->Ch.push_back(this);
    root->Ch.push_back(newEntry);

    return root;
}

//-------------------------------------

BpTree::BpTree(int ti, int tl)
{
    root = new Node(ti, tl, true);
}

//-------------------------------------

void BpTree::insert(int key)
{
    Node* newEntry = NULL;
    int val = 0;
    root->insert(key, &val, newEntry);

    if (newEntry != NULL)
        root = root->makeNewRoot(val, newEntry);
}

//-------------------------------------

void BpTree::print()
{
    root->traverse(0);
}

//-------------------------------------

int Node::LSearchindex(int x)
{
	if (Lflag==1)
	{
		return -1;
	}
	int index=0,temp=0;

	if(leaf)
	{
  		//cout<<keys.size()<<"\n";
  		// while (index<keys.size())
 		//{cout<<"\n>>>"<<keys[index]; index++;}

		while (index<keys.size() && keys[index] <= x)
    	{
			++index;
    	}
    	DeleteFromLeaf(index-1);
    	//cout<<Lflag;
	}

	else
	{
		while (temp<keys.size() && keys[temp] <= x)
    	{
			++temp;
	   	}
		index=Ch[temp]->LSearchindex(x);

		if (Ch[temp]->keys.size() < Lmin && Lflag==1 && index!=0)
		{
			ManageLUF(temp);
		}
	}

	return index;
}

//-------------------------------------

int Node::Searchindex(int x)
{
	int index=0;
  //cout<<keys.size()<<"\n";
  // while (index<keys.size())
  //{cout<<"\n>>>"<<keys[index]; index++;}

	while (index<keys.size() && keys[index] < x)
    {
		++index;
    }
	return index;
}

//-------------------------------------

void Node::Delete(int x)
{
	int index = Searchindex(x);
	//cout<<"ff"<<index;

	if (index < keys.size() && keys[index] == x)
	{
		//cout<<"eeeee";
		if (leaf)
			DeleteFromLeaf(index);
		else
		{
			//cout<<"eeee";
			DeleteFromNonLeaf(index);
		}
	}
	else
	{

		if (leaf && Lflag==1)
			return;

		if (leaf && Lflag==0)
		{
			cout << "The key "<< x <<" is does not exist in the tree\n";
			return;
		}

		// Flag indicates if the key is present in the sub-tree rooted with last child of this node
		bool flag = ( (index==keys.size())? true : false );

	    //cout<<"dd";
	    if(Ch[index]->leaf)
	    {
			if (Ch[index]->keys.size() < Lmin)
			{
				ManageLUF(index);
			}
		}
		else
		{
			if (Ch[index]->keys.size() < NLmin)
			{
				ManageNLUF(index);
			}
		}


		if (flag && index > keys.size())
			Ch[index-1]->Delete(x);
		else
		{
			Ch[index]->Delete(x);

			if(Ch[index]->leaf)
	    	{
				if (Ch[index]->keys.size() < Lmin)
					ManageLUF(index);
			}
			else
			{
				if (Ch[index]->keys.size() < NLmin)
					ManageNLUF(index);
			}
		}
	}
	return;
}

//-------------------------------------

void Node::DeleteFromLeaf (int index)
{
	//cout<<"eeeee"<<keys.size();
	//for (int i=0; i<keys.size(); ++i)
	//cout<<"\n"<<keys[i]<<"\n";
	if(index==-1)
		return;
	Lflag=1;
	for (int i=index+1; i<keys.size(); ++i)
		keys[i-1] = keys[i];

	int n=keys.size();
	keys.resize(n-1);	
	return;
}

//-------------------------------------

void Node::DeleteFromNonLeaf(int index)
{

	int x = keys[index];

	if(Ch[index+1]->leaf)
	{
	    if  (Ch[index+1]->keys.size() >= Lmin)
	    {
	        int successor = FindSucc(index);
	        keys[index] = successor;
	        //Ch[index+1]->Delete(successor);
    	}
    }

    else
    {
		if  (Ch[index+1]->keys.size() >= NLmin)
    	{
    	    int successor = FindSucc(index);
    	    keys[index] = successor;
    	    //Ch[index+1]->Delete(successor);
    	}
    }
    return;
}

//-------------------------------------

int Node::FindPred(int index)
{
	//Reach Leaf
	Node *cursor=Ch[index];
	while (!cursor->leaf)
		cursor = cursor->Ch[cursor->keys.size()];

	return cursor->keys[cursor->keys.size()-1];
}

//-------------------------------------

int Node::FindSucc(int index)
{
	//Reach Leaf
	Node *cursor = Ch[index+1];
	while (!cursor->leaf)
		cursor = cursor->Ch[0];

	return cursor->keys[0];
}

//-------------------------------------

void Node::ManageLUF(int index)
{

	if (index!=0 && Ch[index-1]->keys.size()>Lmin)
		borrowFromPrev(index);

	else if (index!=keys.size() && Ch[index+1]->keys.size()>Lmin)
		borrowFromNext(index);

	else
	{
		if (index != keys.size())
			merge(index);
		else
			merge(index-1);
	}
	return;
}

//-------------------------------------

void Node::ManageNLUF(int index)
{

	if (index!=0 && Ch[index-1]->keys.size()>NLmin)
		borrowFromPrev(index);

	else if (index!=keys.size() && Ch[index+1]->keys.size()>NLmin)
		borrowFromNext(index);

	else
	{
		if (index != keys.size())
			merge(index);
		else
			merge(index-1);
	}
	return;
}

//-------------------------------------

void Node::borrowFromPrev(int index)
{

	Node *child=Ch[index];
	Node *sibling=Ch[index-1];

  	int a=child->keys.size();
	int b=sibling->keys.size();
	child->keys.resize(a+1);


	for (int i=child->keys.size()-2; i>=0; --i)
		child->keys[i+1] = child->keys[i];

	if (!child->leaf)
	{
		for(int i=child->keys.size(); i>=0; --i)
			child->Ch[i+1] = child->Ch[i];
	}

	child->keys[0] = keys[index-1];

	if(!child->leaf)
		child->Ch[0] = sibling->Ch[sibling->keys.size()];

	keys[index-1] = sibling->keys[sibling->keys.size()-1];

	sibling->keys.resize(b-1);
	return;
}

//-------------------------------------

void Node::borrowFromNext(int index)
{

	Node *child=Ch[index];
	Node *sibling=Ch[index+1];

	int a=child->keys.size();
	int b=sibling->keys.size();
	child->keys.resize(a+1);

	child->keys[(child->keys.size())-1] = keys[index];

	if (!(child->leaf))
		child->Ch[(child->keys.size())] = sibling->Ch[0];

	for (int i=1; i<sibling->keys.size(); ++i)
		sibling->keys[i-1] = sibling->keys[i];

	keys[index] = sibling->keys[0];

	if (!sibling->leaf)
	{
		for(int i=1; i<=sibling->keys.size(); ++i)
			sibling->Ch[i-1] = sibling->Ch[i];
	}

	sibling->keys.resize(b-1);
	return;
}

//-------------------------------------

void Node::merge(int index)
{
	Node *child = Ch[index];
	Node *sibling = Ch[index+1];

 	int a=child->keys.size();
	int b=sibling->keys.size();
	


	if (!child->leaf)
	{
		child->keys.resize(a+b+1);
		child->keys[a] = keys[index];

		for (int i=0; i<sibling->keys.size(); ++i)
		child->keys[i+a+1] = sibling->keys[i];

		for(int i=0; i<=sibling->keys.size(); ++i)
			child->Ch[i+a+1] = sibling->Ch[i];
	}

	else
	{
		child->keys.resize(a+b);

		for (int i=0; i<sibling->keys.size(); ++i)
		child->keys[i+a] = sibling->keys[i];
	}

	for (int i=index+1; i<keys.size(); ++i)
		keys[i-1] = keys[i];

	for (int i=index+2; i<=keys.size(); ++i)
		Ch[i-1] = Ch[i];


	int n=keys.size();
	keys.resize(n-1);

	delete(sibling);
	return;
}

//-------------------------------------

void BpTree::Delete(int x)
{
	if (!root)
	{
		cout << "The Tree is Empty\n";
		return;
	}

	root->LSearchindex(x);
	//root->traverse(0);
	root->Delete(x);

	if (root->keys.size()==0)
	{
		Node *tmp = root;
		if (root->leaf)
			root = NULL;
		else
			root = root->Ch[0];

		delete tmp;
	}
	return;
}

//-----------------------------------------------------------CLASS DEF-------------------------------------------------------------------------

int main()
{

    int choice,NLOrder,LOrder,e,n,x;
    cout<<"\nEnter the Interior Tree Order(>2): ";
   	cin>>NLOrder;

    cout<<"\nEnter the Leaf Tree Order(>2): ";
   	cin>>LOrder;

    BpTree* T = new BpTree(NLOrder,LOrder);

   s: cout<<"\n1: INSERT(small tree)\t  2:INSERT first n Numbers(big tree)\t 3:DELETE\t 4:PRINT\t 5:DONE \n";
    cin>>choice;

    switch(choice)
    {
    	case 1:
    		cout<<"Enter the element:";
    		cin>>e;
    		T->insert(e);
    		goto s;

    	case 2:
    		cout<<"n: ";
    		cin>>n;

    		for(int i=1;i<=n;i++)
    			T->insert(i);
    		goto s;

    	case 3:
    		cout<<"Delete: ";
    		cin>>x;
    		Lflag=0;
    		T->Delete(x);
    		goto s;

    	case 4:
    		cout<<"\n\n";
    		T->print();
    		cout<<"\n";
    		goto s;

    	case 5:
    		break;

    	default:
    		cout<<"\nNo such case! Ending program.";
    		break;
    }

    return 0;
}
