#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>

class HttpServer{
    
    public:
        int PORT = 8080;
        int socket_fd;
        int max_conn;
        int run();
        int bindSocket();
        int startServer();
        int acceptConn(sockaddr_in& cl_addr);
        void closeConn(int fd);
        HttpServer();
        void parseData(int fd, std::string& res, std::string& reqBody);
        std::string getPath(std::string data);
        bool _isFileRequest(std::string path);
        std::string _buildFilePath(std::string rawPath);
    private:
        sockaddr_in socketaddr;
        sockaddr_in client_addr;
        enum METHODS {
            GET,
            POST,
            PUT,
            PATCH,
            DELETE,
            HEAD,
            OPTIONS
        };
};