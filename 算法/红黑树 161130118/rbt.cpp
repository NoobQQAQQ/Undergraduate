#include<iostream>
#include<fstream>
#include<string>
#include<cassert>

using namespace std;

#define DEBUG

// define color in rbt
enum Color {red, black};

// define the structure of rbtNode
struct rbtNode
{
	uint32_t val;
	rbtNode *lc, *rc, *pa; // left child and right child and parent
	Color color; // 0 red 1 black;
	rbtNode(uint32_t x) { val = x; color = red; pa = lc = rc = NULL; } // the node to be inserted is always red
};

// define the structure of operation pair, like <operation, val>
struct Token
{
	char optr; // 'a' means insert, 'd' means delete
	uint32_t val;
};

// define RBT
class RBT
{
public:
	RBT() { root = NULL; }
	RBT(ifstream& fin, const char &c); // construct a rbt using ifstream, output tree after each operation to file
	~RBT() { destory(root); } // remove the whole rbt
	void insert(uint32_t x); // insert a node with val = x
	void remove(uint32_t x); // remove the node whose val = x
	rbtNode* search(uint32_t x); // return the pointer pointing to the node with val = x
	void output(const string &fileOut); // output the rbt into a file with in-order
private:
	rbtNode *root;

	// for debug
	bool checkColor(rbtNode *&root) const;
	bool checkOrder(rbtNode *&root);
	int checkHeight(rbtNode *&root) const;
	int curValue;

	void destory(rbtNode *&root); // for destructor
	string get_tree(rbtNode *&root); // get the tree with in-order format

	void do_optr(const Token &token); // add or delete on tree

	void insert_fix(rbtNode *&cur); // fix rbt after insertion
	void repairLeft(rbtNode *&cur); // fix left tree
	void repairRight(rbtNode *&cur); // fix right tree
	
	rbtNode* getNext(rbtNode *&px);
	void remove_fix(rbtNode *&pdc, rbtNode *&pdcf);
	void fixLeft(rbtNode *&pdc, bool &flag);
	void fixRight(rbtNode *&pdc, bool &flag);
};

void RBT::destory(rbtNode *&root)
{
	if (root != NULL)
	{
		destory(root->lc);
		destory(root->rc);
		delete root;
	}
}

//-----------------------Debug Part---------------------------
int RBT::checkHeight(rbtNode *&root) const
{
	int Lh, Rh;
	if (root->lc == NULL)
		Lh = 1;
	else
		Lh = checkHeight(root->lc);
	
	if (root->rc == NULL)
		Rh = 1;
	else
		Rh = checkHeight(root->rc);

	if (Lh == Rh)
		return Lh + ((root->color == black) ? 1 : 0);
	else
	{
		cout << "root: " << root->val << endl;
		cout << "Lh: " << Lh << " Rh: " << Rh << endl;
		assert(0);
	}
}

bool RBT::checkColor(rbtNode *&root) const
{
	Color leftColor;
	if (root->lc == NULL)
		leftColor = black;
	else
	{
		if (!checkColor(root->lc))
			return false;
		leftColor = root->lc->color;
	}

	Color rightColor;
	if (root->rc == NULL)
		rightColor = black;
	else
	{
		if (!checkColor(root->rc))
			return false;
		rightColor = root->rc->color;
	}

	if (root->color == black)
		return true;
	if (leftColor == red || rightColor == red)
		return false;
	return true;
}

bool RBT::checkOrder(rbtNode *&root) 
{
	if (root == NULL)
		return true;

	if (!checkOrder(root->lc))
		return false;

	if ((int) root->val < curValue)
		return false;
	curValue = root->val;

	if (!checkOrder(root->rc))
		return false;

	return true;
}
//------------------------------------------------------------

//--------------------------IO Part---------------------------
RBT::RBT(ifstream& fin, const char &c)
{
	root = NULL;
	int Optr_Num;
	fin >> Optr_Num; // may be useful when file.eof uesless?
	Token token;
	string fileOut = ".\\out\\rb";
	fileOut.push_back(c);
	fileOut.append(".out");
	while (!fin.eof())
	{
		fin >> token.optr >> token.val;
		do_optr(token);
#ifdef DEBUG
		if (!checkColor(root))
		{
			cout << "check color failed!\n";
			assert(0);
		}
		checkHeight(root);
		if (!checkOrder(root))
		{
			cout << "check order failed!\n";
			assert(0);
		}
#endif
		output(fileOut);
	}
}

