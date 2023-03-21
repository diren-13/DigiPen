/************************************************************************************//*!
\file           UDPSocket.cpp
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           April 14, 2022
\brief          Contains the implemtation of a UDP Socket for networking.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <algorithm>
#include <iterator>
#include <iostream>
// Primary Header
#include "UDPSocket.h"

namespace Network
{
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/

    UDPSocket::UDPSocket()
    : Socket    (Type::UDP)
    , buffer    ()
    , bufferSize(0LLU)
    {
        memset(buffer, 0, MAX_BUFFER_SIZE);
    }

    /*---------------------------------------------------------------------------------*/
    /* Getter Functions                                                                */
    /*---------------------------------------------------------------------------------*/

    const std::vector<char>& UDPSocket::GetBuffer() const
    {
        static std::vector<char> userBuffer(MAX_BUFFER_SIZE);
        userBuffer.clear();

        memcpy_s(userBuffer.data(), bufferSize, buffer, bufferSize);
        return userBuffer;
    }


    /*---------------------------------------------------------------------------------*/
    /* Public Function Members                                                         */
    /*---------------------------------------------------------------------------------*/

    Socket::Status UDPSocket::Bind(uint32_t ipAddress, uint16_t port)
    {
        // If socket is already bound, close and recreate
        Close();
        Create();

        sockaddr_in addr = CreateAddress(ipAddress, port).CreateWindowsSocketAddress();
        const int RESULT = bind(handle, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        if (RESULT < 0)
        {
            std::cout << "Socket[" + std::to_string(id) + "] Cannot be bound!" << std::endl;
            return Status::Error;
        }

        return Status::Done;
    }

    void UDPSocket::Unbind()
    {
        Close();
    }

    Socket::Status UDPSocket::Send(const void* data, size_t size, uint32_t targetAddress, uint16_t targetPort)
    {
        // If socket doesn't exist, create one
        Create();

        // Assert that all the data will fit in one datagram
        if (size > MAX_BUFFER_SIZE)
        {
            std::cout << "Socket[" + std::to_string(id) + "] cannot send a datagram over " + std::to_string(MAX_BUFFER_SIZE) + " bytes" << std::endl;

            return Status::Error;
        }

        // Build target address
        sockaddr_in addr = CreateAddress(targetAddress, targetPort).CreateWindowsSocketAddress();

        // Send data to target
        const int RESULT = sendto
        (
            handle, reinterpret_cast<const char*>(data), static_cast<int>(size), 0, 
            reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)
        );
        if (RESULT < 0)
        {
            std::cout << "Socket[" + std::to_string(id) + "] failed send a datagram!" << std::endl;

            return GetErrorStatus();
        }

        // Copy data to local buffer
        memset(buffer, 0, MAX_BUFFER_SIZE);
        const char* charData = reinterpret_cast<const char*>(data);
        memcpy_s(buffer, size, charData, size);
        bufferSize = size;

        return Status::Done;
    }

    Socket::Status UDPSocket::Send(const void* data, size_t size, const std::string& targetAddress, uint16_t targetPort)
    {
        AddressInfo tmp = CreateAddress(targetAddress, targetPort);
        return Send(data, size, tmp.GetIPAddressDecimal(), targetPort);
    }


    Socket::Status UDPSocket::Receive(void* data, size_t size, size_t& receivedSize, uint32_t& targetAddress, uint16_t& targetPort)
    {
        receivedSize    = 0LLU;
        targetAddress   = 0U;
        targetPort      = 0;

        if (!data)
        {
            std::cout << "Socket[" + std::to_string(id) + "] is attempting to receive data into an invalid buffer!" << std::endl;

            return Status::Error;
        }

        sockaddr_in addr = CreateAddress(INADDR_ANY, 0).CreateWindowsSocketAddress();
        int addrSize = sizeof(addr);

        const int RECEIVED_SIZE = recvfrom
        (
            handle, reinterpret_cast<char*>(data), static_cast<int>(size), 0,
            reinterpret_cast<sockaddr*>(&addr), &addrSize
        );

        if (RECEIVED_SIZE < 0)
            return GetErrorStatus();

        receivedSize    = static_cast<size_t>(RECEIVED_SIZE) < MAX_BUFFER_SIZE ? static_cast<size_t>(RECEIVED_SIZE) : MAX_BUFFER_SIZE;
        bufferSize      = receivedSize;
        targetAddress   = addr.sin_addr.S_un.S_addr;
        targetPort      = ntohs(addr.sin_port);

        // Copy received data to local buffer
        memset(buffer, 0, MAX_BUFFER_SIZE);
        const char* charData = reinterpret_cast<const char*>(data);
        memcpy_s(buffer, bufferSize, charData, receivedSize);

        return Status::Done;
    }

    Socket::Status UDPSocket::Receive(void* data, size_t size, size_t& receivedSize, std::string& targetAddress, uint16_t& targetPort)
    {
        uint32_t tgtAddress = 0U;
        Status status = Receive(data, size, receivedSize, tgtAddress, targetPort);

        targetAddress = CreateAddress(tgtAddress, targetPort).GetIPAddressString();
        return status;
    }


    uint16_t UDPSocket::GetLocalPort()
    {
        sockaddr_in addr;
        int addrSize = sizeof(addr);
        const int RESULT = getsockname(handle, reinterpret_cast<sockaddr*>(&addr), &addrSize);
        if (RESULT < 0)
            return 0;

        return ntohs(addr.sin_port);
    }


} // namespace PlushieEngine::Network