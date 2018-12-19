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

		LinkedList(Node* first, int n = 3): curNum(0),cap(n) {
		       	head =  new Node();
			head->next = first;
			head->data = -1;
		}

		~LinkedList() {
			Node* p = nullptr;
			do {
				p = head;
				head = head->next;
				delete p;
			} while(head); 
		}

		void AddNode(int data) {
			Node * p = new Node;
			p->data = data;
			p->next = head->next;
			head->next = p;
		}

		int FindMediumElem() {
			Node * fast = head->next;
			Node * slow = head->next;

			while(fast) {
				fast = fast->next;
				if (fast) {
				       	fast = fast->next;
					slow = slow->next;
				}
			}

			if(slow) return slow->data;
			else return -1;
		}

		void DeleteNthByInvertedSeq(unsigned int Nth) {
			Node * slow = head;
			Node * fast = head;

			for(unsigned int i = 0; i < Nth && fast; i++) {
				fast = fast->next;
			}

			if(!fast || fast == head) { // fast == head, return when Nth = 0
				cout << "the number is not validated, " << curNum << " elements in list for now." << endl;
				return ;
			}

			while(fast->next) {
				fast = fast->next;
				slow = slow->next;
			}

			fast = slow->next;
			slow->next = slow->next->next;
			delete fast;
			--curNum;
			
			ShowAll();
		}

		Node * Merge(const LinkedList& other) {
			return MergeRecursive(this->head->next, other.head->next);	
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
		Node* MergeRecursive(Node* head1, Node* head2) {
			if(head1 == nullptr) return head2;
			else if(head2 == nullptr) return head1;

			Node* mergedHead = nullptr;
			if(head1->data > head2->data) {
				mergedHead = head1;
				mergedHead->next = MergeRecursive(head1->next, head2);
			} else {
				mergedHead = head2;
				mergedHead->next = MergeRecursive(head1, head2->next);
			}
			return mergedHead;
		}

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
	LinkedList l1(7);
	l1.AddNode(3);
	l1.AddNode(13);
	l1.AddNode(20);
	l1.AddNode(33);
	l1.AddNode(33);
	l1.AddNode(53);
	l1.AddNode(83);
	LinkedList l2(5);
	l2.AddNode(1);
	l2.AddNode(5);
	l2.AddNode(23);
	l2.AddNode(83);
	l2.AddNode(99);
	LinkedList ll(9);
	int data;
	char cdo = 'a';
	while(true) {
		cout << "select action:[a|r|d|f|m|q]:  ";
		cin >> cdo;
		if('a' == cdo) {
			cout << "input elem with integer value: ";
			cin >> data;
			ll.Get(data);
		} else if ('r' == cdo) {
			cout << "reverse this link list as result:" << endl;
			ll.Reverse();
			ll.ShowAll();
		} else if('d' == cdo) {
			cout << "input the Nth by inverted sequence: ";
			cin >> data;
	       		ll.DeleteNthByInvertedSeq(data);
		} else if('f' == cdo) {
			cout << "the data of medium element is: " << ll.FindMediumElem() << endl;
		} else if ('m' == cdo) {
			Node* first = l1.Merge(l2);
			LinkedList mergedLink(first);
			mergedLink.ShowAll();
			l1.ShowAll();
			l2.ShowAll();
		} else if ('q' == cdo) {
			cout << "PROGRAM EXIT." << endl;
		       	break; 
		}
	}
	return 0;}
