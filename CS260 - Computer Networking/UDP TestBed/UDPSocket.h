/************************************************************************************//*!
\file           UDPSocket.h
\author         Diren D Bharwani, diren.dbharwani, 390002520
\par            email: diren.dbharwani\@digipen.edu
\date           April 14, 2022
\brief          Contains declaration of a UDP Socket for networking.
 
Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent 
of DigiPen Institute of Technology is prohibited.
*//*************************************************************************************/

#pragma once

// Standard Libraries
#include <vector>
// Project Headers
#include "Socket.h"

namespace Network
{
    /*---------------------------------------------------------------------------------*/
    /* Type Definitions                                                                */
    /*---------------------------------------------------------------------------------*/
    /// <summary>
    /// Encapsulates a UDP socket for networking.
    /// </summary>
    class UDPSocket : public Socket
    {
    public:
        /*-----------------------------------------------------------------------------*/
        /* Constructors & Destructor                                                   */
        /*-----------------------------------------------------------------------------*/
        UDPSocket();
        ~UDPSocket()    = default;

        /*-----------------------------------------------------------------------------*/
        /* Getter Functions                                                            */
        /*-----------------------------------------------------------------------------*/
        [[nodiscard]]
        const std::vector<char>& GetBuffer() const;

        /*-----------------------------------------------------------------------------*/
        /* Function Members                                                            */
        /*-----------------------------------------------------------------------------*/
        /// <summary>
        /// Binds the socket to an IP address and a port.
        /// </summary>
        /// <param name="ipAddress">
        /// The IP address to bind the socket to.
        /// </param>
        /// <param name="port">
        /// The port to bind the socket to.
        /// </param>
        /// <returns>
        /// Error if binding fails, Done if success.
        /// </returns>
        [[nodiscard]]
        Status  Bind        (uint32_t ipAddress, uint16_t port);
        /// <summary>
        /// Closes the socket.
        /// </summary>
        void    Unbind      ();
        /// <summary>
        /// Sends data to a given IP address and port. UDP does not guarantee the datagram reaches it's destination.
        /// </summary>
        /// <param name="data">
        /// The raw data to send.
        /// </param>
        /// <param name="size">
        /// The size of the data to send in bytes.
        /// </param>
        /// <param name="targetAddress">
        /// The IP address of the recipient.
        /// </param>
        /// <param name="targetPort">
        /// The port number of the recipient.
        /// </param>
        /// <returns>
        /// Error if failed to send, Done if success.
        /// </returns>
        [[nodiscard]] Status  Send      (const void* data, size_t size, uint32_t targetAddress, uint16_t targetPort);
        [[nodiscard]] Status  Send      (const void* data, size_t size, const std::string& targetAddress, uint16_t targetPort);
        /// <summary>
        /// Receives incoming data from a given IP address and port.
        /// </summary>
        /// <param name="data">
        /// A container of data to receive.
        /// </param>
        /// <param name="size">
        /// The size of the container to hold the received data in bytes.
        /// </param>
        /// <param name="receivedSize">
        /// The size of the data received in bytes.
        /// </param>
        /// <param name="targetAddress">
        /// The IP Address of the sender.
        /// </param>
        /// <param name="targetPort">
        /// The port number of the sender.
        /// </param>
        /// <returns>
        /// Error if failed to receive. Done if success.
        /// </returns>
        [[nodiscard]] Status Receive    (void* data, size_t size, size_t& receivedSize, uint32_t& targetAddress, uint16_t& targetPort);
        [[nodiscard]] Status Receive    (void* data, size_t size, size_t& receivedSize, std::string& targetAddress, uint16_t& targetPort);
        /// <summary>
        /// Gets the local port number of this instance.
        /// </summary>
        /// <returns>
        /// The port number of the attached socket to the instance.
        /// </returns>
        [[nodiscard]] uint16_t  GetLocalPort();


    private:
        /*-----------------------------------------------------------------------------*/
        /* Static Data Members                                                         */
        /*-----------------------------------------------------------------------------*/
        static constexpr size_t MAX_BUFFER_SIZE = 512;

        /*-----------------------------------------------------------------------------*/
        /* Data Members                                                                */
        /*-----------------------------------------------------------------------------*/
        char    buffer[MAX_BUFFER_SIZE];
        size_t  bufferSize;

    };
} // namespace PlushieEngine::Network
