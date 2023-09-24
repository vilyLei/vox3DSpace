#include "testUnit.h"
// 测试内部链接和外部链接
namespace cplus_vvv3_test
{
	int i = 0;							//名字空间变量定义，外部连接
	int foo() { return 0; }				//名字空间函数定义，外部连接
}
int vvv3_test_i = 0;					//全局变量定义，外部连接
int vvv3_test_k = 0;					//全局变量定义，外部连接
int vvv3_test_foo() { return 2; }		//全局函数定义，外部连接

namespace base::testUnit
{
	namespace module_test01
	{
		void testMain()
		{
			//int vvpoo = 1;
			//int vv_bar0 = 0xE+vvpoo;   // 错误：非法的预处理数字 0xE+foo
			//int vv_baz1 = 0xE + vvpoo; // OK
		}
	}
	void testMain()
	{

	}
}