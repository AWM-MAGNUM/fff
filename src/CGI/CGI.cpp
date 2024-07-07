// #include "CGI.hpp"
// #include <cstring>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/wait.h>
// #include <ctime>
// #include <csignal>
// #include <iostream>

// CGI::CGI(NetworkClient &client) :  envData(nullptr) ,client(client), responseStatus(200) {
// }

// CGI::~CGI() {
//     if (envData) {
//         for (size_t i = 0; envData[i] != nullptr; ++i) {
//             delete[] envData[i];
//         }
//         delete[] envData;
//     }
// }

// CGI::CGI(const CGI& otherCGI) : client(otherCGI.client), responseStatus(otherCGI.responseStatus) {
//     if (otherCGI.envData) {
//         size_t count = 0;
//         while (otherCGI.envData[count]) ++count;
//         envData = new char*[count + 1];
//         for (size_t i = 0; i < count; ++i) {
//             envData[i] = new char[strlen(otherCGI.envData[i]) + 1];
//             strcpy(envData[i], otherCGI.envData[i]);
//         }
//         envData[count] = nullptr;
//     } else {
//         envData = nullptr;
//     }
// }

// CGI& CGI::operator=(const CGI& otherCGI) 
// {
//     if (this != &otherCGI) {
//         for (size_t i = 0; envData && envData[i] != nullptr; ++i) {
//             delete[] envData[i];
//         }
//         delete[] envData;

//         responseStatus = otherCGI.responseStatus;

//         if (otherCGI.envData) {
//             size_t count = 0;
//             while (otherCGI.envData[count]) ++count;
//             envData = new char*[count + 1];
//             for (size_t i = 0; i < count; ++i) {
//                 envData[i] = new char[strlen(otherCGI.envData[i]) + 1];
//                 strcpy(envData[i], otherCGI.envData[i]);
//             }
//             envData[count] = nullptr;
//         } else {
//             envData = nullptr;
//         }
//     }
//     return *this;
// }

// void CGI::initializeEnvData(const std::vector<std::string>& envs) 
// {
//     if (this->envData) {
//         for (size_t i = 0; this->envData[i] != nullptr; ++i) {
//             delete[] this->envData[i];
//         }
//         delete[] this->envData;
//     }

//     this->envData = new char*[envs.size() + 1];
//     size_t i = 0;
//     for (; i < envs.size(); ++i) {
//         this->envData[i] = new char[envs[i].size() + 1];
//         std::strcpy(this->envData[i], envs[i].c_str());
//     }
//     this->envData[i] = nullptr;
// }

// void CGI::configureEnvironment(const std::string& script_name) 
// {
//     std::vector<std::string> envs;
//     std::map<std::string, std::string> &headers = this->client.getRequest().getHeaderFields();
//     HttpRequest &request = this->client.getRequest();

//     envs.push_back("CONTENT_TYPE=" + headers["Content-Type"]);
//     envs.push_back("REDIRECT_STATUS=200");
//     envs.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
//     envs.push_back("HTTP_COOKIE=" + headers["Cookie"]);
//     envs.push_back("HTTP_USER_AGENT=" + headers["User-Agent"]);
//     envs.push_back("PATH_INFO=");
//     envs.push_back("QUERY_STRING=" + request.get_queryString());
//     envs.push_back("REMOTE_ADDR=");
//     envs.push_back("REMOTE_HOST=");
//     envs.push_back("REQUEST_METHOD=" + request.getMethod());
//     envs.push_back("SCRIPT_NAME=" + script_name);
//     envs.push_back("SERVER_NAME=" + headers["Host"]);
//     envs.push_back("SERVER_SOFTWARE=HTTP/1.1");
//     envs.push_back("SCRIPT_FILENAME=" + std::string("/Users/user/Desktop/roro/src") + request.getUri());
//     envs.push_back("REQUEST_URI=" + request.getUri());

//     this->initializeEnvData(envs);
// }


// char** CGI::get_CGIenvironmentVariables() {
//     return this->envData;
// }

// bool CGI::initializePipes(int pipeIn[2], int pipeOut[2]) {
//     if (pipe(pipeOut) == -1) {
//         responseStatus = 500;
//         return false;
//     }
//     if (client.getRequest().getMethod() == "POST") {
//         if (pipe(pipeIn) == -1) {
//             responseStatus = 500;
//             close(pipeOut[0]);
//             close(pipeOut[1]);
//             return false;
//         }
//     }
//     return true;
// }

