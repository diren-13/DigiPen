
#include <iostream>
#include "UDPSocket.h"

#define SERVER "127.0.0.1"  // or "localhost" - ip address of UDP server
#define PORT    8888        // the port on which to listen for incoming data

void clientLocal();
void clientSample();
void serverLocal();
void serverSample();

constexpr bool      client      = true;
constexpr size_t    bufferLen   = 512LLU;

int main()
{
    if (client)
    {
        //clientSample();
        clientLocal();
    }
    else
    {
        //serverSample();
        serverLocal();
    }

}

void clientLocal()
{
    Network::UDPSocket client;
    client.Create();
    //client.SetIsBlocking(false);

    // Get server address
    Network::AddressInfo serverInfo = Network::Socket::CreateAddress(SERVER, PORT);

    while (true)
    {
        std::string msg;
        std::cout << "Enter Message: ";
        std::getline(std::cin, msg);

        // Check for quit
        if (msg[0] == 'Q' || msg[0] == 'q')
            break;

        // Send message
        Network::Socket::Status status = client.Send(msg.data(), msg.length(), serverInfo.GetIPAddressDecimal(), serverInfo.GetPortNumber());
        if (status != Network::Socket::Status::Done)
        {
            std::cout << "Failed to send message!" << std::endl;
        }

        // Receive message
        char data[bufferLen] = {};

        size_t receivedSize = 0LLU;

        uint32_t serverAddr = 0U;
        uint16_t serverPort = 0;

        status = client.Receive(data, bufferLen, receivedSize, serverAddr, serverPort);
        if (status == Network::Socket::Status::Error || receivedSize == 0)
            continue;

        serverInfo.SetIPAddress(serverAddr);
        serverInfo.SetPortNumber(serverPort);

        data[receivedSize + 1] = '\0';
        std::string recvMsg{ data };
        std::cout << recvMsg << std::endl;
    }

    client.Close();
}

void clientSample()
{
    WSADATA ws;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0)
    {
	    printf("Failed. Error Code: %d", WSAGetLastError());
	    return;
    }
    printf("Initialised.\n");

    // create socket
    sockaddr_in server;
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) // <<< UDP socket
    {
	    printf("socket() failed with error code: %d", WSAGetLastError());
	    return;
    }

    // setup address structure
    memset((char*)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVER);

    // start communication
    while (true)
    {
	    char message[bufferLen];
	    printf("Enter message: ");
	    std::cin.getline(message, bufferLen);

	    // send the message
	    if (sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR)
	    {
		    printf("sendto() failed with error code: %d", WSAGetLastError());
		    return;
	    }

	    // receive a reply and print it
	    // clear the answer by filling null, it might have previously received data
	    char answer[bufferLen] = {};

	    // try to receive some data, this is a blocking call
	    int slen = sizeof(sockaddr_in);
	    int answer_length;
	    if (answer_length = recvfrom(client_socket, answer, bufferLen, 0, (sockaddr*)&server, &slen) == SOCKET_ERROR)
	    {
		    printf("recvfrom() failed with error code: %d", WSAGetLastError());
		    exit(0);
	    }

	    std::cout << answer << "\n";
    }

    closesocket(client_socket);
    WSACleanup();
}

void serverLocal()
{
    Network::UDPSocket server;
    Network::Socket::Status status = server.Bind(INADDR_ANY, PORT);
    //server.SetIsBlocking(false);

    while (true)
    {
        // Receive message
        char data[bufferLen] = {};

        size_t receivedSize = 0LLU;

        uint32_t clientAddr = 0U;
        uint16_t clientPort = 0;

        status = server.Receive(data, bufferLen, receivedSize, clientAddr, clientPort);
        if (receivedSize == 0)
            continue;

        data[receivedSize + 1] = '\0';
        std::string recvMsg{ data };
        std::cout << recvMsg << std::endl;

        // Respond
        Network::AddressInfo clientInfo = Network::Socket::CreateAddress(clientAddr, clientPort);
        std::string respondMsg = "Hello Client!";

        status = server.Send(respondMsg.data(), respondMsg.length(), clientInfo.GetIPAddressString(), clientInfo.GetPortNumber());
        if (status != Network::Socket::Status::Done)
        {
            std::cout << "Failed to send message!" << std::endl;
        }
    }

    server.Unbind();
}

void serverSample()
{
    sockaddr_in server, client;

    // initialise winsock
    WSADATA wsa;
    printf("Initialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
	    printf("Failed. Error Code: %d", WSAGetLastError());
	    exit(0);
    }
    printf("Initialised.\n");

    // create a socket
    SOCKET server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
    {
	    printf("Could not create socket: %d", WSAGetLastError());
    }
    printf("Socket created.\n");

    // prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // bind
    if (bind(server_socket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
	    printf("Bind failed with error code: %d", WSAGetLastError());
	    exit(EXIT_FAILURE);
    }
    puts("Bind done.");

    while (true)
    {
	    printf("Waiting for data...");
	    fflush(stdout);
	    char message[bufferLen] = {};

	    // try to receive some data, this is a blocking call
	    int message_len;
	    int slen = sizeof(sockaddr_in);
	    if (message_len = recvfrom(server_socket, message, bufferLen, 0, (sockaddr*)&client, &slen) == SOCKET_ERROR)
	    {
		    printf("recvfrom() failed with error code: %d", WSAGetLastError());
		    exit(0);
	    }

	    // print details of the client/peer and the data received
	    printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
	    printf("Data: %s\n", message);

	    std::cin.getline(message, bufferLen);

	    // reply the client with 2the same data
	    if (sendto(server_socket, message, strlen(message), 0, (sockaddr*)&client, sizeof(sockaddr_in)) == SOCKET_ERROR)
	    {
		    printf("sendto() failed with error code: %d", WSAGetLastError());
		    return;
	    }
    }

    closesocket(server_socket);
    WSACleanup();
}


