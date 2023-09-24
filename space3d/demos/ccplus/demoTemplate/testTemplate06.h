#include <iostream>
#include <thread>

namespace demoTemplate::base06
{
	namespace templateDemo_b03
	{
		template<typename... Args>
		void FormatPrint(Args... args)
		{
			(std::cout << ... << (std::cout << "[" << args, "]")) << std::endl;
		}

		void testMain(void)
		{
			FormatPrint(1, 2, 3, 4);
			FormatPrint("good", 2, "hello", 4, 110);
		}
	}
	namespace templateDemo_b02
	{
		// ģ�������
		template<class T>
		struct container
		{
			container(T t) {}

			template<class Iter>
			container(Iter beg, Iter end) {}
		};

		// ������Ƶ�ָ��
		template<class Iter>
		container(Iter b, Iter e)->container<typename std::iterator_traits<Iter>::value_type>;

		//// ʹ��
		container c(7); // OK������ʽ���ɵ�ָ���Ƶ��� T=int
		std::vector<double> v = {/* ... */ };
		auto d = container(v.begin(), v.end()); // OK���Ƶ��� T=double
		//container e{ 5, 6 }; // ����std::iterator_traits<int>::value_type ������
	}
	namespace templateDemo_b01
	{
		template<class T>
		struct A
		{
			T t;

			struct
			{
				int a, b;
			} u;
		};
		//A<int> a{ 1,2,3 };		// ok
		//A<int> a{ 1, {2, 3} };	// ok

		//A a{ 1, 2, 3 };	// error
		
		// �ۺ��Ƶ���ѡ��
		//   template<class T>
		//   A<T> F(T, long, long);

		template<class... Args>
		struct B : std::tuple<Args...>, Args... {};
		B b{ std::tuple<std::any, std::string>{}, std::any{} };
		// �ۺ��Ƶ���ѡ��
		//   template<class... Args> 
		//   B<Args...> F(std::tuple<Args...>, Args...);

		// �Ƶ��� b �������� B<std::any, std::string>

		
		void testMain()
		{
			std::cout << "templateDemo_b01::testMain() begin.\n";

			std::cout << "templateDemo_b01::testMain() end.\n";
		}
	}
	void testMain()
	{
		std::boolalpha(std::cout);
		std::cout << "demoTemplate::base06::testMain() begin.\n";
		templateDemo_b01::testMain();
		std::cout << "demoTemplate::base06::testMain() end.\n";
	}
}