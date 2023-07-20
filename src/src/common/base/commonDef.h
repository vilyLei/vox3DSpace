/** \file commonDef.h
 *  \brief Include all needed defines and macros
 *  Here all defines and macro switching is done for FreeCAD.
 *  Every used library has its own section to define the configuration.
 *  This file keeps the makefiles and project files cleaner.
 */
#ifndef COMMON_DEF_H
#define COMMON_DEF_H

#define  FC_ENDIAN_LITTLE   0
#define  FC_ENDIAN_BIG      1	  	
#define  FC_ENDIAN	        FC_ENDIAN_LITTLE

#define FC_PLATFORM_APPLE       0
#define FC_PLATFORM_APPLE_IOS	1
#define FC_PLATFORM_WIN32       2

#if defined(WIN32)	|| defined(_WIN64) 	|| defined(__WIN32__) 
#define FC_PLATFORM  	FC_PLATFORM_WIN32
#endif

//**************************************************************************
// switching the operating systems

// First check for *WIN64* since the *WIN32* are also set on 64-bit platforms
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#   ifndef FC_OS_WIN32
#   define FC_OS_WIN32
#   endif
#   ifndef FC_OS_WIN64
#   define FC_OS_WIN64
#   endif
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#   ifndef FC_OS_WIN32
#   define FC_OS_WIN32
#   endif
#   if defined(__MINGW32__)
#   if HAVE_CONFIG_H
#   include <config.h>
#   endif // HAVE_CONFIG_H

#   endif
#elif defined(__MWERKS__) && defined(__INTEL__)
#   ifndef FC_OS_WIN32
#   define FC_OS_WIN32
#   endif
#elif defined(__APPLE__)

#define FC_PLATFORM      FC_PLATFORM_APPLE

#   ifndef FC_OS_MACOSX
#   define FC_OS_MACOSX
#   endif
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GLIBC__)
#   ifndef FC_OS_LINUX
#   define FC_OS_LINUX
#   endif
#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
#   ifndef FC_OS_BSD
#   define FC_OS_BSD
#   endif
#elif defined(__CYGWIN__)
#   ifndef FC_OS_CYGWIN
#   define FC_OS_CYGWIN
// Avoid conflicts with Inventor
#   define HAVE_INT8_T
#   define HAVE_UINT8_T
#   define HAVE_INT16_T
#   define HAVE_UINT16_T
#   define HAVE_INT32_T
#   define HAVE_UINT32_T
#   define HAVE_INT64_T
#   define HAVE_UINT64_T
#   define HAVE_INTPTR_T
#   define HAVE_UINTPTR_T
#endif

#else
#   error "HyperRender is not ported to this OS yet."
#endif

#ifdef FC_OS_WIN32
#   define PATHSEP '\\'
#else
#   define PATHSEP '/'
#endif

//**************************************************************************
// Standard types for Windows

#if defined(__MINGW32__)
// nothing specific here
#elif defined (FC_OS_WIN64) || defined (FC_OS_WIN32)

#ifndef HAVE_INT8_T
#define HAVE_INT8_T
typedef signed char         int8_t;
#endif

#ifndef HAVE_UINT8_T
#define HAVE_UINT8_T
typedef unsigned char       uint8_t;
#endif

#ifndef HAVE_INT16_T
#define HAVE_INT16_T
typedef short               int16_t;
#endif

#ifndef HAVE_UINT16_T
#define HAVE_UINT16_T
typedef unsigned short      uint16_t;
#endif

#ifndef HAVE_INT32_T
#define HAVE_INT32_T
typedef int                 int32_t;
#endif

#ifndef HAVE_UINT32_T
#define HAVE_UINT32_T
typedef unsigned int        uint32_t;
#endif

#ifndef HAVE_INT64_T
#define HAVE_INT64_T
typedef __int64             int64_t;
#endif

#ifndef HAVE_UINT64_T
#define HAVE_UINT64_T
typedef unsigned __int64    uint64_t;
#endif

#endif


#ifdef _MSC_VER
#   ifndef WNT
#   define WNT
#   endif
#   ifndef WIN32
#   define WIN32
#   endif
#   ifndef _WINDOWS
#   define _WINDOWS
#   endif
#endif

#ifdef FC_OS_LINUX
#   define LIN
#   define LININTEL
//#       define NO_CXX_EXCEPTION
#endif

#define CSFDB

/// enables the use of the OCC DocumentBrowser
#ifndef FC_OS_LINUX
#   define FC_USE_OCAFBROWSER
#endif


#ifdef FC_OCC_DEBUG
#   ifdef FC_DEBUG
#       define DEBUG 1
#   else
#       undef  DEBUG
#   ifndef NDEBUG
#       define NDEBUG
#   endif
#   endif
#endif


