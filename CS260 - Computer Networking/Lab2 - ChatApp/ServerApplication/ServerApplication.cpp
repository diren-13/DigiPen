/************************************************************************************//*!
 \file    ServerApplication.cpp
 \author  Diren D Bharwani, diren.dbharwani, 390002520
 \par     email: diren.dbharwani\@digipen.edu
 \date    Feb 25, 2022
 \brief   This file contains the 'main' function. Program execution begins and ends there.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <iostream>
#include <conio.h>
#include <algorithm>
// Primary Header
#include "ServerApplication.h"

ClientInfo::ClientInfo(int id, const std::string& name, SOCKET socket, std::thread&& thread)
: Id        { id }
, Name      { name }
, Socket    { socket }
, Thread    { std::move(thread) }
{}

bool ClientInfo::Validate() const
{
    return Socket != INVALID_SOCKET;
}

void ClientInfo::Send(const std::string& msg) const
{
    if (Socket == INVALID_SOCKET)
        return;

    const int RESULT = send(Socket, msg.c_str(), static_cast<int>(msg.length()), 0);
    if (RESULT <= 0)
    {
        std::cout << "send() failed: " << WSAGetLastError() << std::endl;
    }
}

std::string ClientInfo::Receive() const
{
    if (Socket == INVALID_SOCKET)
        return std::string{};


    char receivedMsg[DEFAULT_BUFFER_LENGTH];
    const int RESULT = recv(Socket, receivedMsg, DEFAULT_BUFFER_LENGTH, 0);
    if (RESULT != SOCKET_ERROR)
    {
        return std::string{ receivedMsg };
    }
    else
    {
        std::cout << "recv() failed: " << WSAGetLastError() << std::endl;
        return std::string{};
    }
}



ServerApplication::ServerApplication()
: serverSocket  { INVALID_SOCKET }
{
    //Initialize Winsock
    std::cout << "Intializing Winsock..." << std::endl;
    WSADATA wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);

    //Create a listening socket for connecting to server
    std::cout << "Creating server socket..." << std::endl;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Failed to create socket!" << std::endl;
        WSACleanup();
        (void)_getch();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_addr.s_addr  = INADDR_ANY;
    serverAddr.sin_port         = htons(PORT_NUMBER);

     
    //Assign an address to the server socket.
    std::cout << "Binding socket..." << std::endl;
    const int RESULT = bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (RESULT == SOCKET_ERROR) 
    {
        std::cout << "bind() failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        (void)_getch();
        return;
    }

     
    //Listen for incoming connections.
    std::cout << "Listening..." << std::endl;
    listen(serverSocket, SOMAXCONN);
}

ServerApplication::~ServerApplication()
{
    // Close listening socket
    closesocket(serverSocket);

    // Close all client sockets
    for (auto& c : clients)
    {
        closesocket(c.Socket);
    }

    // Clean up
    WSACleanup();
    std::cout << "Program has ended successfully." << std::endl;
    (void)_getch();
}

SOCKET ServerApplication::Run() const
{
    return accept(serverSocket, nullptr, nullptr);
}

void ServerApplication::ServerMessage(const std::string msg) const
{
    std::cout << msg << std::endl;
}


void ServerApplication::BroadcastMessage(const std::string& msg)
{
    for (const auto& c : clients)
    {
        const int RESULT = send(c.Socket, msg.c_str(), static_cast<int>(msg.length()), 0);
        if (RESULT <= 0)
        {
            std::cout << "send() failed: " << WSAGetLastError() << std::endl;
        }
    }
}

void ServerApplication::BroadcastMessage(const std::string& msg, int id)
{
    for (const auto& c : clients)
    {
        if (c.Id == id)
            continue;

        const int RESULT = send(c.Socket, msg.c_str(), static_cast<int>(msg.length()), 0);
        if (RESULT <= 0)
        {
            std::cout << "send() failed: " << WSAGetLastError() << std::endl;
        }
    }
}

void ServerApplication::RemoveClient(int id)
{
    auto removeIt = clients.end();
    for (auto it = clients.begin(); it != clients.end(); ++it)
    {
        if (it->Id == id)
        {
            removeIt = it;
            break;
        }
    }

    if (removeIt == clients.end())
        return;

    closesocket(removeIt->Socket);
    removeIt->Thread.detach();
    clients.erase(removeIt);
}

