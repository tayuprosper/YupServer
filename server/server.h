#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class HttpServer{
    

    // TODO:
    /*
        Fix project folder so that argument passed is name
        of project and consequently the folder the files
        will be served relative to.
    */

    public:
        int PORT = 8080;
        int socket_fd;
        int max_conn = 10;
        fs::path rootDir;
        fs::path canonicalRoot;
        int run();
        int bindSocket();
        int startServer();
        int acceptConn(sockaddr_in& cl_addr);
        void closeConn(int fd);
        HttpServer(const std::string& servedDir);
        void parseData(int fd, std::string& res, std::string& reqBody);
        std::string getPath(std::string data);
        bool _isFileRequest(std::string path);
        std::string sanitizeFilePath(std::string rawPath);
        bool containsTraversal(const std::string& decodedPath);
        fs::path buildFullPath(const std::string& path);
        public:
        enum METHODS {
            GET,
            POST,
            PUT,
            PATCH,
            DELETE,
            HEAD,
            OPTIONS,
            UNKNOWN // Added for unsupported methods
        };
        METHODS getMethod(std::string data);
        std::string decodePath(const std::string& path);
        
    METHODS stringToMethod(const std::string& methodStr);
    private:
        sockaddr_in socketaddr;
        sockaddr_in client_addr;
};

