#include "testUnit.h"
// �����ڲ����Ӻ��ⲿ����
namespace cplus_vvv3_test
{
	int i = 0;							//���ֿռ�������壬�ⲿ����
	int foo() { return 0; }				//���ֿռ亯�����壬�ⲿ����
}
int vvv3_test_i = 0;					//ȫ�ֱ������壬�ⲿ����
int vvv3_test_k = 0;					//ȫ�ֱ������壬�ⲿ����
int vvv3_test_foo() { return 2; }		//ȫ�ֺ������壬�ⲿ����

namespace base::testUnit
{
	namespace module_test01
	{
		void testMain()
		{
			//int vvpoo = 1;
			//int vv_bar0 = 0xE+vvpoo;   // ���󣺷Ƿ���Ԥ�������� 0xE+foo
			//int vv_baz1 = 0xE + vvpoo; // OK
		}
	}
	void testMain()
	{

	}
}