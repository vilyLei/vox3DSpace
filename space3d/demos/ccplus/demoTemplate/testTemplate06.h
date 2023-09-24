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
		// 模板的声明
		template<class T>
		struct container
		{
			container(T t) {}

			template<class Iter>
			container(Iter beg, Iter end) {}
		};

		// 额外的推导指引
		template<class Iter>
		container(Iter b, Iter e)->container<typename std::iterator_traits<Iter>::value_type>;

		//// 使用
		container c(7); // OK：用隐式生成的指引推导出 T=int
		std::vector<double> v = {/* ... */ };
		auto d = container(v.begin(), v.end()); // OK：推导出 T=double
		//container e{ 5, 6 }; // 错误：std::iterator_traits<int>::value_type 不存在
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
		
		// 聚合推导候选：
		//   template<class T>
		//   A<T> F(T, long, long);

		template<class... Args>
		struct B : std::tuple<Args...>, Args... {};
		B b{ std::tuple<std::any, std::string>{}, std::any{} };
		// 聚合推导候选：
		//   template<class... Args> 
		//   B<Args...> F(std::tuple<Args...>, Args...);

		// 推导出 b 的类型是 B<std::any, std::string>

		
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