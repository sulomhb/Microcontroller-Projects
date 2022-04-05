#include "mbed.h"
#include "json.hpp"


void send_request(TCPSocket *socket, char *request) {
    nsapi_size_t bytes_to_send = strlen(request);
    nsapi_size_or_error_t bytes_sent = 0;

    while (bytes_to_send) {
        bytes_sent = socket->send(request + bytes_sent, bytes_to_send);

        if (bytes_sent < 0) {
            printf("Error");
        } else {
            printf("sent %d bytes\n", bytes_sent);
        }

        bytes_to_send -= bytes_sent;
    }
}


void recieve_request(TCPSocket *socket, char* buffer) {
    int remaining_bytes = 1000;
    int recieved_bytes = 0;
    
    while (remaining_bytes > 0) {
        // mottar 100 bytes av requesten om gangen og printer det ut med ... mellom hver chunk
        nsapi_size_or_error_t result = socket->recv(buffer + recieved_bytes, 100);
        if(result == 0) 
            break;
        if(result < 0) {
            printf("Error");
            break;
        }
        
        printf("%s\n...\n", &buffer[recieved_bytes]);  
        recieved_bytes += result;
        remaining_bytes -= result;
    }
}


int main() 
{
    NetworkInterface *network = NetworkInterface::get_default_instance();

    if (!network) 
    {
        printf("Failed to get default network interface\n");
        while (1);
    }

    printf("Connecting to the network...\n");
    nsapi_size_or_error_t result = network->connect();

    if (result != 0) 
    {
        printf("Failed to connect to network: %d\n", result);
        while (1);
    }


    TCPSocket socket;
    socket.open(network);
    SocketAddress address;

    result = network->gethostbyname("www.mocky.io", &address);

    if (result != 0) 
    {
        printf("Failed to get IP address of host: %d\n", result);
        while (1);
    }

    address.set_port(80);
    result = socket.connect(address);

    if (result != 0) 
    {
        printf("Failed to connect to server: %d\n", result);
        while (1);
    }


    char request[] = "GET /v2/5e37e64b3100004c00d37d03 HTTP/1.1\r\n"
  				     "Host: www.mocky.io\r\n"
					 "Connection: close\r\n"
  					 "\r\n";
    
    send_request(&socket, request);
  
    char buffer[1000] = {0}; // her lagrer vi hele responsen
    recieve_request(&socket, buffer);
    
    socket.close();
    printf("Complete Message:\n%s", buffer);
    

    char *json_start = strchr(buffer, '{'); // her får vi en peker til første { i responsen, som er starten på json-fila
    nlohmann::json j = nlohmann::json::parse(json_start, nullptr, false); // parser jsonen ved hjelp av biblioteket
    
    if (j.is_discarded()) 
    {
        printf("\nError");
    }

    // henter ut de verdiene vi skal printe ut
    std::string fname = j["first name"].get<std::string>(); 
    std::string lname = j["last name"].get<std::string>();
    int age = j["age"].get<int>();
    
    printf("First Name: %s\nLast Name: %s\nAge: %d\n", fname.c_str(), lname.c_str(), age);
    
    return 0;
}