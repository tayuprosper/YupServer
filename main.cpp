#include "server/server.h"
#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main(int argc,char* argv[])
{

    std::string servedDir = (argc > 0) ? argv[1] : "./public";
    HttpServer app(servedDir);
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
        if (cl_con == -1)
        {
            std::cerr << "Failed to create socket" << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "client Connected" << htons(cl_addr.sin_port) << std::endl;

        std::string header, body, statusCode = "200 OK", responseBody;
        app.parseData(cl_con, header, body);

        HttpServer::METHODS method = app.getMethod(header);
        std::string extractedPath = app.getPath(header);

        switch (method)
        {
        case HttpServer::POST:
            std::cout << "Handling POST payload: " << body << std::endl;
            responseBody = "Resource Created successfully";
            statusCode = "201 Created";
            break;

        case HttpServer::PUT:
            std::cout << "Handling PUT payload: " << body << std::endl;
            responseBody = "Resource Updated successfully";
            break;

        case HttpServer::GET:
            responseBody = "Hello world\r\n";
            break;

        default:
            statusCode = "405 Method Not Allowed";
            responseBody = "Method not supported";
            break;
        }

        std::string response = "HTTP/1.1 " 
                                + statusCode + "\r\n" 
                                + "Content-Length: " 
                                + std::to_string(responseBody.length()) 
                                + "\r\n\r\n"
                                + responseBody;

        send(cl_con, response.c_str(), response.length(), 0);
        app.closeConn(cl_con);
    }
    return 0;
}
