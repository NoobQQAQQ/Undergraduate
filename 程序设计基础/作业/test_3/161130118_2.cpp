#include<iostream>
using namespace std;

struct Node
{
	int content;
	Node *next;
	Node(int x = 0){ content = x; next = NULL; }
};

void Input(Node *&head)
{
	int x;
	Node *cur = head;
	while (cin >> x)
	{
		if (x != -1)
		{
			Node *p = new Node(x);
			cur->next = p;
			cur = cur->next;
		}
		else
			break;
	}
}

void RemoveSame(Node *&head)
{
	Node *cur = head->next;
	while (cur != NULL)
	{
		if (cur->next == NULL)
			break;
		else if (cur->next->content == cur->content)
		{
			Node *p = cur->next;
			cur->next = p->next;
			delete p;
		}
		else
			cur = cur->next;
	}
}

void OutPut(Node *&head)
{
	Node *cur = head->next;
	while (cur != NULL)
	{
		cout << cur->content << ' ';
		cur = cur->next;
	}
	cout << endl;
}

int main()
{
	Node *head = new Node; // 带附加头结点
	Input(head);
	RemoveSame(head);
	OutPut(head);

}