//**************************************************************************
// Boost
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif
//#define DLL_ENABLED
//**************************************************************************
// Exception handling

// Don't catch C++ exceptions in DEBUG!
#ifdef FC_DEBUG
# define DONT_CATCH_CXX_EXCEPTIONS 1
# define DBG_TRY
# define DBG_CATCH(X)
#else
/// used to switch a catch with the debug state
# define DBG_TRY try {
/// see docu DBGTRY
# define DBG_CATCH(X) } catch (...) { X }
#endif

#

//**************************************************************************
// Windows import export DLL defines
#if defined (FC_OS_WIN32) || defined(FC_OS_CYGWIN)
#   ifdef FCApp
#       define AppExport   __declspec(dllexport)
#       define DataExport  __declspec(dllexport)
#   else
#       define AppExport   __declspec(dllimport)
#       define DataExport  __declspec(dllimport)
#   endif
#   ifdef FCBase
#       define BaseExport  __declspec(dllexport)
#   else
#       ifdef DLL_ENABLED
#			define BaseExport  __declspec(dllimport)
#		else
#            define BaseExport
#		endif
#   endif
#   ifdef FCGui
#       define GuiExport   __declspec(dllexport)
#   else
#       define GuiExport   __declspec(dllimport)
#   endif
#   ifdef ImporterDLL
#       define SCENE_IO_API  __declspec(dllexport)
#   else
#       define SCENE_IO_API  __declspec(dllimport)
#   endif

#   ifdef VRDLL
#       define VR_API  __declspec(dllexport)
#   else
#       define VR_API  __declspec(dllimport)
#   endif


#   ifdef DDatDLL
#       define DAT_API  __declspec(dllexport)
#   else
#       define DAT_API  __declspec(dllimport)
#   endif


//*************
// Windows import / export DLL defines
#   ifdef ExportRHI
#       define RC_API  __declspec(dllexport)
#   else
#     ifdef IPMORT_LIB
#       define RC_API 
#     else
#       define RC_API  __declspec(dllimport)
#     endif
#   endif 	  

#   ifdef FCBase
#		define MATH_EMPTY_BASES __declspec( empty_bases)
#   else
#		define MATH_EMPTY_BASES __declspec( empty_bases) 
#   endif





#else
//*****************************************************************************
// compatibility with non-clang compilers...
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_feature
#define __has_feature(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if __has_attribute(visibility)
#       define RC_API        __attribute__ ((visibility("default")))
#       define BaseExport    __attribute__ ((visibility("default")))
#       define DataExport    __attribute__ ((visibility("default")))
#       define SCENE_IO_API  __attribute__ ((visibility("default")))
#       define AppExport     __attribute__ ((visibility("default")))
#       define DDatDLL		 __attribute__ ((visibility("default")))
#       define VR_API        __attribute__ ((visibility("default")))
#       define DAT_API       __attribute__ ((visibility("default")))
#else
#       define RC_API
#       define BaseExport
#       define DataExport
#       define SCENE_IO_API
#       define AppExport
#       define DDatDLL
#       define VR_API
#       define DAT_API
#endif


#   define MATH_EMPTY_BASES
// C++11 allows pragmas to be specified as part of defines using the _Pragma syntax.
#   define MATH_NOUNROLL _Pragma("nounroll")

#endif
//**************************************************************************


//**************************************************************************
// here get the warnings of too long specifiers disabled (needed for VC6)
#ifdef _MSC_VER
#   pragma warning( disable : 4251 )
//#   pragma warning( disable : 4503 )
//#   pragma warning( disable : 4786 )  // specifier longer then 255 chars
//#   pragma warning( disable : 4290 )  // not implemented throw specification
#   pragma warning( disable : 4996 )  // suppress deprecated warning for e.g. open()
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#   pragma warning( disable : 4244 )
#   pragma warning( disable : 4267 )
#endif
//#	define _PreComp_                  // use precompiled header
#endif


#ifndef __has_builtin
#	define __has_builtin(x) 0
#endif

/*
 * helps the compiler's optimizer predicting branches
 */
#if __has_builtin(__builtin_expect)
#   ifdef __cplusplus
#      define UTILS_LIKELY( exp )    (__builtin_expect( !!(exp), true ))
#      define UTILS_UNLIKELY( exp )  (__builtin_expect( !!(exp), false ))
#   else
#      define UTILS_LIKELY( exp )    (__builtin_expect( !!(exp), 1 ))
#      define UTILS_UNLIKELY( exp )  (__builtin_expect( !!(exp), 0 ))
#   endif
#else
#   define UTILS_LIKELY( exp )    (!!(exp))
#   define UTILS_UNLIKELY( exp )  (!!(exp))
#endif



#endif //COMMON_DEF_H
