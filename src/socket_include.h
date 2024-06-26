
#ifndef _SOCKETS_socket_include_H
#define _SOCKETS_socket_include_H

#include <string>
#include <memory>


//#define info_warning(format, ...) printf("File: "__FILE__", Line: %05d: "format"\n", __LINE__, ##__VA_ARGS__)



#ifdef _MSC_VER
#pragma warning(disable:4514)
#endif


#if !defined(__WINDOWS__) && (defined(WIN32) || defined(WIN64) || defined(_MSC_VER) || defined(_WIN32))
#define __WINDOWS__
#endif

#ifdef __WINDOWS__
    #undef _WIN32_WINNT
    #define _WIN32_WINNT 0x0601

    #define LIB_CDECL __cdecl
    #define LIB_STDCALL __stdcall

    /* export symbols by default, this is necessary for copy pasting the C and header file */
    #if !defined(LIB_HIDE_SYMBOLS) && !defined(LIB_IMPORT_SYMBOLS) && !defined(LIB_EXPORT_SYMBOLS)
        #define LIB_EXPORT_SYMBOLS
    #endif

    #if defined(LIB_HIDE_SYMBOLS)
        #define LIB_PUBLIC(type)   type LIB_STDCALL
    #elif defined(LIB_EXPORT_SYMBOLS)
        #define LIB_PUBLIC(type)   __declspec(dllexport) type LIB_STDCALL
        #define LIB_DESCRIBE       __declspec(dllexport)
    #elif defined(LIB_IMPORT_SYMBOLS)
        #define LIB_PUBLIC(type)   __declspec(dllimport) type LIB_STDCALL
    #endif
#else /* !__WINDOWS__ */
    #define LIB_CDECL
    #define LIB_STDCALL

    #if (defined(__GNUC__) || defined(__SUNPRO_CC) || defined (__SUNPRO_C)) && defined(CJSON_API_VISIBILITY)
        #define LIB_PUBLIC(type)   __attribute__((visibility("default"))) type
    #else
        #define LIB_PUBLIC(type) type
    #endif
#endif




// common defines affecting library and applications using library

/* Define SOCKETS_DYNAMIC_TEMP to use dynamically allocated buffers
   in read operations - helps on ECOS */
#define SOCKETS_DYNAMIC_TEMP

/** define type to uniquely identify a socket instance. */
typedef unsigned long socketuid_t;

// platform specific stuff
#if (defined(__unix__) || defined(unix)) && !defined(USG)
    #include <sys/param.h>
    #include <unistd.h>
#endif
#include <list>

#ifdef __WINDOWS__
    #ifndef uint64_t
        typedef unsigned __int64 uint64_t;
    #endif
    #ifndef int64_t
        typedef __int64 int64_t;
    #endif
#else
    #include <stdlib.h>
    #ifdef SOLARIS
        # include <sys/types.h>
    #else
        # include <stdint.h>
    #endif
#endif

#ifndef __WINDOWS__
    // common unix includes / defines
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>


    #include <netdb.h>


    // all typedefs in this file will be declared outside the sockets namespace,
    // because some os's will already have one or more of the type defined.
    typedef int SOCKET;
    #define Errno errno
    #define StrError strerror

    // WIN32 adapt
    #define closesocket close
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1

    #ifndef INADDR_NONE
    #define INADDR_NONE ((unsigned long) -1)
    #endif // INADDR_NONE

#endif // !_WIN32


// ----------------------------------------
// Generic
#ifndef SOL_IP
#define SOL_IP IPPROTO_IP
#endif


// ----------------------------------------
// OS specific adaptions

#ifdef SOLARIS 
// ----------------------------------------
// Solaris
typedef unsigned short port_t;
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
// no defs

#ifdef SOCKETS_NAMESPACE
}
#endif

#define s6_addr16 _S6_un._S6_u8
#define MSG_NOSIGNAL 0