void RBT::output(const string &fileOut)
{
	ofstream fout;
	fout.open(fileOut, ios::out | ios::app);
	string tree_in_order = get_tree(root);
	fout << tree_in_order << endl;
	fout.close();
}

string RBT::get_tree(rbtNode *&root)
{
	string res, left, right, cur;
	if (root != NULL)
	{
		left = get_tree(root->lc);

		string val, color;
		val = to_string(root->val);
		color = (root->color == red) ? "red" : "black";
		cur = ", (" + val + "," + color + "), ";
		 
		right = get_tree(root->rc);

		left.insert(0, "(");
		right.push_back(')');
		res = left + cur + right;
		return res;
	}
	else
		return "NIL";
}
//------------------------------------------------------------

//-----------------------Operation Part-----------------------
void RBT::do_optr(const Token &token)
{
#ifdef DEBUG
	curValue = -1; // for debug
#endif
	if (token.optr == 'a')
		insert(token.val);
	else if (token.optr == 'd')
		remove(token.val);
	else
	{
		cout << "an invalid operation " << token.optr << endl;
		assert(0);
	}
}

void RBT::insert(uint32_t x)
{
	if (root == NULL) // insert a node into an empty tree
	{
		root = new rbtNode(x);
		root->color = black;
	}
	else
	{
		rbtNode *cur = root;
		while (1) // insert a new node, adjust the tree 
		{
			if (cur->lc == NULL && cur->rc == NULL) // leaf node
			{
				if (x < cur->val)
				{
					cur->lc = new rbtNode(x);
					cur->lc->pa = cur;
					insert_fix(cur);
				}
				else if (x > cur->val)
				{
					cur->rc = new rbtNode(x);
					cur->rc->pa = cur;
					insert_fix(cur);
				}
				break;
			}
			else if (cur->lc == NULL && x < cur->val) // insert left
			{
				cur->lc = new rbtNode(x);
				cur->lc->pa = cur;
				insert_fix(cur);
				break;
			}
			else if (cur->rc == NULL && x > cur->val) // insert right
			{
				cur->rc = new rbtNode(x);
				cur->rc->pa = cur;
				insert_fix(cur);
				break;
			}
			else // not find insert position, keep going
			{
				if (x < cur->val)
					cur = cur->lc;
				else if (x > cur->val)
					cur = cur->rc;
			}
		}
	}
}

void RBT::insert_fix(rbtNode *&cur)
{
	while (cur != NULL && cur->color == red) // update cur in repairLeft or repairRight
	{
		if (cur->pa->lc == cur) // repair left
			repairLeft(cur);
		else // repair right
			repairRight(cur);
		if (root->color == red) root->color = black;
	}
}

void RBT::repairLeft(rbtNode *&cur)
{
	rbtNode *MP, *L, *M, *R, *LL, *LR, *RL, *RR;
	MP = cur->pa->pa;
	L = cur; M = cur->pa; R = cur->pa->rc;
	LL = LR = RL = RR = NULL;
	if (L != NULL) { LL = L->lc; LR = L->rc; }
	if (R != NULL) { RL = R->lc; RR = R->rc; }

	if (R != NULL && R->color == red) // colorFlip
	{
		M->color = red;
		R->color = L->color = black;
		cur = cur->pa->pa;
	}
	else if (LL != NULL && LL->color == red) // rrb
	{
		M->lc = LR; 
		if (LR != NULL) LR->pa = M;
		L->rc = M; M->pa = L;

		if (MP == NULL) { root = L; L->pa = NULL; }
		else if (MP->lc == M) { MP->lc = L; L->pa = MP; }
		else { MP->rc = L; L->pa = MP;  }

		L->color = black;
		LL->color = M->color = red;
	}
	else if (LR != NULL && LR->color == red) // brr
	{
		L->rc = LR->lc; 
		if (LR->lc != NULL) LR->lc->pa = L;
		M->lc = LR->rc; 
		if (LR->rc != NULL) LR->rc->pa = M;
		LR->lc = L;
		L->pa = LR;
		LR->rc = M; 
		M->pa = LR;

		if (MP == NULL) { root = LR; LR->pa = NULL; }
		else if (MP->lc == M) { MP->lc = LR; LR->pa = MP; }
		else { MP->rc = LR; LR->pa = MP; }

		LR->color = black;
		L->color = M->color = red;
	}
}