// void CGI::executeChildProcess(const char* scriptPath, char* args[], int pipeIn[2], int pipeOut[2]) {
//     (void)scriptPath;
//     if (client.getRequest().getMethod() == "POST") {
//         close(pipeIn[1]);
//         dup2(pipeIn[0], STDIN_FILENO);
//         close(pipeIn[0]);
//     }

//     close(pipeOut[0]);
//     dup2(pipeOut[1], STDOUT_FILENO);
//     close(pipeOut[1]);

//     execve(args[0], args, this->envData);

//     std::string errorMessage = "Content-Type: text/html\r\n\r\n<html><body style='text-align:center;'><h1>500 Internal Error</h1></body></html>";
//     write(STDOUT_FILENO, errorMessage.c_str(), errorMessage.size());
//     std::exit(EXIT_FAILURE);
// }

// void CGI::handleParentProcess(pid_t processId, int pipeIn[2], int pipeOut[2], const std::string& /*scriptArgument*/) {
//     if (client.getRequest().getMethod() == "POST") {
//         close(pipeIn[0]);
//         int inputFileDescriptor = open(client.getRequest().get_bodyFileName().c_str(), O_RDONLY);
//         if (inputFileDescriptor == -1) {
//             responseStatus = 500;
//             std::cerr << "Failed to open input file: " << strerror(errno) << std::endl;
//             std::exit(EXIT_FAILURE);
//         }
//         char buffer[1024];
//         ssize_t bytesRead;
//         while ((bytesRead = read(inputFileDescriptor, buffer, sizeof(buffer))) > 0) {
//             write(pipeIn[1], buffer, bytesRead);
//         }
//         close(inputFileDescriptor);
//         close(pipeIn[1]);
//     }

//     close(pipeOut[1]);

//     int processStatus = 200;
//     time_t startTime = time(NULL);
//     while (time(NULL) - startTime < 5) {
//         if (waitpid(processId, NULL, WNOHANG) == processId) {
//             processStatus = -2;
//             break;
//         }
//     }
//     if (processStatus != -2 && waitpid(processId, NULL, WNOHANG) != processId) {
//         responseStatus = 504;
//         kill(processId, SIGKILL);
//     }

//     char buffer[1024];
//     ssize_t bytesRead;
//     std::string response;

//     while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
//         buffer[bytesRead] = '\0';
//         response += buffer;
//     }

//     close(pipeOut[0]);

//     // Separate headers and body
//     size_t headerEndPos = response.find("\r\n\r\n");
//     if (headerEndPos != std::string::npos) {
//         std::string headers = response.substr(0, headerEndPos + 4);
//         std::string body = response.substr(headerEndPos + 4);

//         // Calculate content length and update headers if needed
//         size_t contentLength = body.size();
//         if (headers.find("Content-Length:") == std::string::npos) {
//             headers += "Content-Length: " + std::to_string(contentLength) + "\r\n";
//         }

//         headers += "\r\n"; // Ensure there's a blank line between headers and body

//         // Prepend the status line to the headers
//         std::string statusLine = "HTTP/1.1 200 OK\r\n";
//         headers = statusLine + headers;

//         // Log the response for debugging
//         std::cerr << "CGI Response Headers: " << headers << std::endl;
//         std::cerr << "CGI Response Body: " << body << std::endl;

//         // Send headers and body together
//         std::string fullResponse = headers + body;
//         client.setResponse(fullResponse.c_str());
//     } else {
//         std::cerr << "Invalid CGI response: no headers found" << std::endl;
//         client.setResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1></body></html>");
//     }
// }





// void CGI::executeScript() {
//     pid_t processId;
//     int pipeIn[2] = {-1, -1}, pipeOut[2] = {-1, -1};

//     std::string scriptPath = client.getRequest().getUri();
//     if (scriptPath[0] == '/')
//         scriptPath.erase(0, 1); // remove leading slash if present
//     scriptPath = "./src/" + scriptPath;
//     char* scriptArgument = new char[scriptPath.size() + 1];
//     std::strcpy(scriptArgument, scriptPath.c_str());

//     char *arguments[] = {scriptArgument, nullptr};

//     if (!initializePipes(pipeIn, pipeOut)) {
//         delete[] scriptArgument;
//         std::exit(EXIT_FAILURE);
//     }