#elif defined __FreeBSD__
// ----------------------------------------
// FreeBSD
# if __FreeBSD_version >= 400014
#  define s6_addr16 __u6_addr.__u6_addr16
#  if !defined(MSG_NOSIGNAL)
#   define MSG_NOSIGNAL 0
#  endif
#  include <netinet/in.h>
typedef	in_addr_t ipaddr_t;
typedef	in_port_t port_t;
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
// no defs

#ifdef SOCKETS_NAMESPACE
}
#endif

#  define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#  define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP
# else
#  error FreeBSD versions prior to 400014 does not support ipv6
# endif

#elif defined MACOSX 
// ----------------------------------------
// Mac OS X
//#include <string.h>
#ifdef __DARWIN_UNIX03
typedef unsigned short port_t;
#else
#include <mach/port.h>
#endif // __DARWIN_UNIX03
typedef unsigned long ipaddr_t;
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
// no defs

#ifdef SOCKETS_NAMESPACE
}
#endif

#define s6_addr16 __u6_addr.__u6_addr16
#define MSG_NOSIGNAL 0 // oops - thanks Derek
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#define IPV6_DROP_MEMBERSHIP IPV6_LEAVE_GROUP

#elif defined _WIN32 
// ----------------------------------------
// Win32
#ifdef _MSC_VER
#pragma comment(lib, "wsock32.lib")
#endif
#define strcasecmp _stricmp
#ifndef __CYGWIN__
#define snprintf sprintf_s
#define vsnprintf vsprintf_s
#endif

typedef unsigned long ipaddr_t;
typedef unsigned short port_t;
typedef int socklen_t;
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
// no defs

#ifdef SOCKETS_NAMESPACE
}
#endif

// 1.8.6: define FD_SETSIZE to something bigger than 64 if there are a lot of
// simultaneous connections (must be done before including winsock.h)
//#define FD_SETSIZE 1024

// windows 2000 with ipv6 preview installed:
//    http://msdn.microsoft.com/downloads/sdks/platform/tpipv6.asp
// see the FAQ on how to install
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#if _MSC_VER < 1200
#ifndef __CYGWIN__
#ifdef ENABLE_IPV6
#include "tpipv6.h"  // For IPv6 Tech Preview.
#endif
#endif
#endif // _MSC_VER < 1200


#define MSG_NOSIGNAL 0
//#define SHUT_RDWR 2
#define SHUT_WR 1

#define Errno WSAGetLastError()
const char *StrError(int x);

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif


// class WSAInitializer is a part of the Socket class (on win32)
// as a static instance - so whenever an application uses a Socket,
// winsock is initialized
class WSAInitializer // Winsock Initializer
{
public:
	WSAInitializer() {
		if (WSAStartup(0x101,&m_wsadata)) 
		{
			exit(-1);
		}

        HANDLE hTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;

        if(!GetConsoleMode(hTerminal, &dwMode))
        {
            printf("error:%d\n", GetLastError());
        }
        dwMode |= 0x0004;
        if (!SetConsoleMode(hTerminal, dwMode))
        {
            printf("error:%d\n", GetLastError());
        }


	}
	~WSAInitializer() {
		WSACleanup();
	}
private:
	WSADATA m_wsadata;
};

#ifdef SOCKETS_NAMESPACE
}
#endif

#else 
// ----------------------------------------
// LINUX 
typedef unsigned long ipaddr_t;
typedef unsigned short port_t;
#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
// no defs

#ifdef SOCKETS_NAMESPACE
}
#endif


#endif

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif
	/** List type containing file descriptors. */
	typedef std::list<SOCKET> socket_v;


#ifdef SOCKETS_NAMESPACE
}
#endif


// getaddrinfo / getnameinfo replacements
#ifdef NO_GETADDRINFO
#ifndef AI_NUMERICHOST
#define AI_NUMERICHOST 1
#endif
#ifndef NI_NUMERICHOST
#define NI_NUMERICHOST 1
#endif
#endif


#endif // _SOCKETS_socket_include_H

