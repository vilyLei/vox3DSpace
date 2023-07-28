#include <iostream>
#include <chrono>
#include <atomic>
/*
//#include <tbb/atomic.h>
#include "tbb/tbb.h"

#include <pthread.h>
#include <vector>

using namespace std;
typedef std::atomic<int> ATOMINT;
ATOMINT                  v;

void* tf(void* args)

{

    for (int i = 0; i < 100000; i++)


    {

        v += 1;
    }

    return NULL;
}

int main()
{
    thread tid;
    vector<thread> vp;

    for (int i = 0; i < 100; i++)
    {
        pthread_create(&tid, NULL, tf, NULL);
        vp.push_back(tid);
    }
    for (int i = 0; i < 100; i++)
    {
        pthread_join(vp[i], NULL);
    }

    cout << v << endl;

    return 0;
}
//*/
namespace tbbAtomic
{
void testMain()
{

}
} // namespace tbbAtomic