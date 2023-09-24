#include <iostream>
#include <thread>

namespace demoTemplate::base05
{
	namespace templateDemo_b05
	{
		//thread_local unsigned int rage = 1;
		//thread_local static unsigned int rage = 1;
		template<class T>
		struct S
		{
			thread_local static int tlm;
		};
		template<>
		thread_local int S<float>::tlm = 0; // "static" ���ڴ˳���

		void testMain()
		{
			std::cout << "templateDemo_b05::testMain() begin.\n";

			std::cout << "templateDemo_b05::testMain() end.\n";
		}
	}
	namespace templateDemo_b04
	{
		class foo
		{
			static int i;
			//static int i;//error: redefine
			float speed;
			//float speed;//error: redefine
		public:
			int fooFunc();
			//int fooFunc();//error: redefine 
		};

		void testMain()
		{
			std::cout << "templateDemo_b04::testMain() begin.\n";
			std::cout << "templateDemo_b04::testMain() end.\n";
		}
	}
	namespace templateDemo_b03
	{
		// non-type template parameter
		// thanks: https://blog.csdn.net/xiexievv/article/details/8487132

		template<int>
		void fun1() {}

		template<int*>
		void fun1_1() {}

		template<int&>
		void fun1_2() {}

		template<char*>
		void fun2() {}

		template<char>
		void fun3() {}

		struct	CA {};
		union	Ua {};

		// yes or no: ������ ����float,double���ǲ�����Ϊ non-type template parameter��
		template<float>
		void fun4()	{}

		// Ҫʹ����Ļ������ϵ�ָ�� ��������
		//template<CA>			
		//void fun4()	{}

		//template<Ua>
		//void fun5()	{}

		template<CA*>
		void fun4_1() {}

		template<CA&>
		void fun4_2() {}

		template<Ua*>
		void fun5_1() {}

		template<Ua&>
		void fun5_2() {}

		//------------------------------------------------------------------------//
		int			idata3 = 20;				// �ⲿ����
		volatile int			idata3_a = 20;	// �ⲿ����
		const int	idata2 = 9;					// �ڲ�����

		CA			impCa1;						// �ⲿ����
		const CA	impCa2;						// �ڲ�����
		static CA	impCa3;						// �ڲ�����

		Ua			impUa1;						// �ⲿ����
		const Ua	impUa2;						// �ڲ�����
		static Ua	impUa3;						// �ڲ�����

		const char	str3[] = "str";				// �ڲ�����
		static char	str4[] = "str";				// �ڲ�����
		char		str5[] = "str";				// �ⲿ����

		char* str6 = {0};		// �ⲿ����
		const char* str7 = "str";		// �ڲ�����
		char* const str8 = (char* const)"str";		// �ڲ�����
		const char* const str9 = "str";	// �ڲ�����

		const char	chr = 'x';		// �ڲ�����
		void funtest(int);
		void funtest(int) {}

		static void funtest2(int);
		void funtest2(int) {}

		typedef void (*ptr_func)(int);

		template<ptr_func>
		void fun6() {}

		ptr_func pfun2 = &funtest;
		const ptr_func pfun3 = &funtest;
		static const ptr_func pfun4 = &funtest;

		void testMain()
		{
			std::cout << "templateDemo_b03::testMain() begin.\n";
			int i = 12;
			//const int idata = i;	// error 2971 �ֲ������������������Ͳ���
			const int idata = 10;	// �ֲ����� ok
			fun1<idata>();			// ok

			int idata4 = 20;
			//fun1_1<&idata2>();	// error c2970 �漰�������ڲ����ӵı��ʽ������Ϊ non-type argument.
			//fun1_1<&idata4>();	// error c2971(mscv: c2672) 
			fun1_1<&idata3>();		// ok
			//fun1_1<&idata3_a>();		// error c2672

			//fun1_2<idata2>();		// error c2970
			//fun1_2<idata4>();		// error c2971 
			fun1_2<idata3>();		// ok

			char		str1[] = "string";
			const char* str2 = "string";
			//fun2<str1>();			// error c2971, because the str1 is a local variable
			//fun2<str2>();			// error c2971
			//fun2<str3>();			// error c2970 
			//fun2<str4>();			// error c2970
			//fun2<"test">();		// error c2762 ģ������ı��ʽ��Ч,Ϊʲô����Ч���ַ���"test"�����ڲ�����? ����1��
			fun2<str5>();			// ok
			//fun2<str6>();			// error 2975 ��Ҫһ�������ڳ������ʽ
			//fun2<str7>();			// error 2975
			//fun2<str8>();			// error c2971
			//fun2<str9>();			// error c2971
			fun4<1.0f>();

			//fun3<str2[1]>();		// error 2975 ��Ҫһ�������ڳ������ʽ
			//fun3<str3[0]>();		// error 2975 Ϊʲôstr3[0]���Ǳ����ڳ���������2
			fun3<'x'>();			// ok
			fun3<chr>();			// ok

			// ok
			fun4_1<&impCa1>();
			fun4_2<impCa1>();

			// ok
			fun5_1<&impUa1>();
			fun5_2<impUa1>();

			//
			ptr_func pfun = &funtest;
			//fun6<pfun>();		// error c2971
			//fun6<pfun2>();	// error c2975
			//fun6<pfun3>();	// error C2970
			//fun6<&pfun4>();	// error C2970
			fun6<&funtest>();	// ok
			fun6<&funtest2>();

			std::cout << "templateDemo_b03::testMain() end.\n";
		}

	}
	namespace templateDemo_b02
	{
		//template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
		template <typename T>
		class NumCalcUnit
		{
		public:
			T value;
			T addOne() {
				value += 1;
				return value;
			}
		};

