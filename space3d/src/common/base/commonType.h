#pragma once

// STL 
#include <string>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <set>
#include <stack>
#include <queue>
#include <memory>
#include <bitset>
#include <algorithm>

//streams
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>



namespace baseMath 
{

    template <typename T>
    using TDeque = std::deque<T>;

    template <typename T>
    using TQueue = std::queue<T>;

    template <typename T>
    using TVector = std::vector<T>;

    template <typename T>
    using TList = std::list<T>;

    template <typename T, typename P = std::less<T>>
    using TSet = std::set<T, P>;

    template <typename K, typename V, typename P = std::less<K>>
    using TMap = std::map<K, V, P, std::allocator<std::pair<const K, V>>>;

    template <typename K, typename V, typename P = std::less<K>>
    using TMultimap = std::multimap<K, V, P>;

    template<typename K, typename V, typename H = std::hash<K>, typename KE = std::equal_to<K> >
    using TUnorderedMap = std::unordered_map<K, V, H, KE>;

    template<typename T, typename H = std::hash<T>, typename KE = std::equal_to<T>>
    using TUnorderedSet = std::unordered_set<T, H, KE>;

/* Initial platform/compiler-related stuff to set.
*/
// Integer formats of fixed bit width
    typedef unsigned int	uint32;
    typedef unsigned short	uint16;
    typedef unsigned char	uint8;
    typedef int			int32;
    typedef short		int16;
    typedef signed char int8;
// define uint64 type
#if defined( _MSC_VER )
    typedef unsigned __int64 uint64;
typedef __int64 int64;
#else
    typedef unsigned long long uint64;
    typedef long long int64;
#endif
    typedef float Real;
/** In order to avoid finger-aches :)
*/
    typedef unsigned char  uchar;
    typedef unsigned short ushort;
    typedef unsigned int   uint;
    typedef unsigned long  ulong;

#if defined(_WCHAR_T)
    typedef std::basic_stringstream<wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t> > _StringStreamBase;
#else
    typedef std::basic_stringstream<char, std::char_traits<char>, std::allocator<char> > _StringStreamBase;
#endif

    typedef std::string String;
    typedef std::string ConstString;

    typedef _StringStreamBase StringStream;
    typedef StringStream	  stringstream;
    typedef TVector<std::string> StringVector;
    typedef TSet<std::string>    StringSet;
/// Constant blank string, useful for returning by ref where local does not exist
    const String BLANKSTRING;

	class DataStream;
	class MemoryDataStream;

	typedef std::shared_ptr<DataStream> DataStreamPtr;
	typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
}