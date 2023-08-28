#include <iostream>
#include <thread>
namespace parallel::hazardPtr1
{
namespace hazardPtr1_01
{
template <typename T>
struct node_t
{
    T*         nodeValue;
    node_t<T>* next;

    node_t() :
        nodeValue(nullptr),
        next(nullptr)
    {
    }
};

struct hazardPoint_t
{
    std::atomic<std::thread::id> threadID;
    std::atomic<void*>           hazardStorePoint;
};

const int     MAX_HAZARD_POINT_NUMBER = 100;             //1
hazardPoint_t hazardPointArray[MAX_HAZARD_POINT_NUMBER]; //2

struct hpThreadOwn
{
    hazardPoint_t* thisThreadHp;
    hpThreadOwn() :
        thisThreadHp(nullptr)
    {
        std::thread::id defaultInitThreadID;
        for (int i = 0; i < MAX_HAZARD_POINT_NUMBER; i++)
        {
            if (hazardPointArray[i].threadID.compare_exchange_strong(defaultInitThreadID, std::this_thread::get_id())) //3
            {
                thisThreadHp = &hazardPointArray[i];
                break;
            }
        }
        if (thisThreadHp == nullptr)
        {
            std::cout << "error: cant init this thread hazard point" << std::endl;
        }
    }
    bool findConflictNode(void* catchNode)
    {
        for (int i = 0; i < MAX_HAZARD_POINT_NUMBER; i++)
        {
            if (hazardPointArray[i].hazardStorePoint.load() == catchNode) //4
            {
                return false;
            }
        }
        return true;
    }
    ~hpThreadOwn()
    {
        thisThreadHp->hazardStorePoint.store(nullptr); //5
        thisThreadHp->threadID.store(std::thread::id());
    }
};


template <typename T>
class lockFreeStack
{
public:
    T*   popNode();
    void pushNode(T* value);
    void deleteWaitQueue(hpThreadOwn* hazardPoint);
    void addDeleteQueue(node_t<T>* toDeleteNode);

private:
    std::atomic<node_t<T>*> storeStack;
    std::list<node_t<T>*>   toDeleteQueue;
};

template <typename T>
T* lockFreeStack<T>::popNode()
{
    static thread_local hpThreadOwn thisThreadHazardPos; //6
    node_t<T>*                      catchNode;
    T*                              catchValue;
    node_t<T>*                      tempNode;

    catchNode = storeStack.load();

    do
    {
        do
        {
            tempNode = catchNode;
            thisThreadHazardPos.thisThreadHp->hazardStorePoint.store((void*)catchNode); //7
            catchNode = storeStack.load();                                              //8
        } while (tempNode != catchNode);
    } while (catchNode != nullptr && !storeStack.compare_exchange_strong(catchNode, catchNode->next)); //9

    if (catchNode != nullptr)
    {
        catchValue           = catchNode->nodeValue;
        catchNode->nodeValue = nullptr;
        thisThreadHazardPos.thisThreadHp->hazardStorePoint.store(nullptr);
        if (thisThreadHazardPos.findConflictNode((void*)catchNode) == false) //10
        {
            addDeleteQueue(catchNode);
        }
        else
        {
            delete catchNode;
        }
    }
    deleteWaitQueue(&thisThreadHazardPos); //11
    return catchValue;
}

template <typename T>
void lockFreeStack<T>::deleteWaitQueue(hpThreadOwn* hazardPoint)
{
    node_t<T>* waitToDeleteHead = toDeleteQueue.exchange(nullptr); //12
    if (waitToDeleteHead != nullptr)
    {
        node_t<T>* tempDeleteNode = waitToDeleteHead;
        while (waitToDeleteHead)
        {
            if (hazardPoint->findConflictNode(waitToDeleteHead) == false) //13
            {
                addDeleteQueue(tempDeleteNode);
            }
            else
            {
                tempDeleteNode = waitToDeleteHead->next;
                delete waitToDeleteHead;
                waitToDeleteHead = tempDeleteNode;
            }
        }
    }
}

template <typename T>
void lockFreeStack<T>::addDeleteQueue(node_t<T>* toDeleteNode)
{
    node_t<T>* tempNode = toDeleteQueue.load();
    while (toDeleteQueue.compare_exchange_weak(tempNode->next, tempNode))
        ;
}
void testMain()
{
}
} // namespace hazardPtr1_01
void testMain() {
    hazardPtr1_01::testMain();
}
} // namespace parallel::hazardPtr1