#include <iostream>

using namespace std;

class mycache {
	public:
		mycache(int clen = 10): capacity(clen), num(0), data(clen > 0 ? new int[clen]:nullptr) {
		}
		~mycache() {delete[] data; data = nullptr;}

		bool get(int elem) {
			for(int i = capacity - 1; i >= capacity - num; i--) {
				if(elem == data[i]) {
					int j = i;
					for(; j > capacity - num; j--)
						data[j] = data[j-1];
					data[capacity-num] = elem;
					show();
					return true;
				}
			}
			if(num == capacity) {
				for(int i = capacity - 1; i > 0; i--) {
					data[i] = data[i-1];
				}
				--num;
			} 
			data[capacity - (++num)] = elem;
			cout << "add new value to cache" << endl;
			show();
			return false;
		}
		void show() {
			cout << "current keys: " << endl;
			for(int i = capacity - 1; i >= capacity - num; i--) {
				cout << data[i] << " ";
			}
			cout << endl;
			cout << " the newest key: " << data[capacity - num] << endl;
			cout << " the oldest key: " << data[capacity - 1] << endl;
		}

	private:
		int capacity;
		int num;
		int *data;
};

int main(int argc, char** argv) {
	mycache mc;
	int key;
	while(true) {
		cout << "intup key: ";
		cin >> key;
		mc.get(key);
	}

	return 0;
}
