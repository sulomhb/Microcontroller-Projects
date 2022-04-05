#include "mbed.h"
#include "DevI2C.h"
#include "HTS221Sensor.h"
#include <cstring>

#define THINGSBOARD_DEVICE_ID "wknP3WOGiaW1LNGafvEV"
#define HOST_IP "192.168.10.198"
#define HTTP_REQUEST_BUFFER_SIZE 300
#define JSON_CONTENT_BUFFER_SIZE 100
#define HTTP_RESPONSE_BUFFER_SIZE 400;

DevI2C i2c(PB_11, PB_10);
HTS221Sensor hts221(&i2c);


bool send_request(TCPSocket *socket, char *request) 
{
    nsapi_size_t bytes_to_send = strlen(request);
    nsapi_size_or_error_t bytes_sent = 0;

    while (bytes_to_send) {
        bytes_sent = socket->send(request + bytes_sent, bytes_to_send);

        if (bytes_sent < 0) {
            printf("Error: %d\n", bytes_sent);
            return false;
        } else {
            printf("sent %d bytes\n", bytes_sent);
        }

        bytes_to_send -= bytes_sent;
    }
    return true;
}


void recieve_request(TCPSocket *socket, char* buffer) 
{
    int remaining_bytes = HTTP_RESPONSE_BUFFER_SIZE;
    int recieved_bytes = 0;
    
    while (remaining_bytes > 0) {
        nsapi_size_or_error_t result = socket->recv(buffer + recieved_bytes, remaining_bytes);
        if(result == 0) 
            break;
        if(result < 0) {
            printf("Error");
            break;
        }
        
        recieved_bytes += result;
        remaining_bytes -= result;
    }
    printf("%s", buffer);
}


int main()
{
   NetworkInterface *network = NetworkInterface::get_default_instance();

    if (!network) 
    {
        printf("Failed to get default network interface\n");
    }

    printf("Connecting to the network...\n");
    nsapi_size_or_error_t result = network->connect();

    if (result != 0) 
    {
        printf("Failed to connect to network: %d\n", result);
    }


    TCPSocket socket;
    SocketAddress address;
    
    if (!address.set_ip_address(HOST_IP)) 
    {
        printf("Failed to get IP address of host\n");
    }

    address.set_port(9090);
    
    /* Initialiserer sensor*/
    if (hts221.init(NULL) != 0) printf("Failed to initialize");
    if (hts221.enable() != 0) printf("Failed to power up");


    while(true)
    {
        float hum;
        float temp;
        /* Henter ut temperature og humidity */
        hts221.get_humidity(&hum);
        hts221.get_temperature(&temp);

        printf("Temperature: %.1f | Humidity: %.1f\n", temp, hum);

        char json[JSON_CONTENT_BUFFER_SIZE];
        snprintf(json, JSON_CONTENT_BUFFER_SIZE, "{\"temperature\": %.1f,\"humidity\": %.1f}", temp, hum);
        
        char post[HTTP_REQUEST_BUFFER_SIZE];
        snprintf(post, HTTP_REQUEST_BUFFER_SIZE,
                "POST /api/v1/%s/telemetry HTTP/1.1\r\n"
                "Host: %s\r\n"
                "Connection: close\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: %u\r\n"
                "\r\n",
                THINGSBOARD_DEVICE_ID, HOST_IP, strlen(json));

        strcat(post, json);

        socket.open(network);
        result = socket.connect(address);
        
        if (result != 0) 
        {
            printf("Failed to connect to server: %d\n", result);
        }

        send_request(&socket, post);
        
        char buffer[1000] = {0};
        recieve_request(&socket, buffer);
        
        socket.close();
        thread_sleep_for(1000);
    }
}
