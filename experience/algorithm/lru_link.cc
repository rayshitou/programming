#include <iostream>

using namespace std;

typedef struct _node {
	int data;
	struct _node * next;
}Node;

/*
 * Functions:
 * 	1. LRU access element
 * 	2. Reverse link list
 * 	3. Merge two ordered list to one
 * 	4. Find the medium node in the list
 * 	5. Detect there is circle in the list or not
 * 	6. Delete the Nth node with inverted sequence of list
*/
class LinkedList {
	public:
		// no data stored in the <head> node
		LinkedList(int n = 3): curNum(0),cap(n) {
		       	head =  new Node();
			head->next = nullptr;
			head->data = -1;
		}

		~LinkedList() {
			Node * p = head;
			Node * cur = nullptr;
			do {
				cur = p->next;
				delete p;
				p = cur;
			} while(p); 
		}

		void AddNode(int data) {
			Node * p = new Node;
			p->data = data;
			p->next = head->next;
			head->next = p;
		}

		bool Get(int data) {
			Node * p = head;
			Node * pre = nullptr;
			while(p->next) {
				pre = p;
				p = p->next;
				if( p->data == data) {
					LRU(pre, p);
					return true;
				}
			}

			if(curNum == cap) {
				DeleteTailElem();
			} else {
				++curNum;
			}

			AddNode(data);

			ShowAll();
			ShowHead();
			return false;
		}
	
		void ShowHead() {
			cout << "head node: " << head->next->data << endl;
		}

		void ShowAll() {
			cout << "num of elems in linked: " << curNum << endl;
			Node * p = head->next;
			while(p) {
				cout << p->data << " ";
				p = p->next;
			}
			cout<<endl;
		}

		void Reverse() {
			Node* pre = nullptr;
			Node* cur = head->next;
			Node* next = nullptr;

			while(cur) {
				next = cur->next;
				cur->next = pre;
				pre = cur;
				cur = next;
			}

			head->next = pre;
		}
	private:
		void LRU(Node* pre, Node* cur) {
			pre->next = cur->next;
			cur->next = head->next;
			head->next = cur;
		}

		void DeleteTailElem() {
			Node * p = head;
			Node * pre = head;
			while(p->next) {
				pre = p;
				p = p->next;
			}
			delete p;
			pre->next = nullptr;
		}
	
	private:
		int curNum;
		int cap;
		Node * head;
};

int main(int argc, char**argv) {
	LinkedList ll(3);
	int data;
	char cdo = 'a';
	while(true) {
		cout << "select action:[a|r|q]:  ";
		cin >> cdo;
		if('a' == cdo) {
			cout << "input elem with integer value: ";
			cin >> data;
			ll.Get(data);
		} else if ('r' == cdo) {
			cout << "reverse this link list as result:" << endl;
			ll.Reverse();
			ll.ShowAll();
		} else if ('q' == cdo) {
			cout << "PROGRAM EXIT." << endl;
		       	break; 
		}
	}
	return 0;}
