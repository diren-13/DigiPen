/************************************************************************************//*!
 \file      ServerApplication.h
 \author    Diren D Bharwani, diren.dbharwani, 390002520
 \par       email: diren.dbharwani\@digipen.edu
 \date      Feb 26, 2022
 \brief     The interface for the ClientInfo and ClientApplication objects.
 
 Copyright (C) 2022 DigiPen Institute of Technology.
 Reproduction or disclosure of this file or its contents without the prior written 
 consent of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

/// We define LEAN_AND_MEAN to ensure Windows.h does not include WinSock as it conflicts
/// with WinSock2.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define _WINSOCK_DEPRECATED_NO_WARNINGS

// Windows Headers
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
// Standard Libraries
#include <string>
#include <vector>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define PORT_NUMBER             16666
#define DEFAULT_BUFFER_LENGTH   512

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/// <summary>
/// Encapsulates the info of a client object.
/// </summary>
struct ClientInfo
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    int         Id;
    std::string Name;
    SOCKET      Socket;
    std::thread Thread;

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    ClientInfo(int id, const std::string& name, SOCKET socket, std::thread&& thread);

    
    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Validates the ClientInfo.
    /// </summary>
    /// <returns>
    /// True if the socket is not invalid.
    /// </returns>
    bool        Validate    () const;
    /// <summary>
    /// Send a message to the client.
    /// </summary>
    /// <param name="msg">
    /// The message to send to the client.
    /// </param>
    void        Send        (const std::string& msg) const;
    /// <summary>
    /// Receives a message from the client.
    /// </summary>
    std::string Receive     () const;
};


/// <summary>
/// Encapsulates an application for a simple chat server.
/// </summary>
class ServerApplication
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/

    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    ServerApplication();
    ~ServerApplication();

    /*---------------------------------------------------------------------------------*/
    /* Getter & Setter FUnctions                                                       */
    /*---------------------------------------------------------------------------------*/
    [[nodiscard]] inline const std::vector<ClientInfo>& GetClients()    const   { return clients;  }
    [[nodiscard]] inline std::vector<ClientInfo>&       Clients()               { return clients; }

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Runs the server application. This is meant to be called in a while loop.
    /// </summary>
    /// <returns>
    /// A socket. The socket will be invalid if no new socket was connected.
    /// </returns>
    SOCKET  Run             () const;
    /// <summary>
    /// Prints a message on the server.
    /// </summary>
    /// <param name="msg">
    /// The message to print.
    /// </param>
    void        ServerMessage   (const std::string msg) const;
    /// <summary>
    /// Broadcasts a message to all clients
    /// </summary>
    /// <param name="msg">
    /// The message to send.
    /// </param>
    void        BroadcastMessage(const std::string& msg);
    /// <summary>
    /// Broadcasts a message to all clients except the one passed in.
    /// </summary>
    /// <param name="msg">
    /// The message to send.
    /// </param>
    /// <param name="id">
    /// The client to ignore.
    /// </param>
    void        BroadcastMessage(const std::string& msg, int id);
    /// <summary>
    /// Removes a client from the server.
    /// </summary>
    /// <param name="id">
    /// The id of the client to remove
    /// </param>
    void        RemoveClient    (int id);

private:

    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    SOCKET                      serverSocket;
    std::vector<ClientInfo>     clients;
};