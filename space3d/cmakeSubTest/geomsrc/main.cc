#include <iostream>
int main()
{
    std::cout << "geom lib hello world" << std::endl;
    std::cout << "LIB_VERSION_NUMBER: "<< LIB_VERSION_NUMBER << std::endl;
    #ifdef USE_LOG0
		std::cout << "use log 0." << std::endl;
	#endif
    #ifdef USE_LOG1
		std::cout << "use log 1." << std::endl;
	#endif

}