#include <iostream>          
#include <queue>
#include <thread>             
#include <mutex>              
#include <condition_variable> 

using namespace std; 

mutex mtx;
condition_variable produce, consume;

queue<int> q;
unsigned maxSize = 3;

void consumer() {
	while (true) {
		this_thread::sleep_for(chrono::milliseconds(1000));

		unique_lock<mutex> lck(mtx);
		consume.wait(lck, [] {return q.size() != 0;});

		cout << "consumer " << this_thread::get_id() << ": ";

		q.pop();
		cout << q.size() << '\n';

		produce.notify_all();
	}
}
  
void producer(int id) {
       	while (true) { 
		this_thread::sleep_for(chrono::milliseconds(900));

		unique_lock<mutex> lck(mtx);
		produce.wait(lck, [] {return q.size() != maxSize;});

		cout << "-> producer " << this_thread::get_id() << ": ";

		q.push(id);

		cout << q.size() << '\n';

		consume.notify_all();
	}
} 
 
int main() {
	thread consumers[2], producers[2];
	for (int i = 0; i < 2; ++i) {
	        consumers[i] = thread(consumer);
		producers[i] = thread(producer, i + 1);
	} 

	for (int i = 0; i < 2; ++i) { 
		producers[i].join();
		consumers[i].join();
	} 

	return 0;
}

