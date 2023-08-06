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
namespace test_3
{
    class V3
    {
    public:

    };
}
namespace test_2
{
class Door
{
private:
    unsigned     uuid;
    unsigned int color;
    bool         rotFlag;

public:
    std::string name;
    Door() :
        uuid(0)
        , class_type("Door")
        ,name("door") {}
    int getColor()
    {
        return color;
    }
    virtual operator std::string() const
    {
        std::string str = "[" + class_type + "](" + name + ")";
        return str;
    }

protected:
    std::string class_type;
    void setColor(unsigned int c)
    {
        color = c;
    }
};
std::ostream& operator<<(std::ostream& os, Door q)
{
    os << std::string(q);
    return os;
}
class FlexDoor : private Door
{
protected:
    using Door::class_type;
public:
    // 如下这样写了之后，FlexDoor的子类才能正常访问
    using Door::name;
    using Door::getColor;
    //using Door::setColor;
    // 如下这样写了之后，FlexDoor的外部对于FlexDoor的属性访问才能正常访问
    using Door::operator std::string;
    FlexDoor() :
        Door()
    {
        class_type = "FlexDoor";
        name = "flexDoor";
    }
    void initFlexDoor()
    {
    }

protected:
    void flexAct()
    {
        auto c = getColor();
        //rotFlag
    }
};
std::ostream& operator<<(std::ostream& os, FlexDoor q)
{
    os << std::string(q);
    return os;
}
class CarDoor : public FlexDoor
{
public:
    CarDoor()
    {

        class_type = "CarDoor";
        name       = "carDoor";
    }
    void initCarDoor()
    {
        initFlexDoor();
        flexAct();
        auto c = getColor();
        //setColor(1);
    }
};
void testMain()
{
    using std::string;
    Door d01;
    FlexDoor fd01;
    CarDoor car_d01;
    //fd01.getColor();
    std::cout << string(d01) << std::endl;
    std::cout << string(fd01) << std::endl;

    //std::cout << "ostream cout: " << &d01 << std::endl;
    std::cout << fd01 << std::endl;
    std::cout << car_d01 << std::endl;
}
} // namespace test_2
namespace test_1
{
class Door
{
    unsigned int color;

public:
    std::string name;
    Door() {}
    int getColor()
    {
        return color;
    }

protected:
    void setColor(unsigned int c)
    {
        color = c;
    }
};
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
} // namespace test_1
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoClass::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //test_1::testMain();
    test_2::testMain();
    std::cout << "base::demoClass::testMain() end.\n";
}
} // namespace demoClass
} // namespace base