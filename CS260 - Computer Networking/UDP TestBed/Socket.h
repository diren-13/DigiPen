/************************************************************************************//*!
\file           Socket.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           April 14, 2022
\brief          Contains declaration of a generic Socket for networking.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

/// We define LEAN_AND_MEAN to ensure Windows.h does not include WinSock as it conflicts
/// with WinSock2.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Standard Libraries
#include <string>
// Windows Headers
#include <Windows.h>
#include <WinSock2.h>



namespace Network
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Encapsulates the IP address and port number of an address.
    /// </summary>
    class AddressInfo
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        AddressInfo();

        /// <summary>
        /// Constructor for an AddressInfo.
        /// </summary>
        /// <param name="ipAddress">
        /// The IP Address for the AddressInfo to store.
        /// </param>
        /// <param name="port">
        /// The port number for the AddressInfo to store.
        /// </param>
        AddressInfo(uint32_t ipAddress, uint16_t port);
        AddressInfo(const std::string& ipAddress, uint16_t port);

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]] uint32_t      GetIPAddressDecimal()   const;
        [[nodiscard]] std::string   GetIPAddressString()    const;
        [[nodiscard]] uint16_t      GetPortNumber()         const;

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void    SetIPAddress    (uint32_t ipAddress);
        void    SetIPAddress    (const std::string& ipAddress);
        void    SetPortNumber   (uint16_t port);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Creates a socket address struct for Windows.
        /// </summary>
        /// <returns>
        /// A WinSock Socket Address struct.
        /// </returns>
        [[nodiscard]] sockaddr_in CreateWindowsSocketAddress() const;

    private:
        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        uint32_t    decimalIPv4Address;
        uint16_t    portNumber;
    };


    /// <summary>
    /// Encapsulates a generic socket for networking.
    /// </summary>
    class Socket
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Type Definitions                                                            */
        /*-----------------------------------------------------------------------------*/
        // TODO: Add more for TCP if required
        enum class Status
        {
            Done,       // The socket has sent / received the data
            NotReady,   // The socket is not ready to send / receive data yet
            Error       // An unexpected error happened
        };

        enum class Type
        {
            UDP,
            TCP         //! Not implemented
        };

        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Default constructor for Socket
        /// </summary>
        /// <param name="t">
        /// The protocol type of the socket.
        /// </param>
        Socket  (Type t = Type::UDP);
        ~Socket ();

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]]
        uint32_t            GetId()         const   { return id; }
        [[nodiscard]]
        const SOCKET&       GetHandle()     const   { return handle; }
        [[nodiscard]]
        bool                GetIsBlocking() const   { return isBlocking; }

        /*-----------------------------------------------------------------------------*/
        /* Setter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        void SetIsBlocking(bool value);

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Creates the socket.
        /// </summary>
        void        Create();
        /// <summary>
        /// Checks if the socket is invalid
        /// </summary>
        bool        IsInvalid();
        /// <summary>
        /// Closes the socket.
        /// </summary>
        void        Close();

        /// <summary>
        /// Creates a local address.
        /// </summary>
        /// <param name="ipAddress">
        /// The Target IP Address.
        /// </param>
        /// <param name="port">
        /// The Target Port Number.
        /// </param>
        /// <returns>
        /// An object containing the address info for interfacing with WinSock.
        /// </returns>
        [[nodiscard]] static AddressInfo CreateAddress  (uint32_t ipAddress, uint16_t port);
        [[nodiscard]] static AddressInfo CreateAddress  (const std::string& ipAddress, uint16_t port);
        /// <summary>
        /// Gets the error of the last known state of the socket.
        /// </summary>
        /// <returns>
        /// The error status from the last call.
        /// </returns>
        [[nodiscard]]static Status      GetErrorStatus  ();

    protected:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static              uint32_t count;
        static constexpr    uint32_t INVALID_ID = -1;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        Type        type;
        uint32_t    id;             // Identifier for a socket
        SOCKET      handle;
        bool        isBlocking;
    };


} // PlushieEngine::Network

