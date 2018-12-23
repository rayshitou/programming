#include <iostream>

using namespace std;

class Stack;
ostream& operator<<(ostream& os, const Stack& st);
			
class Stack{
	public:
		Stack(int size):len(size),top(0) {
			data = new int[size];	
		}
		~Stack() { delete []data; }

		bool empty() { return top == 0; }

		void push(int elem) {
			if(top != len) {
				data[top++] = elem;
			}
		}

		int pop() {
			if(!empty()) {
				return data[--top];
			}

			return -1;
		}

		friend ostream& operator<<(ostream& os, const Stack& st) {
			for(int i = 0; i < st.top; i++) {
				os << st.data[i] << " ";
			}
			return os;
		}

	private:
		int *data;
		int len;
		int top;
};

int main(int argc, char** argv) {
	Stack sk(3);
	sk.push(3);
	sk.push(6);
	sk.push(8);
	sk.push(9);
	cout<<sk<<endl;
	sk.pop();
	cout<<sk<<endl;
	sk.pop();
	cout<<sk<<endl;
	return 0;
}