//     processId = fork();
//     if (processId == -1) {
//         std::string errorMessage = "Content-Type: text/html\r\n\r\n<html><body style='text-align:center;'><h1>500 Internal Error</h1></body></html>";
//         write(STDOUT_FILENO, errorMessage.c_str(), errorMessage.size());
//         delete[] scriptArgument;
//         std::exit(EXIT_FAILURE);
//     } else if (processId == 0) {
//         executeChildProcess(scriptPath.c_str(), arguments, pipeIn, pipeOut);
//     } else {
//         handleParentProcess(processId, pipeIn, pipeOut, scriptPath);
//         delete[] scriptArgument;
//     }
// }

#include "CGI.hpp"
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctime>
#include <csignal>
#include <iostream>

CGI::CGI(NetworkClient &client) : envData(nullptr), client(client), responseStatus(200) {
}

CGI::~CGI() {
    if (envData) {
        for (size_t i = 0; envData[i] != nullptr; ++i) {
            delete[] envData[i];
        }
        delete[] envData;
    }
}

CGI::CGI(const CGI& otherCGI) : client(otherCGI.client), responseStatus(otherCGI.responseStatus) {
    if (otherCGI.envData) {
        size_t count = 0;
        while (otherCGI.envData[count]) ++count;
        envData = new char*[count + 1];
        for (size_t i = 0; i < count; ++i) {
            envData[i] = new char[strlen(otherCGI.envData[i]) + 1];
            strcpy(envData[i], otherCGI.envData[i]);
        }
        envData[count] = nullptr;
    } else {
        envData = nullptr;
    }
}

CGI& CGI::operator=(const CGI& otherCGI) {
    if (this != &otherCGI) {
        for (size_t i = 0; envData && envData[i] != nullptr; ++i) {
            delete[] envData[i];
        }
        delete[] envData;

        responseStatus = otherCGI.responseStatus;

        if (otherCGI.envData) {
            size_t count = 0;
            while (otherCGI.envData[count]) ++count;
            envData = new char*[count + 1];
            for (size_t i = 0; i < count; ++i) {
                envData[i] = new char[strlen(otherCGI.envData[i]) + 1];
                strcpy(envData[i], otherCGI.envData[i]);
            }
            envData[count] = nullptr;
        } else {
            envData = nullptr;
        }
    }
    return *this;
}

void CGI::initializeEnvData(const std::vector<std::string>& envs) {
    if (this->envData) {
        for (size_t i = 0; this->envData[i] != nullptr; ++i) {
            delete[] this->envData[i];
        }
        delete[] this->envData;
    }

    this->envData = new char*[envs.size() + 1];
    size_t i = 0;
    for (; i < envs.size(); ++i) {
        this->envData[i] = new char[envs[i].size() + 1];
        std::strcpy(this->envData[i], envs[i].c_str());
    }
    this->envData[i] = nullptr;
}

void CGI::configureEnvironment(const std::string& script_name) {
    std::vector<std::string> envs;
    std::map<std::string, std::string> &headers = this->client.getRequest().getHeaderFields();
    HttpRequest &request = this->client.getRequest();

    envs.push_back("CONTENT_TYPE=" + headers["Content-Type"]);
    envs.push_back("REDIRECT_STATUS=200");
    envs.push_back("CONTENT_LENGTH=" + headers["Content-Length"]);
    envs.push_back("HTTP_COOKIE=" + headers["Cookie"]);
    envs.push_back("HTTP_USER_AGENT=" + headers["User-Agent"]);
    envs.push_back("PATH_INFO=");
    envs.push_back("QUERY_STRING=" + request.get_queryString());
    envs.push_back("REMOTE_ADDR=");
    envs.push_back("REMOTE_HOST=");
    envs.push_back("REQUEST_METHOD=" + request.getMethod());
    envs.push_back("SCRIPT_NAME=" + script_name);
    envs.push_back("SERVER_NAME=" + headers["Host"]);
    envs.push_back("SERVER_SOFTWARE=HTTP/1.1");
    envs.push_back("SCRIPT_FILENAME=" + std::string("/Users/user/Desktop/roro/src") + request.getUri());
    envs.push_back("REQUEST_URI=" + request.getUri());

    this->initializeEnvData(envs);
}

char** CGI::get_CGIenvironmentVariables() {
    return this->envData;
}

bool CGI::initializePipes(int pipeIn[2], int pipeOut[2]) {
    if (pipe(pipeOut) == -1) {
        responseStatus = 500;
        return false;
    }
    if (client.getRequest().getMethod() == "POST") {
        if (pipe(pipeIn) == -1) {
            responseStatus = 500;
            close(pipeOut[0]);
            close(pipeOut[1]);
            return false;
        }
    }
    return true;
}

