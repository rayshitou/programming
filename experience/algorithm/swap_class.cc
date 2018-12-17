#include <iostream>
#include <algorithm>
#include <cstddef>
#include <cstring>

using namespace std;

class mystring;
void swap(mystring& lobj, mystring& robj);

class mystring {
	public:
		~mystring() { delete[] data; data = NULL;}

		mystring(size_t size = 0):iSize(size),data(size ? new char[size] : nullptr) {}
		
		mystring(const char* s)
			:iSize(strlen(s)+1),
			data(iSize ? new char[iSize] : nullptr)
       		{
			memcpy(data, s, iSize);
		}

		mystring(const mystring& other) {
			iSize = other.iSize;
			data = new char[iSize];
			memcpy(data, other.data, iSize);
			cout << "copy constructor is called" << endl;
		}

		mystring& operator=(mystring other) {
			swap(*this, other);
			cout << "assignment operator is called" << endl;
		       	return *this;
		}

		mystring(mystring && other) {
			data = other.data;
			iSize = other.iSize;
			other.data = nullptr;
			cout << "move constructor is called" << endl;
		}

		friend mystring operator+(const mystring& lobj, const mystring& robj) {
			size_t new_size = lobj.iSize + robj.iSize - 1;
			char *new_data = new char[new_size];
			memcpy(new_data, lobj.data, lobj.iSize);
			memcpy(new_data + lobj.iSize - 1, robj.data, robj.iSize);
			return mystring(new_data);
		}

		friend void swap(mystring& lobj, mystring& robj) {
			using std::swap;
			swap(lobj.data, robj.data);
			swap(lobj.iSize, robj.iSize);
		}

		friend ostream& operator<<(ostream& out, const mystring& mystr) {
			out << mystr.data << ", size: " << mystr.iSize;
			return out;
		}
	private:
		size_t iSize;
		char* data;
};


int main(int argc, char**argv) {
	mystring mys("hello");
	mystring mys2(mys);
	mystring mys3 = mystring();
	mystring mys5(mys + mys3);
	cout << mys5 << endl;
	mys5 = mys;
	cout << mys << endl;
	cout << mys2 << endl;
	cout << mys3 << endl;
	cout << mys5 << endl;
	return 0;
}
