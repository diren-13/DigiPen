/************************************************************************************//*!
 \file    main.cpp
 \author  Diren D Bharwani, diren.dbharwani, 390002520
 \par     email: diren.dbharwani\@digipen.edu
 \date    Feb 26, 2022
 \brief   This file contains the 'main' function. Program execution begins and ends there.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

// Standard Libraries
#include <iostream>
// Project Headers
#include "ServerApplication.h"


// Function Declarations
void        ProcessClient       (ServerApplication& app, SOCKET socket, int id);
int         ReceiveFromSocket   (SOCKET socket, std::string& msg);
int         SendToSocket        (SOCKET socket, const std::string& msg);

int main()
{
    ServerApplication server;

    while (true)
    {
        SOCKET newClient = server.Run();

        if (newClient == INVALID_SOCKET)
            continue;

        int newId = static_cast<int>(server.GetClients().size());
        std::thread newClientThread{ &ProcessClient, std::ref(server), std::ref(newClient), std::ref(newId) };
        server.Clients().emplace_back(ClientInfo{ newId, std::string{}, newClient, std::move(newClientThread) });
    }

    // Automatically freed due to RAII.
}

void ProcessClient(ServerApplication& app, SOCKET socket, int id)
{
    std::string clientName;
    std::string msgToSend;
    std::string receivedMsg;

    bool    connected   = false;
    int     result      = 0;

    while (true)
    {
        if (socket != 0)
        {
            while (!connected)
            {
                msgToSend = "Enter a username: ";
                if (SendToSocket(socket, msgToSend) == SOCKET_ERROR)
                    break;
                
                if (ReceiveFromSocket(socket, receivedMsg) == SOCKET_ERROR)
                    break;

                // Check for duplicate name in the list of clients
                bool isNameValid = true;
                for (const auto& c : app.GetClients())
                {
                    if (c.Name == receivedMsg)
                    {
                        isNameValid = false;
                        break;
                    }
                }

                if (!isNameValid)
                {
                    msgToSend = "[Username has already been used. Please enter another name.]";
                    if (SendToSocket(socket, msgToSend) == SOCKET_ERROR)
                        break;

                    continue;
                }

                // Welcome message for client
                msgToSend = "[Welcome " + receivedMsg + "!]";
                SendToSocket(socket, msgToSend);

                // Broadcast to everyone a new user has joined
                msgToSend = "[" + receivedMsg + " joined]";
                app.ServerMessage(msgToSend);
                app.BroadcastMessage(msgToSend, id);

                clientName = receivedMsg;
                app.Clients()[id].Name = clientName;
                connected = true;
                break;
            }
            
            result = ReceiveFromSocket(socket, receivedMsg);
            if (result == SOCKET_ERROR)
            {
                app.RemoveClient(id);

                // Broadcast the disconnection to the rest.
                std::string msg = "[" + clientName + " exited]";
                app.ServerMessage(msg);
                app.BroadcastMessage(msg);

                break;
            }

            if (receivedMsg.length() == 0)
                continue;

            // Handle @quit
            if (receivedMsg == "@quit")
            {
                app.RemoveClient(id);

                // Broadcast the disconnection to the rest.
                msgToSend = "[" + clientName + " exited]";
                app.ServerMessage(msgToSend);
                app.BroadcastMessage(msgToSend);

                break;
            }

            // Handle @name & generic messages
            if (receivedMsg == "@names")
            {
                msgToSend = "[Connected users: ";
                for (size_t i = 0; i < app.GetClients().size(); ++i)
                {
                    msgToSend.append(app.GetClients()[i].Name);

                    if (i + 1 != app.GetClients().size())
                    {
                        msgToSend.append(", ");
                    }
                }
                msgToSend.append("]");

                SendToSocket(socket, msgToSend);
            }
            else
            {
                msgToSend = "[" + clientName + ":] " + receivedMsg;
                std::cout << msgToSend << std::endl;

                // Broadcast message to all other clients
                app.BroadcastMessage(msgToSend, id);
            }
        }
    }
}

int ReceiveFromSocket(SOCKET socket, std::string& msg)
{
    char temp[DEFAULT_BUFFER_LENGTH];
    memset(temp, 0, DEFAULT_BUFFER_LENGTH);

    const int RESULT = recv(socket, temp, DEFAULT_BUFFER_LENGTH, 0);
    if (RESULT <= 0)
    {
        std::cout << "recv() failed: " << WSAGetLastError() << std::endl;
    }

    msg = temp;
    return RESULT;
}

int SendToSocket(SOCKET socket, const std::string& msg)
{
    const int RESULT = send(socket, msg.c_str(), static_cast<int>(msg.length()), 0);
    if (RESULT <= 0)
    {
        std::cout << "send() failed: " << WSAGetLastError() << std::endl;
    }
    return RESULT;
}