		//template <typename T, std::enable_if_t<std::is_floating_point<T>::value, bool> = true>
		template<>
		class NumCalcUnit<float>
		{
		public:
			float value;
			float addOne() {
				std::cout << "call NumCalcUnit<float>::addOne().\n";
				value += 1.0f;
				return value;
			}
		};
		template<>
		class NumCalcUnit<int>
		{
		public:
			int value;
			float addOne() {
				std::cout << "call NumCalcUnit<int>::addOne().\n";
				value += 1;
				return value;
			}
		};

		struct ICounter {
			virtual void increase() = 0;
			virtual ~ICounter() {}
		};

		struct Counter : public ICounter {
			void increase() override {
				std::cout << "call Counter::increase().\n";
			}
		};
		template <typename T>
		void inc_counter(T& counterObj, typename std::enable_if<std::is_base_of<ICounter, T>::value>::type* = nullptr) {
			counterObj.increase();
		}
		template <typename T>
		void inc_counter(T& counterInt, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) {
			counterInt ++;
			std::cout << "inc_counter call integral + 1.\n";
		}
		// thanks: https://en.cppreference.com/w/cpp/types/enable_if
		template <typename T, std::enable_if_t<std::is_floating_point<T>::value, int> = 0>
		void inc_counter2(T& counterObj) {
			counterObj += 1.0f;
			std::cout << "inc_counter2 call float + 1.0.\n";
		}
		template <typename Ty, std::enable_if_t<std::is_base_of<ICounter, Ty>::value, bool> = true>
		void inc_counter2(Ty& counterObj) {
			counterObj.increase();
		}
		template <typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
		void inc_counter2(T& counterObj) {
			counterObj += 1;
			std::cout << "inc_counter2 call integral + 1.\n";
		}

		template <typename T>
		void inc_counter3(T& counterObj, decltype(counterObj.increase())* = nullptr) {
			counterObj.increase();
		}
		template <typename T>
		void inc_counter3(T& counterInt, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr) {
			counterInt++;
			std::cout << "inc_counter3 call integral + 1.\n";
		}

		template <typename T>
		void inc_counter4(T& counterObj, std::decay_t<decltype(counterObj.increase())>* = nullptr) {
			counterObj.increase();
		}
		template <typename T>
		void inc_counter4(T& counterInt, std::decay_t<decltype(++counterInt)>* = nullptr) {
			counterInt++;
			std::cout << "inc_counter4 call integral + 1, counterInt: " << counterInt <<"\n";
		}

		struct X {
			typedef int type;
		};

		struct Y {
			typedef int type2;
		};