void CGI::executeChildProcess(const char* scriptPath, char* args[], int pipeIn[2], int pipeOut[2]) {
    (void)scriptPath;
    if (client.getRequest().getMethod() == "POST") {
        close(pipeIn[1]);
        dup2(pipeIn[0], STDIN_FILENO);
        close(pipeIn[0]);
    }

    close(pipeOut[0]);
    dup2(pipeOut[1], STDOUT_FILENO);
    close(pipeOut[1]);

    execve(args[0], args, this->envData);

    std::string errorMessage = "Content-Type: text/html\r\n\r\n<html><body style='text-align:center;'><h1>500 Internal Error</h1></body></html>";
    write(STDOUT_FILENO, errorMessage.c_str(), errorMessage.size());
    std::exit(EXIT_FAILURE);
}

void CGI::handleParentProcess(pid_t processId, int pipeIn[2], int pipeOut[2], const std::string& /*scriptArgument*/) {
    if (client.getRequest().getMethod() == "POST") {
        close(pipeIn[0]);
        int inputFileDescriptor = open(client.getRequest().get_bodyFileName().c_str(), O_RDONLY);
        if (inputFileDescriptor == -1) {
            responseStatus = 500;
            std::cerr << "Failed to open input file: " << strerror(errno) << std::endl;
            std::exit(EXIT_FAILURE);
        }
        char buffer[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(inputFileDescriptor, buffer, sizeof(buffer))) > 0) {
            write(pipeIn[1], buffer, bytesRead);
        }
        close(inputFileDescriptor);
        close(pipeIn[1]);
    }

    close(pipeOut[1]);

    int processStatus = 200;
    time_t startTime = time(NULL);
    while (time(NULL) - startTime < 5) {
        if (waitpid(processId, NULL, WNOHANG) == processId) {
            processStatus = -2;
            break;
        }
    }
    if (processStatus != -2 && waitpid(processId, NULL, WNOHANG) != processId) {
        responseStatus = 504;
        kill(processId, SIGKILL);
    }

    char buffer[1024];
    ssize_t bytesRead;
    std::string response;

    while ((bytesRead = read(pipeOut[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        response += buffer;
    }

    close(pipeOut[0]);

    // Separate headers and body
    size_t headerEndPos = response.find("\r\n\r\n");
    if (headerEndPos != std::string::npos) {
        std::string headers = response.substr(0, headerEndPos + 4);
        std::string body = response.substr(headerEndPos + 4);

        // Calculate content length and update headers if needed
        size_t contentLength = body.size();
        if (headers.find("Content-Length:") == std::string::npos) {
            headers += "Content-Length: " + std::to_string(contentLength) + "\r\n";
        }

        headers += "\r\n"; // Ensure there's a blank line between headers and body

        // Prepend the status line to the headers
        std::string statusLine = "HTTP/1.1 200 OK\r\n";
        headers = statusLine + headers;

        // Log the response for debugging
        std::cerr << "CGI Response Headers: " << headers << std::endl;
        std::cerr << "CGI Response Body: " << body << std::endl;

        // Send headers and body together
        std::string fullResponse = headers + body;
        client.setResponse(fullResponse.c_str());
    } else {
        std::cerr << "Invalid CGI response: no headers found" << std::endl;
        client.setResponse("HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>500 Internal Server Error</h1></body></html>");
    }
}

void CGI::executeScript() {
    pid_t processId;
    int pipeIn[2] = {-1, -1}, pipeOut[2] = {-1, -1};

    std::string scriptPath = client.getRequest().getUri();
    if (scriptPath[0] == '/')
        scriptPath.erase(0, 1); // remove leading slash if present
    scriptPath = "./src/" + scriptPath;
    char* scriptArgument = new char[scriptPath.size() + 1];
    std::strcpy(scriptArgument, scriptPath.c_str());

    char *arguments[] = {scriptArgument, nullptr};

    if (!initializePipes(pipeIn, pipeOut)) {
        delete[] scriptArgument;
        std::exit(EXIT_FAILURE);
    }

    processId = fork();
    if (processId == -1) {
        std::string errorMessage = "Content-Type: text/html\r\n\r\n<html><body style='text-align:center;'><h1>500 Internal Error</h1></body></html>";
        write(STDOUT_FILENO, errorMessage.c_str(), errorMessage.size());
        delete[] scriptArgument;
        std::exit(EXIT_FAILURE);
    } else if (processId == 0) {
        executeChildProcess(scriptPath.c_str(), arguments, pipeIn, pipeOut);
    } else {
        handleParentProcess(processId, pipeIn, pipeOut, scriptPath);
        delete[] scriptArgument;
    }
}
