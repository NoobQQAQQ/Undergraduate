#include<iostream>
#include<string>
#include<sstream>
using namespace std;

template <class T>
struct Node
{
	T val;
	Node *next;
	Node(T x = 0) { val = x; next = NULL; }
};

template <class T>
class OneWayList
{
public:
	OneWayList();
	OneWayList(const string &s);
	OneWayList(T arr[], int n);
	~OneWayList();
	int Insert(Node<T> *itr, T val); // insert a node after the node pointed by itr
	int Remove(Node<T> *itr); // remove the node after the node pointed by itr
	int Remove(T val); // remove nodes which value = val
	int push_front(T val);
	int push_back(T val);
	T pop_front();
	T pop_back();
	int size() const;
	Node<T>* begin() { return head->next; }
	Node<T>* end() { return tail; }
	friend ostream& operator << (ostream &out, const OneWayList<T> &L)
	{
		Node<T> *cur = L.head->next;
		while (cur != NULL)
		{
			out << cur->val << ' ';
			cur = cur->next;
		}
		out << endl;
		return out;
	}

	friend istream& operator >> (istream &in, OneWayList<T> &L)
	{
		stringstream ss;
		string line;
		getline(in, line);
		ss << line;
		while (!ss.eof())
		{
			T val;
			ss >> val;
			L.push_back(val);
		}
		return in;
	}
private:
	Node<T> *head;
	Node<T> *tail;
	int NodeNum;
};

template <class T>
OneWayList<T>::OneWayList()
{
	NodeNum = 0;
	Node<T> *p = new Node<T>();
	head = tail = p;
}

template <class T>
OneWayList<T>::OneWayList(const string &s)
{
	stringstream ss;
	ss << s;
	while (!ss.eof())
	{
		T val;
		ss >> val;
		Node<T> *p = new Node<T>(val);
		tail->next = p;
		tail = p;
		NodeNum++;
	}
}

template <class T>
OneWayList<T>::OneWayList(T arr[], int n)
{
	NodeNum = n;
	for (int i = 0; i < n; i++)
	{
		Node<T> *p = new Node<T>(arr[i]);
		tail->next = p;
		tail = p;
	}
}

template <class T>
OneWayList<T>::~OneWayList()
{
	tail = NULL;
	while (head != NULL)
	{
		Node<T> *cur = head;
		head = head->next;
		delete cur;
	}
}

template <class T>
int OneWayList<T>::Insert(Node<T> *itr, T val) // insert a node after the node pointed by itr
{
	Node<T> *p = new Node(val);
	p->next = itr->next;
	itr->next = p;
	NodeNum++;
	return NodeNum;
}

template <class T>
int OneWayList<T>::Remove(Node<T> *itr) // remove the node after the node pointed by itr
{
	Node<T> *p = itr->next;
	itr->next = p->next;
	delete p;
	NodeNum--;
	return NodeNum;
}

template <class T>
int OneWayList<T>::Remove(T val) // remove nodes which value = val
{
	Node<T> *cur = head->next;
	Node<T> *pre = head;
	while (cur != NULL)
	{
		if (cur->val == val)
		{
			cur = cur->next;
			Remove(pre);
			NodeNum--;
		}
		else
		{
			pre = cur;
			cur = cur->next;
		}
	}
	return NodeNum;
}

template <class T>
int OneWayList<T>::push_front(T val)
{
	Node<T> *p = new Node<T>(val);
	p->next = head->next;
	head->next = p;
	NodeNum++;
	return NodeNum;
}

template <class T>
int OneWayList<T>::push_back(T val)
{
	Node<T> *p = new Node<T>(val);
	tail->next = p;
	tail = tail->next;
	NodeNum++;
	return NodeNum;
}

template <class T>
T OneWayList<T>::pop_front()
{
	Node<T> *p = head->next;
	head->next = p->next;
	T v = p->val;
	delete p;
	NodeNum--;
	return v;
}

template <class T>
T OneWayList<T>::pop_back()
{
	Node<T> *p = head->next;
	while (p->next != tail)
		p = p->next;
	T v = tail->val;
	tail = p;
	Remove(p);
	return v;
}

template <class T>
int OneWayList<T>::size() const
{
	return NodeNum;
}

int main()
{
	OneWayList<int> L;
	cin >> L;
	cout << L.size() << endl;
	cout << L;
	L.push_back(0);
	cout << L;
	L.pop_front();
	cout << L;
	L.push_front(0);
	cout << L;
	L.pop_back();
	cout << L;
	L.Remove(2);
	cout << L;
	L.Remove(L.begin());
	cout << L;
}