		template <typename T> void foo(typename T::type t) {
			std::cout << "call foo 01.\n";
		}
		template <typename T> void foo(typename T::type2 t) {
			std::cout << "call foo 02.\n";
		}
		template <typename T> void foo(T t) {
			std::cout << "call foo 03.\n";
		}
		void testMain()
		{
			std::cout << "templateDemo_b02::testMain() begin.\n";
			foo<X>(5);
			foo<Y>(10);
			foo<int>(15);
			std::cout << "--- --- inc_counter --- ---\n";

			Counter cntObj{};
			int cntUI32{};
			float cntFloat{};

			inc_counter(cntObj, (void*)0);
			inc_counter(cntUI32);
			std::cout << "--- --- inc_counter2 --- ---\n";

			inc_counter2(cntUI32);
			inc_counter2(cntFloat);
			inc_counter2(cntObj);

			std::cout << "--- --- inc_counter3 --- ---\n";

			inc_counter3(cntObj);
			inc_counter3(cntUI32);

			std::cout << "--- --- inc_counter4 --- ---\n";

			inc_counter4(cntObj);
			inc_counter4(cntUI32);

			std::cout << "--- --- NumCalcUnit --- ---\n";

			NumCalcUnit<int> intCU;
			NumCalcUnit<float> floatCU;
			intCU.value = 1.5f;
			floatCU.value = 1.5f;
			intCU.addOne();
			floatCU.addOne();
			std::cout << "intCU.value: " << intCU.value << "\n";
			std::cout << "floatCU.value: " << floatCU.value << "\n";


			std::cout << "templateDemo_b02::testMain() end.\n";
		}
	}
	namespace templateDemo_b01
	{
		template <class>
		inline constexpr bool is_const_t_v = false; // determine whether type argument is const qualified

		template <class _Ty>
		inline constexpr bool is_const_t_v<const _Ty> = true;

		template <class>
		inline constexpr bool is_volatile_t_v = false; // determine whether type argument is volatile qualified

		template <class _Ty>
		inline constexpr bool is_volatile_t_v<volatile _Ty> = true;

		template <class _Ty, _Ty _Val>
		struct integral_constant_t {
			static constexpr _Ty value = _Val;

			using value_type = _Ty;
			using type = integral_constant_t;

			constexpr operator value_type() const noexcept {
				return value;
			}

			_NODISCARD constexpr value_type operator()() const noexcept {
				return value;
			}
		};

		template <bool _Val>
		using bool_constant_t = integral_constant_t<bool, _Val>;

		template <class _Ty>
		struct is_const_t : bool_constant_t<is_const_t_v<_Ty>> {};

		template <class _Ty>
		struct is_volatile_t : bool_constant_t<is_volatile_t_v<_Ty>> {};

		template <typename T> std::string type_name_string();

		template <class T>
		std::string type_name_string()
		{
			typedef typename std::remove_reference<T>::type TR;
			std::unique_ptr<char, void(*)(void*)> own
			(
#ifndef _MSC_VER
				abi::__cxa_demangle(typeid(TR).name(), nullptr,
					nullptr, nullptr),
#else
				nullptr,
#endif
				std::free
			);
			std::string r = own != nullptr ? own.get() : typeid(TR).name();
			if (std::is_const<TR>::value)
				r += " const";
			if (std::is_volatile<TR>::value)
				r += " volatile";
			if (std::is_lvalue_reference<T>::value)
				r += "&";
			else if (std::is_rvalue_reference<T>::value)
				r += "&&";
			return r;
		}
		void testMain()
		{
			std::cout << "templateDemo_b01::testMain() begin.\n";

			auto boolValue0 = is_const_t_v<int>;
			std::cout << "boolValue0: " << boolValue0 << "\n";
			auto boolValue1 = is_const_t_v<const int>;
			std::cout << "boolValue1: " << boolValue1 << "\n";

			int v_0 = 0;
			decltype(v_0) k_0 = 1;
			//auto boolValue2 = is_const_t<decltype(v_0)>::value;
			auto boolValue2 = is_const_t<decltype(v_0)>();
			std::cout << "is_const_t, boolValue2: " << boolValue2 << "\n";

			const int v_1 = 0;
			auto boolValue3 = is_const_t<decltype(v_1)>();
			std::cout << "is_const_t, boolValue3: " << boolValue3 << "\n";

			int v_2 = 0;
			auto boolValue4 = is_volatile_t<decltype(v_2)>();
			std::cout << "is_volatile_t, boolValue4: " << boolValue4 << "\n";

			volatile int v_3 = 0;
			auto boolValue5 = is_volatile_t<decltype(v_3)>();
			std::cout << "is_volatile_t, boolValue5: " << boolValue5 << "\n";

			const int ci = 0;
			std::cout << "type_name_string: " << type_name_string<decltype(ci)>() << '\n';
			std::cout << "templateDemo_b01::testMain() end.\n";
		}
	}

	void testMain()
	{
		std::boolalpha(std::cout);
		std::cout << "demoTemplate::base05::testMain() begin.\n";
		templateDemo_b01::testMain();
		templateDemo_b02::testMain();
		templateDemo_b03::testMain();
		std::cout << "demoTemplate::base05::testMain() end.\n";
	}
}