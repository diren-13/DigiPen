/************************************************************************************//*!
 \file      ClientApplication.h
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

// Windows Headers
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
// Standard Libraries
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LENGTH 512

/*-------------------------------------------------------------------------------------*/
/* Type  Definitions                                                                   */
/*-------------------------------------------------------------------------------------*/

/// <summary>
/// Encapsulate the info for a client.
/// </summary>
struct ClientInfo
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    SOCKET      Socket;
    std::string Name;
    std::string MessageToSend;
    char        ReceivedMessage[DEFAULT_BUFFER_LENGTH];

    /*---------------------------------------------------------------------------------*/
    /* Constructors                                                                    */
    /*---------------------------------------------------------------------------------*/
    ClientInfo();
};

/// <summary>
/// Encapsulates an application for a simple chat client.
/// </summary>
class ClientApplication
{
public:
    /*---------------------------------------------------------------------------------*/
    /* Constructors & Destructor                                                       */
    /*---------------------------------------------------------------------------------*/
    ClientApplication   ();
    ~ClientApplication  ();

    /*---------------------------------------------------------------------------------*/
    /* Getter & Setter FUnctions                                                       */
    /*---------------------------------------------------------------------------------*/
    [[nodiscard]] inline const std::string&     GetServerAddress()  const   { return serverAddress; }
    [[nodiscard]] inline const std::string&     GetPortNumber()     const   { return portNumber; }

    [[nodiscard]] inline std::string&           ServerAddress()             { return serverAddress; }
    [[nodiscard]] inline std::string&           PortNumber()                { return portNumber; }

    /*---------------------------------------------------------------------------------*/
    /* Function Members                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Attaches a ClientInfo struct to the Client Application.
    /// </summary>
    /// <param name="info">
    /// The info to attach.
    /// </param>
    void            AttachClientInfo    (ClientInfo& info);
    /// <summary>
    /// Initialises the winsock application for a Client
    /// </summary>
    /// <returns>
    /// The connection status. True if successful.
    /// </returns>
    bool            ConnectToServer     ();
    /// <summary>
    /// Initialises the winsock application for a Client
    /// </summary>
    /// <param name="msg">
    /// The message to send.
    /// </param>
    void            SendMessageToServer (const std::string& msg) const;
    /// <summary>
    /// Initialises the winsock application for a Client
    /// </summary>
    /// <returns>
    /// The received message.
    /// </returns>
    std::string     GetMessageFromServer() const;

private:
    /*---------------------------------------------------------------------------------*/
    /* Data Members                                                                    */
    /*---------------------------------------------------------------------------------*/
    ClientInfo*         clientInfo;
    std::string         portNumber;
    std::string         serverAddress;
};


