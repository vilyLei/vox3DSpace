#include <iostream>
#include <string>
#include <tuple>

namespace demoTemplate::base04
{
	namespace templateDemo_b03
	{
		void testMain()
		{
			std::cout << "\ntemplateDemo_b02::testMain() begin.\n";
			std::tuple<int, double, std::string> tp3 = std::make_tuple(1, 2.5, "");
			std::cout << "templateDemo_b02::testMain() end.\n";
		}
	}
	namespace templateDemo_b02
	{
		template<class T>
		struct SCItem
		{
			std::vector<T> v;

			SCItem(std::initializer_list<T> l) : v(l)
			{
				std::cout << "constructed with a " << l.size() << "-element list\n";
			}

			void append(std::initializer_list<T> l)
			{
				v.insert(v.end(), l.begin(), l.end());
			}

			std::pair<const T*, std::size_t> c_arr() const
			{
				return { &v[0], v.size() }; // copy list-initialization in return statement
											// this is NOT a use of std::initializer_list
			}
		};
		template<typename T>
		void templated_fn(T) {}

		void testMain1()
		{
			std::cout << "templateDemo_b02::testMain1() begin.\n";
			SCItem<int> sci = { 1, 2, 3, 4, 5 }; // copy list-initialization
			sci.append({ 6, 7, 8 });        // list-initialization in function call

			std::cout << "The vector now has " << sci.c_arr().second << " ints:\n";

			for (auto n : sci.v)
				std::cout << n << ' ';
			std::cout << '\n';

			std::cout << "Range-for over brace-init-list: \n";

			for (int x : {-1, -2, -3}) // the rule for auto makes this ranged-for work
				std::cout << x << ' ';
			std::cout << '\n';

			auto al = { 10, 11, 12 }; // special rule for auto

			std::cout << "The list bound to auto has size() = " << al.size() << '\n';

			//  templated_fn({1, 2, 3}); // compiler error! "{1, 2, 3}" is not an expression,
										 // it has no type, and so T cannot be deduced
			templated_fn<std::initializer_list<int>>({ 1, 2, 3 }); // OK
			templated_fn<std::vector<int>>({ 1, 2, 3 });           // also OK
			std::cout << "templateDemo_b02::testMain1() end.\n";
		}
		template<class T>
		class SimpleContainer {
		public:
			SimpleContainer(std::initializer_list<T> args) {
				data = new T[args.size()];
				n = 0;
				for (T x : args) { data[n++] = x; }
			}
			T* begin() { return data; }
			T* end() { return data + n; }
			T* operator[](size_t i) {
				if (i >= n) {
					i = n - 1;
				}
				return data + i;
			}
		private:
			unsigned n;
			T* data;
		};

		class ClassA {
		public:
			ClassA(int a, int b) {
				std::cout << "ClassA::construct() 01, a: " << a << ", b: " << b << "\n";
			}
			ClassA(std::initializer_list<int> list) {

				auto a = *(list.begin() + 1);
				std::cout << "ClassA::construct() 02, list.size(): " << list.size() << "\n";
			}
		};
		void testMain()
		{
			std::cout << "templateDemo_b02::testMain() begin.\n";

			auto i_var = { 10,20, 30 };
			ClassA ca0(0, 1);
			ClassA ca1{ std::initializer_list<int>{0,1,2} };
			ClassA ca2{ 0,1,2,3,4,5 };
			std::cout << "--- --- ---\n";
			SimpleContainer sc01{ 0,1,2,3,4,5 };
			auto value_0 = *sc01[1];
			std::cout << "value_0: " << value_0 << "\n";

			SimpleContainer sc02{ "hello", "jey", "hi"};
			auto value_1 = *sc02[1];
			std::cout << "value_1: " << value_1 << "\n";
			std::cout << "--- --- ---\n";
			testMain1();
			std::cout << "templateDemo_b02::testMain() end.\n";
		}
	}
	namespace templateDemo_b01
	{

		class ClassTA {
		public:
			ClassTA(int a, int b) {
				std::cout << "ClassTA::construct() 01, a: " << a << ", b: " << b << "\n";
			}
			ClassTA(std::initializer_list<int> list) {

				auto a = *(list.begin() + 1);
				std::cout << "ClassTA::construct() 02, list.size(): " << list.size() << "\n";
			}
			/* definitions ... */
		};
		// recursive termination function
		//template <typename T>
		//void print01(T t)
		//{
		//	std::cout << t << std::endl;
		//}
		// recursive termination function
		void print01()
		{
			std::cout << "]" << std::endl;
		}
		// unpack function
		template <class T, class ...Args>
		void print01(T head, Args... args)
		{
			//std::cout << "parameter " << head << std::endl;
			if (sizeof...(args) > 0) {
				std::cout << head << ",";
			}
			else {
				std::cout << head;
			}
			print01(args...);
		}
		template <class ...Args>
		void print01A(Args... args)
		{
			std::cout << "[";
			print01(args...);
		}