void RBT::repairRight(rbtNode *&cur)
{
	rbtNode *MP, *L, *M, *R, *LL, *LR, *RL, *RR;
	MP = cur->pa->pa;
	R = cur; M = cur->pa; L = cur->pa->lc;
	LL = LR = RL = RR = NULL;
	if (L != NULL) { LL = L->lc; LR = L->rc; }
	if (R != NULL) { RL = R->lc; RR = R->rc; }

	if (L != NULL && L->color == red) // color filp
	{
		M->color = red;
		R->color = L->color = black;
		cur = cur->pa->pa;
	}
	else if (RL != NULL && RL->color == red) // rrb
	{
		M->rc = RL->lc; 
		if (RL->lc != NULL) RL->lc->pa = M;
		R->lc = RL->rc; 
		if (RL->rc != NULL) RL->rc->pa = R;
		RL->lc = M; M->pa = RL;
		RL->rc = R; R->pa = RL;
		if (MP == NULL) { root = RL; RL->pa = NULL; }
		else if (MP->lc == M) { MP->lc = RL; RL->pa = MP; }
		else { MP->rc = RL; RL->pa = MP; }
		RL->color = black;
		M->color = R->color = red;
	}
	else if (RR != NULL && RR->color == red) // brr
	{
		M->rc = RL; 
		if (RL != NULL) RL->pa = M;
		R->lc = M; M->pa = R;
		if (MP == NULL) { root = R; R->pa = NULL; }
		else if (MP->lc == M) { MP->lc = R; R->pa = MP; }
		else { MP->rc = R; R->pa = MP; }
		R->color = black;
		M->color = RR->color = red;
	}
}


void RBT::remove(uint32_t x)
{
	rbtNode *px = search(x); // point to the node deleted logically
	rbtNode *pd = NULL; // point to the node deleted structurally
	if (px->lc == NULL || px->rc == NULL)
		pd = px;
	else // node x has both subtrees
		pd = getNext(px);

	rbtNode *pdcf = pd->pa; // pdcf == NULL iff pd is root node
	rbtNode *pdc; // point to the child of pd, either rc or lc or null
	if (pd->rc != NULL)
		pdc = pd->rc;
	else if (pd->lc != NULL)
		pdc = pd->lc;
	else
		pdc = NULL;
	if (pdc != NULL) pdc->pa = pd->pa; // set new parent

	// set new child
	if (pd->pa == NULL) // pd is root node
		root = pdc;
	else if (pd->pa->lc == pd)
		pd->pa->lc = pdc;
	else if (pd->pa->rc == pd)
		pd->pa->rc = pdc;


	// determine if need to exchange px and pd
	if (px != pd)
		px->val = pd->val;
	if (pd->color == black)
		remove_fix(pdc, pdcf);
	delete pd;
}

rbtNode* RBT::search(uint32_t x)
{
	rbtNode *cur = root;
	while (cur->val != x)
	{
		if (x < cur->val)
			cur = cur->lc;
		else
			cur = cur->rc;
	}
	return cur;
}

rbtNode* RBT::getNext(rbtNode *&px)
{
	rbtNode* cur = px->rc;
	while (cur->lc != NULL)
		cur = cur->lc;
	return cur;
}

void RBT::remove_fix(rbtNode *&pdc, rbtNode *&pdcf)
{
	rbtNode *Nil = NULL; bool flag = 1;
	if (pdc == NULL && pdcf == NULL) // delete lead to an empty tree
		return;
	if (pdc == NULL && pdcf != NULL) // pd is a leaf node, pdc is nil node, new a vitural nil node
	{
		if (pdcf->lc == NULL && pdcf->rc == NULL)
		{
			assert(pdcf->color == red);
			pdcf->color = black;
			return;
		}
		else  // new a vitural nil node
		{
			if (pdcf->lc == NULL)
			{
				Nil = new rbtNode(0);
				Nil->color = black;
				Nil->pa = pdcf;
				pdcf->lc = Nil;
			}
			else if (pdcf->rc == NULL)
			{
				Nil = new rbtNode(0);
				Nil->color = black;
				Nil->pa = pdcf;
				pdcf->rc = Nil;
			}
			pdc = Nil;
		}
	}
	if (pdc != NULL && pdcf == NULL) // pdc is root node
	{
		pdc->color = black;
		return;
	}
	while (pdc != root && pdc->color == black && flag)
	{
		flag = 0;
		if (pdc == pdc->pa->lc)
			fixLeft(pdc, flag);
		else
			fixRight(pdc, flag);
	}
	pdc->color = black;
	if (Nil != NULL) // delete the vitural nil node
	{
		if (Nil->pa->lc == Nil) Nil->pa->lc = NULL;
		else Nil->pa->rc = NULL;
		delete Nil;
	}
}

