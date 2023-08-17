#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <cassert>
#include <list>
#include <string>
#include <mutex>
#include <algorithm>

namespace base
{
namespace demoClass
{
namespace test_3
{

template <typename T>
class V4
{
public:
    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };
        T vs[4];
    };
    T& operator[](int i)
    {
        return vs[i];
    }
};
void testMain()
{
    std::cout << "base::demoClass::test_3::testMain() ..." << std::endl;

    V4<float> v4_0{};
    v4_0.x = 10.0f;
    v4_0.y = 8.5f;
    v4_0[0] += 0.7f;
    v4_0[1] += 0.7f;

    std::cout << " v4_0.x:" << v4_0.x << std::endl;
    std::cout << " v4_0.y:" << v4_0.y << std::endl;
    std::cout << " v4_0.vs[0]:" << v4_0.vs[0] << std::endl;
    std::cout << " v4_0.vs[1]:" << v4_0.vs[1] << std::endl;
}
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
namespace std_optional
{
// optional 可用作可能失败的工厂的返回类型
std::optional<std::string> create(bool b)
{
    if (b)
        return "Godzilla";
    return {};
}

// 能用 std::nullopt 创建任何（空的） std::optional
auto create2(bool b)
{
    return b ? std::optional<std::string>{"Godzilla"} : std::nullopt;
}

void testMain()
{
    std::cout << "create(false) 返回 "
              << create(false).value_or("empty") << '\n';
    std::cout << "create(true) 返回 "
              << create(true).value() << '\n';

    // 返回 optional 的工厂函数可用作 while 和 if 的条件
    if (auto str = create2(true))
        std::cout << "create2(true) 返回 " << *str << '\n';
}

}
void testMain()
{
    std::boolalpha(std::cout);
    std::cout << "base::demoClass::testMain() begin.\n";
    //std::cout << std::atomic<int>::is_always_lock_free << "\n";
    //test_1::testMain();
    //test_2::testMain();
    std_optional::testMain();
    std::cout << "base::demoClass::testMain() end.\n";
}
} // namespace demoClass
} // namespace base