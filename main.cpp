#include "server/server.h"
#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main()
{
    HttpServer app;
    int fd = app.run();

    sockaddr_in cl_addr;
    memset(&cl_addr, 0, sizeof(cl_addr));

    if (fd == -1)
    {
        std::cerr << "Failed to create socket" << strerror(errno) << std::endl;
    }
    int bindSoc = app.bindSocket();
    if (bindSoc == -1)
    {
        std::cerr << "Failed to create socket" << strerror(errno) << std::endl;
    }

    int start = app.startServer();
    if (start == -1)
    {
        std::cerr << "Failed to create socket" << strerror(errno) << std::endl;
    }
    std::cout << "Server started\n"
              << "Listening on Port " << app.PORT << std::endl;
    while (true)
    {
        int cl_con = app.acceptConn(cl_addr);

        if (cl_con == -1){
            std::cerr << "Failed to create socket" << strerror(errno) << std::endl;
        }

        std::cout << "client Connected" << htons(cl_addr.sin_port) << std::endl;
        

        std::string header, body;
        app.parseData(cl_con, header, body);
        std::cout << "full read data: " << std::endl;
        std::cout << header << std::endl;
        std::cout << "body" << body << std::endl;

        std::string extractedPath = app.getPath(header);
        std::cout << "extracted path is: " << extractedPath << std::endl;
        
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\nhello world";
        send(cl_con, response.c_str(), response.length(), 0);
        app.closeConn(cl_con);
    }
    // const buff
    return 0;
}
