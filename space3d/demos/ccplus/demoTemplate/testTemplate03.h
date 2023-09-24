#include <iostream>
#include <climits>
#include <concepts>
#include <cstdint>
#include <iomanip>
#include <type_traits>
#include <utility>
#include <vector>
#include <new>
#include <string>
#include <tuple>
#include <cstddef>
#include <concepts>
#include <locale>

namespace demoTemplate::base03
{
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#    define __PRETTY_FUNCTION__ __FUNCSIG__
#endif
	namespace templateDemo_b08
	{
		using namespace std::literals;
		// thanks: https://en.cppreference.com/w/cpp/language/constraints
		// thanks: https://en.cppreference.com/w/cpp/experimental/constraints
		// 
		// Declaration of the concept "EqualityComparable", which is satisfied by
		// any type T such that for values a and b of type T,
		// the expression a==b compiles and its result is convertible to bool
		// 
		//template<typename T>
		//concept bool EqualityComparable = requires(T a, T b) {
		//	{ a == b } -> bool;
		//};
		//void func01(EqualityComparable&&); // declaration of a constrained function template
		// 
		// 
		// 
		// template<typename T>
		// void f(T&&) requires EqualityComparable<T>; // long form of the same
		// 
		// Declaration of the concept "Hashable", which is satisfied by any type 'T'
		// such that for values 'a' of type 'T', the expression std::hash<T>{}(a)
		// compiles and its result is convertible to std::size_t
		template<typename T>
		concept Hashable = requires(T a)
		{
			{ std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
		};

		struct meow {};

		// Constrained C++20 function template:
		template<Hashable T>
		void f(T a) {

			std::cout << "a: " << a << "\n";
		}
		//
		// Alternative ways to apply the same constraint:
		// template<typename T>
		//     requires Hashable<T>
		// void f(T) {}
		//
		// template<typename T>
		// void f(T) requires Hashable<T> {}
		//
		// void f(Hashable auto /*parameterName*/) {}

		void testMain()
		{
			std::cout << "templateDemo_b08::testMain() begin.\n";
			//using std::operator""s;

			f("abc"s);    // OK, std::string satisfies Hashable
			// f(meow{}); // Error: meow does not satisfy Hashable
			std::cout << "templateDemo_b08::testMain() end.\n";
		}
	}
	namespace templateDemo_b07
	{
		template <class... _Ty>
		struct my_common_type;
		//template <class... _Ty>
		//using my_common_type_t = typename  my_common_type<_Ty...>::type;

		//template <>
		//struct  my_common_type<> {};

		template <typename... T>
		struct TypeWrapper
		{
			TypeWrapper(T... args)
			{
				value = (... + args);
			}
			std::common_type_t<T...> value;
		};
		template <class _Ty, _Ty _Val>
		struct IntegralConstant {
			static constexpr _Ty value = _Val;

			using value_type = _Ty;
			using type = IntegralConstant;

			constexpr operator value_type() const noexcept {
				return value;
			}

			_NODISCARD constexpr value_type operator()() const noexcept {
				return value;
			}
		};
		void testMain()
		{
			std::cout << "templateDemo_b07::testMain() begin.\n";
			IntegralConstant<int, 5> i5;
			IntegralConstant<int, 5>::value_type p5_v = 10;

			//my_common_type_t<int> v_0 = 0;
			//std::cout << "v_0: " << v_0 << "\n";

			std::common_type<int>::type value;

			std::cout << "templateDemo_b07::testMain() end.\n";
		}
	}
	namespace templateDemo_b06
	{
		template<typename T>
		std::string format(T t) {
			std::stringstream ss;
			ss << "[" << t << "]";
			return ss.str();
		}
		template<typename... Args>
		void FormatPrint(Args... args)
		{
			//(std::cout << ... << args) << std::endl;
			(std::cout << ... << format(args)) << std::endl;
		}
		void testMain()
		{
			std::cout << "templateDemo_b06::testMain() begin.\n";
			FormatPrint(1, 2, 3, 4);
			FormatPrint("good", 2, "hello", 4, 110);
			std::cout << "templateDemo_b06::testMain() end.\n";
		}
	}
	namespace templateDemo_b05
	{
		// thanks: https://blog.csdn.net/albertsh/article/details/123978539
		template<std::size_t k = 0, typename tup>
		typename std::enable_if<k == std::tuple_size<tup>::value>::type
		FormatTuple(const tup& t)
		{
			//std::cout << "FUNC DETAIL: " << __PRETTY_FUNCTION__ << '\n';
			//std::cout << ">M2"<< (k == std::tuple_size<tup>::value);
			std::cout << std::endl;
		}

		template<std::size_t k = 0, typename tup>
		typename std::enable_if < k < std::tuple_size<tup>::value, void>::type
		FormatTuple(const tup& t) {
			//std::cout << ">M1" << (k < std::tuple_size<tup>::value);
			std::cout << "[" << std::get<k>(t) << "]";
			FormatTuple<k + 1>(t);
		}

		template<typename... Args>
		void FormatPrint(Args... args)
		{
			FormatTuple(std::make_tuple(args...));
		}

