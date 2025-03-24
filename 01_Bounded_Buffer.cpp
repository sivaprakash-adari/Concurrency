#include<iostream>
#include<thread>
#include<queue>
#include<chrono>
#include<mutex>

using namespace std;

class ProducerConsumer {
private:
    queue<int> _data_queue;
    mutex   _mtx;
    bool _done;
    condition_variable _cv;

public:
   ProducerConsumer() :_done(false){ }
   void producer() {

       for(int i=0; i<=10; i++) {
           unique_lock<mutex> lock(_mtx);
           cout << "Produced :" << i << endl;
           _data_queue.push(i);
           lock.unlock();
           _cv.notify_one();
           this_thread::sleep_for(chrono::milliseconds(100));
       } 
       unique_lock<mutex> lock(_mtx);
       _done = true;
       lock.unlock();
       _cv.notify_all();
   }

   void consumer() {
       while(true) {
           unique_lock<mutex> lock(_mtx);
           _cv.wait(lock,[this] {return !_data_queue.empty() || _done;});

           if (_data_queue.empty() && _done == true)
               break;

           while (!_data_queue.empty()) {
               cout << "Consumed :" << _data_queue.front() << endl;
               _data_queue.pop();
           }
       }
   }

};

int main() {
    ProducerConsumer pc;

    thread pThread(&ProducerConsumer::producer, &pc);
    thread cThread(&ProducerConsumer::consumer, &pc);

    pThread.join();
    cThread.join();
    return 0;
}