void RBT::fixLeft(rbtNode *&pdc, bool &flag)
{
	rbtNode *g, *p, *s, *l, *r;
	g = pdc; p = pdc->pa; s = p->rc; l = s->lc; r = s->rc;

	// case 5
	if (l != NULL && l->color == red)
	{
		rbtNode *ll, *lr;
		ll = l->lc; lr = l->rc;

		l->color = p->color;
		p->color = black;

		p->rc = ll; if (ll != NULL) ll->pa = p;
		s->lc = lr; if (lr != NULL) lr->pa = s;

		l->lc = p; l->rc = s;
		s->pa = l; l->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = l;
			else
				p->pa->rc = l;
		}
		else
			root = l;
		p->pa = l;
		return;
	}

	// case 4
	if (p->color == red && s->color == black && l == NULL)
	{
		p->rc = l;
		s->lc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}
	else if (p->color == red && s->color == black && l->color == black)
	{
		p->rc = l; l->pa = p;
		s->lc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}

	// case 3
	if (r != NULL && p->color == black && s->color == black && r->color == red && l == NULL)
	{
		r->color = black;

		p->rc = l;
		s->lc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;

		p->pa = s;
		return;
	}
	else if (r != NULL && p->color == black && s->color == black && r->color == red && l->color == black)
	{
		r->color = black;

		p->rc = l; l->pa = p;
		s->lc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;

		p->pa = s;
		return;
	}

	// case 2
	if (s->color == red)
	{
		p->color = red; s->color = black;
		p->rc = s->lc; if (s->lc != NULL) s->lc->pa = p;
		s->lc = p; s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;

		p->pa = s;
		flag = 1;
		return;
	}

	// case 1
	if (l == NULL && r == NULL && s->color == black && p->color == black)
	{
		s->color = red; pdc = p; flag = 1; return;
	}
	else if (l->color == black && r->color == black && s->color == black && p->color == black)
	{
		s->color = red; pdc = p; flag = 1; return;
	}
}

void RBT::fixRight(rbtNode *&pdc, bool &flag)
{
	rbtNode *g, *p, *s, *l, *r;
	g = pdc; p = pdc->pa; s = p->lc; l = s->lc; r = s->rc;

	// case 5
	if (r != NULL && r->color == red)
	{
		rbtNode *rl, *rr;
		rl = r->lc; rr = r->rc;

		r->color = p->color;
		p->color = black;

		p->lc = rr; if (rr != NULL) rr->pa = p;
		s->rc = rl; if (rl != NULL) rl->pa = s;

		r->rc = p; r->lc = s;
		s->pa = r; r->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = r;
			else
				p->pa->rc = r;
		}
		else
			root = r;
		p->pa = r;
		return;
	}

	// case 4
	if (p->color == red && s->color == black && r == NULL)
	{
		p->lc = r;
		s->rc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{

			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}
	else if (p->color == red && s->color == black && r->color == black)
	{
		p->lc = r; r->pa = p;
		s->rc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{

			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}

	// case 3
	if (l != NULL && p->color == black && s->color == black && l->color == red && r == NULL)
	{
		l->color = black;

		p->lc = r;
		s->rc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{

			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}
	else if (l != NULL && p->color == black && s->color == black && l->color == red && r->color == black)
	{
		l->color = black;

		p->lc = r; r->pa = p;
		s->rc = p;
		s->pa = p->pa;
		if (p->pa != NULL)
		{

			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		return;
	}

	// case 2
	if (s->color == red)
	{
		p->color = red; s->color = black;
		p->lc = s->rc; if (s->rc != NULL) s->rc->pa = p;
		s->rc = p; s->pa = p->pa;
		if (p->pa != NULL)
		{
			if (p->pa->lc == p)
				p->pa->lc = s;
			else
				p->pa->rc = s;
		}
		else
			root = s;
		p->pa = s;
		flag = 1;
		return;
	}

	// case 1
	if (l == NULL && r == NULL && s->color == black && p->color == black)
	{
		s->color = red; pdc = p; flag = 1; return;
	}
	else if (l->color == black && r->color == black && s->color == black && p->color == black)
	{
		s->color = red; pdc = p; flag = 1; return;
	}

}
//------------------------------------------------------------

int main()
{
	ifstream fin;
	for (char c = '0'; c <= '9'; c++)
	{
		string fileName = ".\\in\\rb";
		fileName.push_back(c);
		fileName.append(".in");
		fin.open(fileName, ios::in);
		RBT rbt(fin, c);
		fin.close();
	}
	return 0;
}