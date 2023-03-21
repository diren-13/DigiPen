/************************************************************************************//*!
\file           Socket.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           April 14, 2022
\brief          Contains the implementation of a generic Socket for networking.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <iostream>
// Primary Header
#include "Socket.h"
// Windows Headers
#include <WS2tcpip.h>

namespace Network
{
    /*---------------------------------------------------------------------------------*/
    /* Static Data Member Definitions                                                  */
    /*---------------------------------------------------------------------------------*/
    uint32_t    Socket::count = 0;

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    AddressInfo::AddressInfo()
    : decimalIPv4Address    (static_cast<uint32_t>(INADDR_ANY))
    , portNumber            (0)
    {}

    AddressInfo::AddressInfo(uint32_t ipAddress, uint16_t port)
    : decimalIPv4Address    (ipAddress)
    , portNumber            (port)
    {}

    AddressInfo::AddressInfo(const std::string& ipAddress, uint16_t port)
    : decimalIPv4Address    (static_cast<uint32_t>(INADDR_ANY))
    , portNumber            (port)
    {
        in_addr addr;
        const int RESULT = inet_pton(AF_INET, ipAddress.c_str(), &addr);
        if (RESULT == 1)
        {
            decimalIPv4Address = static_cast<uint32_t>(addr.S_un.S_addr);
        }
    }

    Socket::Socket(Type t)
    : type      ( t )
    , id        ( INVALID_ID )          // use -1 as an invalid socket
    , handle    ( INVALID_SOCKET )
    , isBlocking(false)
    {
        if (type == Type::TCP)
        {
            std::cout << "[Socket::Socket] TCP has not been implemented!" << std::endl;
            return;
        }

        // Initialise WinSock
        WSADATA init;
        const int RESULT = WSAStartup(MAKEWORD(2, 2), &init);
        if (RESULT != 0) 
        {
            std::cout << "[Socket::Socket] Failed to start WinSock with error!" << std::endl;
            return;
        }

        id = count;
        ++count;
    }

    Socket::~Socket()
    {
        WSACleanup();
        --count;
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    uint32_t AddressInfo::GetIPAddressDecimal() const
    {
        return decimalIPv4Address;
    }

    std::string AddressInfo::GetIPAddressString() const
    {
        static constexpr size_t MAX_ADDR_STRLEN = 16;

        char addrStr[16] = {};

        in_addr addr;
        addr.S_un.S_addr = decimalIPv4Address;

        inet_ntop(AF_INET, &addr, addrStr, MAX_ADDR_STRLEN);
        return std::string(addrStr);
    }

    uint16_t AddressInfo::GetPortNumber() const
    {
        return portNumber;
    }

    /*---------------------------------------------------------------------------------*/
    /* Setter Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    void AddressInfo::SetIPAddress(uint32_t ipAddress)
    {
        decimalIPv4Address = ipAddress;
    }

    void AddressInfo::SetIPAddress(const std::string& ipAddress)
    {
        in_addr addr;
        const int RESULT = inet_pton(AF_INET, ipAddress.c_str(), &addr);

        decimalIPv4Address = static_cast<uint32_t>(addr.S_un.S_addr);
    }

    void AddressInfo::SetPortNumber(uint16_t port)
    {
        portNumber = port;
    }

    void Socket::SetIsBlocking(bool value)
    {
        // Set to non blocking
        isBlocking = value;

        ULONG blocking = isBlocking ? 0 : 1;
        ioctlsocket(handle, static_cast<long>(FIONBIO), &blocking);
    }

    /*---------------------------------------------------------------------------------*/
    /* Public Fuction Members                                                          */
    /*---------------------------------------------------------------------------------*/

    sockaddr_in AddressInfo::CreateWindowsSocketAddress() const
    {
        // Prefer zero-initialisation instead of memsock
        sockaddr_in addr = {};

        addr.sin_family             = AF_INET;
        addr.sin_port               = htons(portNumber);
        addr.sin_addr.S_un.S_addr   = decimalIPv4Address;

        return addr;
    }

    void Socket::Create()
    {
        // If socket is already created, ignore
        if (handle != INVALID_SOCKET)
            return;

        // Create socket and check for error
        const int SOCKET_TYPE   = (type == Type::UDP) ? SOCK_DGRAM : SOCK_STREAM;
        const int PROTOCOL      = (type == Type::UDP) ? IPPROTO_UDP : IPPROTO_TCP;

        handle = socket(PF_INET, SOCKET_TYPE, PROTOCOL);
        if (handle == INVALID_SOCKET)
        {
            std::cout << "Socket[" + std::to_string(id) + "] failed to be created!" << std::endl;
        }

        SetIsBlocking(true);

        // Set broadcasting for UDP
        if (type == Type::UDP)
        {
            int yes = 1;    // Not meant to be const
            const int RESULT = setsockopt(handle, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char*>(&yes), sizeof(yes));
            if (RESULT < 0)
            {
                std::cout << "Socket[" + std::to_string(id) + "] failed to enable broadcasting!" + std::to_string(WSAGetLastError()) << std::endl;
            }
        }
        
    }

    bool Socket::IsInvalid()
    {
        return handle == INVALID_SOCKET || id == INVALID_ID;
    }

    void Socket::Close()
    {
        if (handle == INVALID_SOCKET)
            return;

        closesocket(handle);
        handle = INVALID_SOCKET;
    }

    AddressInfo Socket::CreateAddress(uint32_t ipAddress, uint16_t port)
    {
        return AddressInfo(ipAddress, port);
    }

    AddressInfo Socket::CreateAddress(const std::string& ipAddress, uint16_t port)
    {
        return AddressInfo(ipAddress, port);
    }


    Socket::Status Socket::GetErrorStatus()
    {
        const int LAST_ERROR = WSAGetLastError();
        switch (LAST_ERROR)
        {
            case WSAEALREADY:   return Status::NotReady;
            case WSAEISCONN:    return Status::Done;
            default:            return Status::Error;
        }
    }


} // namespace PlushieEngine::Network