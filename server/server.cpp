#include "server.h"
#include <iostream>
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unistd.h>

HttpServer::HttpServer()
{
    memset(&socketaddr, 0, sizeof(socketaddr));
    memset(&client_addr, 0, sizeof(client_addr));
    socketaddr.sin_family = AF_INET;
    socketaddr.sin_port = htons(HttpServer::PORT);
    socketaddr.sin_addr.s_addr = INADDR_ANY;
}

int HttpServer::run()
{
    std::cout << "creating server instance\n";
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    return socket_fd;
}

int HttpServer::bindSocket()
{
    int res = bind(socket_fd, (struct sockaddr *)&socketaddr, sizeof(socketaddr));
    return res;
}

int HttpServer::startServer()
{
    int res = listen(socket_fd, max_conn);
    return res;
}

int HttpServer::acceptConn(sockaddr_in &cl_addr)
{
    socklen_t cl_len = sizeof(cl_addr);
    int res = accept(socket_fd, (struct sockaddr *)&cl_addr, &cl_len);
    return res;
}

void HttpServer::closeConn(int fd)
{
    close(fd);
}

void HttpServer::parseData(int fd, std::string &res, std::string &reqBody)
{

    std::cout << "Processing Request..." << std::endl;
    const size_t BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    while (true)
    {
        ssize_t bytesRead = read(fd, buffer, BUFFER_SIZE);

        if (bytesRead < 0)
        {
            std::cerr << "read error occured" << strerror(errno) << std::endl;
            break;
        }

        if (bytesRead == 0)
        {
            std::cout << "CLient closed connection (EOF)" << std::endl;
            break;
        }

        res.append(buffer, bytesRead);
        size_t headerEnd = res.find("\r\n\r\n");
        if (headerEnd != std::string::npos)
        {
            size_t bodyExist = res.find("Content-Length:");
            if (bodyExist != std::string::npos)
            {
                std::cout << "Request contains body\n Extracting request body..." << std::endl;

                size_t valueStarts = bodyExist + std::string("Content-Length:").length();

                while (valueStarts < res.size() && res[valueStarts] == ' ')
                {
                    valueStarts++;
                }

                size_t lineEnd = res.find("\r\n", valueStarts);
                std::string lengthStr = res.substr(valueStarts, lineEnd - valueStarts);
                int contentLength = std::stoi(lengthStr);

                std::vector<char> tempReqBody(contentLength);

                size_t bodyRecoveredSizeFromHeaderRead = headerEnd + 4;
                size_t bodySizeAlreadyRead = res.size() - bodyRecoveredSizeFromHeaderRead;

                if (bodySizeAlreadyRead >= contentLength)
                {
                    reqBody.append(res, bodyRecoveredSizeFromHeaderRead, contentLength);
                }
                else
                {
                    reqBody.append(res, bodyRecoveredSizeFromHeaderRead, bodySizeAlreadyRead); // what was already in res
                    ssize_t bodyRead = read(fd, tempReqBody.data(), contentLength - bodySizeAlreadyRead);
                    reqBody.append(tempReqBody.data(), bodyRead); // what read() just got
                }
                res.erase(res.size() - bodySizeAlreadyRead);
                break;
            }
            else
            {
                break;
            }
        }

        memset(buffer, 0, BUFFER_SIZE);
    }
}

std::string HttpServer::getPath(std::string data)
{
    // std::cout << "extracting path..." << std::endl;
    size_t pathLineStart = data.find("/");
    size_t pathLineEnd = data.find("\r\n");
    std::string path;
    if (pathLineStart != std::string::npos && pathLineEnd != std::string::npos)
    {
        std::string line = data.substr(pathLineStart, pathLineEnd);
        std::string actualPath = line.substr(0, line.find(" "));
        if (actualPath.length() == 0)
        {
            std::cerr << "Fatal Error: Malformed request header";
        }
        path = actualPath;
        // std::cout << "extracted path:" << actualPath << std::endl;
        // std::cout << "extracted path returned:" << path << std::endl;
        return path;
    }

    std::cerr << "Failed to find path in request" << std::endl;
    return "";
}


bool HttpServer::_isFileRequest(std::string path){
    return path.find(".") != std::string::npos ? true : false;  
}

std::string HttpServer::_buildFilePath(std::string path){
   
}