		void testMain()
		{
			std::cout << "templateDemo_b05::testMain() begin.\n";
			FormatPrint(1, 2, 3, 4);
			FormatPrint("good", 2, "hello", 4, 110);
			std::cout << "templateDemo_b05::testMain() end.\n";
		}
	}
	namespace templateDemo_b04
	{
		template <class ...Args>
		void FormatPrint(Args... args)
		{
			std::cout << "FormatPrint(), sizeof...(args): " << sizeof...(args) << "\n";
			(void)std::initializer_list<int>{ (std::cout << "[" << args << "]", 0)... };
			std::cout << std::endl;
		}

		template <class ...Args>
		void FormatPrint2(Args... args)
		{
			std::cout << "FormatPrint2(), sizeof...(args): " << sizeof...(args) << "\n";
			std::initializer_list<int> t = { (std::cout << "[" << args << "]", 0)... };
			std::cout << std::endl;
		}
		void testMain()
		{
			std::cout << "templateDemo_b04::testMain() begin.\n";
			FormatPrint(1, 2, 3, 4);
			FormatPrint("good", 2, "hello", 4, 110);
			FormatPrint2("2good", 22, "2hello", 23, 2110);
			std::cout << "templateDemo_b04::testMain() end.\n";
		}
	}
	namespace templateDemo_b03
	{
		void FormatPrint()
		{
			std::cout << std::endl;
		}

		template <class T, class ...Args>
		void FormatPrint(T first, Args... args)
		{
			std::stringstream ss;
			ss << first;

			//std::cout << "[" << first << "]";
			std::cout << "[" << std::quoted(ss.str()) << "]";
			FormatPrint(args...);
		}

		void testMain(void)
		{
			std::cout << "templateDemo_b03::testMain() begin.\n";
			FormatPrint(1, 2, 3, 4);
			FormatPrint("good", 2, "hello", 4, 110);
			std::cout << "templateDemo_b03::testMain() end.\n";
		}
	}
	namespace templateDemo_b02
	{
		template<class T, std::size_t N>
		class static_vector
		{
			// properly aligned uninitialized storage for N T's
			std::aligned_storage_t<sizeof(T), alignof(T)> data[N];
			std::size_t m_size = 0;

		public:
			// Create an object in aligned storage
			template<typename ...Args> void emplace_back(Args&&... args)
			{
				if (m_size >= N) // possible error handling
					throw std::bad_alloc{};

				// construct value in memory of aligned storage
				// using inplace operator new
				::new(&data[m_size]) T(std::forward<Args>(args)...);
				++m_size;
			}

			// Access an object in aligned storage
			const T& operator[](std::size_t pos) const
			{
				// Note: std::launder is needed after the change of object model in P0137R1
				return *std::launder(reinterpret_cast<const T*>(&data[pos]));
			}

			// Destroy objects from aligned storage
			~static_vector()
			{
				for (std::size_t pos = 0; pos < m_size; ++pos)
					// Note: std::launder is needed after the change of object model in P0137R1
					std::destroy_at(std::launder(reinterpret_cast<T*>(&data[pos])));
			}
		};

		void testMain()
		{
			std::cout << "templateDemo_b02::testMain() begin.\n";
			static_vector<std::string, 10> v1;
			v1.emplace_back(5, '*');
			v1.emplace_back(10, '*');
			std::cout << v1[0] << '\n' << v1[1] << '\n';
			std::cout << "templateDemo_b02::testMain() end.\n";
		}
	}
	namespace templateDemo_b01
	{
		template<typename... Args>
		void printer(Args&&... args)
		{
			(std::cout << ... << args) << '\n';
		}

		template<typename T, typename... Args>
		void push_back_vec(std::vector<T>& v, Args&&... args)
		{
			static_assert((std::is_constructible_v<T, Args&&> && ...));
			(v.push_back(std::forward<Args>(args)), ...);
		}

		template<class T, std::size_t... dummy_pack>
		constexpr T bswap_impl(T i, std::index_sequence<dummy_pack...>)
		{
			T low_byte_mask = (unsigned char)-1;
			T ret{};
			([&]
				{
					(void)dummy_pack;
					ret <<= CHAR_BIT;
					ret |= i & low_byte_mask;
					i >>= CHAR_BIT;
				}(), ...);
			return ret;
		}

		constexpr auto bswap(std::unsigned_integral auto i)
		{
			return bswap_impl(i, std::make_index_sequence<sizeof(i)>{});
		}

		void testMain()
		{
			std::cout << "templateDemo_b01::testMain() begin.\n";

			printer(1, 2, 3, "abc");

			std::vector<int> v;
			push_back_vec(v, 6, 2, 45, 12);
			push_back_vec(v, 1, 2, 9);
			for (int i : v) std::cout << i << ' ';

			static_assert(bswap<std::uint16_t>(0x1234u) ==
				0x3412u);
			static_assert(bswap<std::uint64_t>(0x0123456789abcdefull) ==
				0xefcdab8967452301ULL);

			std::cout << "templateDemo_b01::testMain() end.\n";
		}
	}

	void testMain()
	{
		std::boolalpha(std::cout);
		std::cout << "demoTemplate::base03::testMain() begin.\n";
		templateDemo_b01::testMain();
		templateDemo_b02::testMain();
		templateDemo_b03::testMain();
		templateDemo_b04::testMain();
		templateDemo_b05::testMain();
		templateDemo_b06::testMain();
		templateDemo_b07::testMain();
		templateDemo_b08::testMain();
		std::cout << "demoTemplate::base03::testMain() end.\n";

	}
}