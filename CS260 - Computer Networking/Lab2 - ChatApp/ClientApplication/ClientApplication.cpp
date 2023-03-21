/************************************************************************************//*!
 \file      ClientApplication.cpp
 \author    Diren D Bharwani, diren.dbharwani, 390002520
 \par       email: diren.dbharwani\@digipen.edu
 \date      Feb 25, 2022
 \brief     This file contains the implementation of the ClientInfo & ClientApplication
            objects.

 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Primary Header
#include "ClientApplication.h"

#include <iostream>
#include <conio.h>
#include <stdexcept>

#pragma comment(lib, "Ws2_32.lib")

ClientInfo::ClientInfo()
: Socket            { INVALID_SOCKET }
, ReceivedMessage   {}
{}

/// <summary>
/// Initialises the winsock application for a Client
/// </summary>
ClientApplication::ClientApplication()
: clientInfo    { nullptr }
{
    // Initialize Winsock
    WSAData wsaData;
    const int RESULT = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (RESULT != 0) 
    {
        std::cout << "WSAStartup() failed with error: " << RESULT << std::endl;
        return;
    }

    std::cout << "Starting Client..." << std::endl;
}

/// <summary>
/// Shuts down the winsock application for a Client
/// </summary>
ClientApplication::~ClientApplication()
{
    // Shutdown
    std::cout << "Shutting down socket..." << std::endl;
    const int RESULT = shutdown(clientInfo->Socket, SD_SEND);
    if (RESULT == SOCKET_ERROR) 
    {
        std::cout << "shutdown() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientInfo->Socket);
        WSACleanup();
        (void)_getch();
        return;
    }

    // Cleanup
    closesocket(clientInfo->Socket);
    WSACleanup();
    (void)_getch();
}

void ClientApplication::AttachClientInfo(ClientInfo& info)
{
    
    clientInfo = &info;
}


bool ClientApplication::ConnectToServer()
{
    if (clientInfo == nullptr)
        throw std::runtime_error("Cannot Connect Client Application to server without an attached Client!");

    addrinfo*   result  = nullptr;
    addrinfo*   ptr     = nullptr;
    addrinfo    hints;

    if (serverAddress.length() == 0)
    {
        std::cout << "Enter Server IP Address: ";
        std::cin >> serverAddress;
    }

    if (portNumber.length() == 0)
    {
        std::cout << "Enter Server Port Number: ";
        std::cin >> portNumber;
    }

    std::cout << "Connecting..." << std::endl;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family    = AF_UNSPEC;
    hints.ai_socktype  = SOCK_STREAM;
    hints.ai_protocol  = IPPROTO_TCP;

    // Resolve the server address and port
    int iResult = getaddrinfo(serverAddress.c_str(), portNumber.c_str(), &hints, &result);
    if (iResult != 0) 
    {
        std::cout << "getaddrinfo() failed with error: " << iResult << std::endl;
        WSACleanup();
        (void)_getch();
        return false;
    }
     
    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != nullptr; ptr = ptr->ai_next)
    {
        // Create a SOCKET for connecting to server
        clientInfo->Socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (clientInfo->Socket == INVALID_SOCKET) 
        {
            std::cout << "socket() failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            (void)_getch();
            return false;
        }
     
        // Connect to server.
        iResult = connect(clientInfo->Socket, ptr->ai_addr, static_cast<int>(ptr->ai_addrlen));
        if (iResult == SOCKET_ERROR) 
        {
            closesocket(clientInfo->Socket);
            clientInfo->Socket = INVALID_SOCKET;
            continue;
        }
        break;
    }
     
    freeaddrinfo(result);
     
    if (clientInfo->Socket == INVALID_SOCKET) 
    {
        std::cout << "Unable to connect to server!" << std::endl;
        WSACleanup();
        (void)_getch();
        return false;
    }
     
    std::cout << "Successfully Connected" << std::endl;
    return true;
}


void ClientApplication::SendMessageToServer(const std::string& msg) const
{
    if (clientInfo == nullptr)
        throw std::runtime_error("Cannot Connect Send Message to server without an attached Client!");

    clientInfo->MessageToSend = msg;
    const int RESULT = send(clientInfo->Socket, msg.c_str(), static_cast<int>(msg.length()), 0);
    if (RESULT <= 0)
    {
        std::cout << "send() failed: " << WSAGetLastError() << std::endl;
    }
}


std::string ClientApplication::GetMessageFromServer() const
{
    std::string message;
    const int RESULT = recv(clientInfo->Socket, clientInfo->ReceivedMessage, DEFAULT_BUFFER_LENGTH, 0);
    if (RESULT != SOCKET_ERROR)
    {
        message = clientInfo->ReceivedMessage;
    }
    else
    {
        if (WSAGetLastError() == WSAECONNRESET)
        {
            std::cout << "The server has disconnected" << std::endl;
        }
    }

    return message;
}