		template <typename T>
		void printArg(T t)
		{
			std::cout << t << std::endl;
		}
		template <typename ...Args>
		void expandArgs(Args... args)
		{
			int arr[] = { (printArg(args), 0)... };
		}
		////////////////////////////////////////////////////////
		//
		template <typename T>
		void printArg2(T t)
		{
			std::cout << t;
		}
		template <typename ...Args>
		void expandArgs2(Args... args)
		{
			std::cout <<'[';
			//int arr[] = { (printArg2(args), 0)... };
			//(void)std::initializer_list<int>{ (printArg2(args), 0)... };
			std::initializer_list<int>{ (printArg2(args), 0)... };
			std::cout << ']' << "\n";
		}
		template <unsigned char Head, unsigned char End, typename ...Args>
		void expandArgs3(Args... args)
		{
			std::cout << Head;
			//int arr[] = { (printArg2(args), 0)... };
			//(void)std::initializer_list<int>{ (printArg2(args), 0)... };
			std::initializer_list<int>{ (printArg2(args), 0)... };
			std::cout << End << "\n";
		}

		///////////////////////
		template<class F, class... Args>void expandFT(const F& f, Args&&...args)
		{
			ClassTA ta{ (f(std::forward<Args>(args)), 0)... };
		}
		template<class F, class... Args>void expandFT1(const F& f, Args&&...args)
		{
			std::initializer_list<int>{(f(std::forward<Args>(args)), 0)...};
		}
		template<class F, class... Args>void expandFT2(const std::string& head, const std::string& end, const F& f, Args&&...args)
		{
			size_t index = 0;
			std::cout << head;
			std::initializer_list<int>{(f(sizeof...(args), index, std::forward<Args>(args)), 0)...};
			std::cout << end;
		}

		template<class F, class... Args>void expandFT3(const std::string& head, const std::string& end, const F& f, Args&&...args)
		{
			size_t index = 0;
			std::cout << head;
			auto func = [=](size_t total, size_t& index, auto v) {
				index++;
				f(v, !((total - index) > 0));
			};
			std::initializer_list<int>{(func(sizeof...(args), index, std::forward<Args>(args)), 0)...};
			std::cout << end;
		}
		///////////////////////
		template <class... T>
		void func01(T... args)
		{
			std::cout << sizeof...(args) << std::endl;
		}
		void testMain()
		{
			std::cout << "templateDemo_b01::testMain() begin.\n";
			// thanks: https://www.cnblogs.com/qicosmos/p/4325949.html
			func01();
			std::cout << "--- --- ---\n";
			func01(1, 2);
			std::cout << "--- --- ---\n";
			func01(1, 2.5, "hello");

			std::cout << "--- --- print01 --- ---\n";
			std::cout << "[";
			print01(1, 2.5, "hello");

			std::cout << "--- --- print01A --- ---\n";
			print01A(1, 2.5, "hello");


			std::cout << "--- --- expandArgs --- ---\n";
			expandArgs(1, 2.5, "hello");
			std::cout << "--- --- expandArgs2 --- ---\n";
			expandArgs2(1, 2.5, "hello");
			std::cout << "--- --- expandArgs3 --- ---\n";
			expandArgs3<'<','>'>(1, 2.5, "hello");
			std::cout << "--- --- expandFT --- ---\n";
			expandFT([](auto i) {std::cout << i << std::endl; }, 1, 2, 3,"vv",6);
			std::cout << "--- --- expandFT2 --- ---\n";
			expandFT2("<<<",">>>\n", [](size_t total, size_t& index, auto v) {
				index++;
				if ((total - index) > 0) {
					std::cout << v << ",";
				}
				else {
					std::cout << v;
				}
				}, 1, 2, 3, "vv", 6);
			std::cout << "--- --- expandFT3 --- ---\n";
			expandFT3("<<<", "", [](auto v, bool last) {
				if (last) {
					std::cout << "(" << v << ")";
				}
				else {
					std::cout << "(" << v << "),";
				}
			}, 1, 2, 3, "vv", 6);
			expandFT3(",", ">>>\n", [](auto v, bool last) {
				std::stringstream ss;
				ss << v;

				if (last) {
					std::cout << std::quoted(ss.str());
				}
				else {
					std::cout << std::quoted(ss.str()) << ",";
				}
				}, 1, 2, 3, "vv", 6);
			std::cout << "templateDemo_b01::testMain() end.\n";
		}
	}
	void testMain()
	{
		std::boolalpha(std::cout);
		std::cout << "demoTemplate::base04::testMain() begin.\n";
		templateDemo_b01::testMain();
		templateDemo_b02::testMain();
		std::cout << "demoTemplate::base04::testMain() end.\n";

	}
}