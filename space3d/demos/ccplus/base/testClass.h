#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <mutex>
#include <algorithm>

namespace base
{
namespace demoClass
{
namespace test_1
{
// 在构造函数和析构函数中调用虚函数是危险的, 也不会有多态的意义
class Car
{
public:
    Car(std::string name) :
        carName(std::move(name))
    {
        std::cout << carName + " constructor()... \n";
        init();
    }
    virtual ~Car()
    {
        std::cout << carName + " destructor()... \n";
        close();
    }
    virtual void init()
    {
        std::cout << carName + " init()... \n";
    }
    virtual void run()
    {
        std::cout << carName + " run()... \n";
    }
    void close()
    {
        std::cout << carName + " close()... \n";
    }

protected:
    std::string carName;
};
class SlowCar : public Car
{
public:
    SlowCar(std::string name) :
        Car(std::move(name))
    {
        std::cout << "\t[SlowCar] " << carName + " constructor()... \n";
        init();
    }
    virtual ~SlowCar()
    {
        std::cout << "\t[SlowCar] " << carName + " destructor()... \n";
        close();
        //Car::close();
    }
    virtual void init()
    {
        std::cout << "\t[SlowCar] " << carName + " init()... \n";
    }
    void run() override
    {
        std::cout << "\t[SlowCar] " << carName + " run()... \n";
    }
    void close()
    {
        std::cout << "\t[SlowCar] " << carName + " close()... \n";
    }
};
void testMain()
{
    //Car car01("car_01");
    //car01.run();
    
    //SlowCar car02("car_02");
    //car02.run();

    //SlowCar* slowCar = new SlowCar("car_02");
    //slowCar->run();
    //delete slowCar;

    Car* tempCar = new SlowCar("car_02");
    tempCar->run();
    delete tempCar;
}
}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoClass::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    test_1::testMain();
    std::cout << "base::demoClass::testMain() end.\n";
}
} // namespace demoConstexpr
} // namespace base