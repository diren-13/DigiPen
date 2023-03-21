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
#include <thread>
// Project Headers
#include "ClientApplication.h"

// Function Declarations
int ProcessClient(ClientInfo& info);

int main()
{
    // Create a thread so clients can send and receive messages concurrently.
    ClientApplication clientApp;

    // Attach client info to app instance
    ClientInfo clientInfo;
    clientApp.AttachClientInfo(clientInfo);

    try
    {
        clientApp.ServerAddress()   = "127.0.0.1";
        clientApp.PortNumber()      = "16666";

        const bool CONNECTED = clientApp.ConnectToServer();

        // Create thread for client so send and receive messages concurrently.
        std::thread clientThread{ ProcessClient, std::ref(clientInfo) };
    
        // Keep connection open until @quit is sent.
        while (CONNECTED)
        {
            std::getline(std::cin, clientInfo.MessageToSend);
            clientApp.SendMessageToServer(clientInfo.MessageToSend);

            if (clientInfo.MessageToSend == "@quit")
            {
                clientThread.detach();
                break;
            }
        }

        // ClientApp frees on its own
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
        return 0;
    }

}

int ProcessClient(ClientInfo& info)
{
    while (true)
    {
        memset(info.ReceivedMessage, 0, DEFAULT_BUFFER_LENGTH);
     
        if (info.Socket != 0)
        {
            const int I_RESULT = recv(info.Socket, info.ReceivedMessage, DEFAULT_BUFFER_LENGTH, 0);


            if (I_RESULT != SOCKET_ERROR)
            {
                std::cout << info.ReceivedMessage << std::endl;
            }
            else
            {
                if (WSAGetLastError() == WSAECONNRESET)
                {
                    std::cout << "The server has disconnected" << std::endl;
                }
                break;
            }
        }
    }

    return 0;
}