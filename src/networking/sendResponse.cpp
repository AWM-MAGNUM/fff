#include "WebServer.hpp"

// void WebServer::sendResponse(HttpRequest &req, NetworkClient &client) 
// { 
//     static int requestCounter = 0;
//     requestCounter++;
//     std::cout << "req count == "<< requestCounter << std::endl;

//     HttpResponse resp(client);

//     resp.generateResponse(req);

//     if (!client.getHeaderSent()) 
//     {
//         client.setResponse(client.getResponseHeader());
//         client.setHeaderSent(true);
//     }
//     else 
//     {
//         if (!client.getResponseBody().empty()) 
//         {
//             char buffer[1024];
//             if (resp.isText() == true) {
//                 ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
//                 if (bytesSent < 0 || bytesSent == (int)client.getResponseBody().length()) {
//                     std::remove(req.get_bodyFileName().c_str());
//                     closeClient(client.fetchConnectionSocket());
//                 }
//                 //client.setResponseComplete(true); // Mark response as complete
//                 return;
//             } 
//             else if (!client.getOpenFile())
//             {
//                 client.openFileForReading();
//                 if (!client.getOpenFile())
//                 {
//                     std::cerr << "Failed to open file: " << client.getResponseBody() << std::endl;
//                     closeClient(client.fetchConnectionSocket());
//                     return;
//                 }
//             }
//             if (client._file.good()) 
//             {
//                 client.readFromFile(buffer, 1024);
//                 std::size_t bytesRead = static_cast<std::size_t>(client.bytesRead());
//                 client.setResponse(std::string(buffer, bytesRead));
//             }
//             else 
//             {
//                 closeClient(client.fetchConnectionSocket());
//                 return;
//             }
//         }
//     }

//     ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
//     if (bytesSent <= 0) {
//         closeClient(client.fetchConnectionSocket());
//     }
// }

// // void WebServer::sendResponse(HttpRequest &req, NetworkClient &client) 
// // { 
// //     static int requestCounter = 0;
// //     requestCounter++;

// //     HttpResponse resp(client);
// //     resp.generateResponse(req);

// //     if (!client.getHeaderSent()) 
// //     {
// //         // Send header and body together if possible
// //         std::string response = client.getResponseHeader();
// //         if (!client.getResponseBody().empty()) {
// //             response += client.getResponseBody();
// //         }

// //         ssize_t bytesSent = send(client.fetchConnectionSocket(), response.c_str(), response.length(), 0);
// //         if (bytesSent <= 0) {
// //             closeClient(client.fetchConnectionSocket());
// //             return;
// //         }

// //         client.setHeaderSent(true);
// //         client.setResponseComplete(true);
// //         return; // Ensure we exit after sending the response
// //     }
// //     else 
// //     {
// //         if (!client.getResponseBody().empty()) 
// //         {
// //             char buffer[1024];
// //             if (resp.isText() == true) {
// //                 ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
// //                 if (bytesSent <= 0 || bytesSent == (int)client.getResponseBody().length()) {
// //                     std::remove(req.get_bodyFileName().c_str());
// //                     closeClient(client.fetchConnectionSocket());
// //                 }
// //                 client.setResponseComplete(true); // Mark response as complete
// //                 return;
// //             } 
// //             else if (!client.getOpenFile())
// //             {
// //                 client.openFileForReading();
// //                 if (!client.getOpenFile())
// //                 {
// //                     std::cerr << "Failed to open file: " << client.getResponseBody() << std::endl;
// //                     closeClient(client.fetchConnectionSocket());
// //                     return;
// //                 }
// //             }
// //             if (client._file.good()) 
// //             {
// //                 client.readFromFile(buffer, 1024);
// //                 std::size_t bytesRead = static_cast<std::size_t>(client.bytesRead());
// //                 client.setResponse(std::string(buffer, bytesRead));
// //             }
// //             else 
// //             {
// //                 closeClient(client.fetchConnectionSocket());
// //                 return;
// //             }
// //         }

// //         ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponse().c_str(), client.getResponse().length(), 0);
// //         if (bytesSent <= 0) {
// //             closeClient(client.fetchConnectionSocket());
// //         }
// //         client.setResponseComplete(true); // Mark response as complete
// //     }
// // }

void WebServer::sendResponse(HttpRequest &req, NetworkClient &client) 
{ 
    static int requestCounter = 0;
    requestCounter++;
    // std::cout << "req count == "<< requestCounter << std::endl;

    HttpResponse resp(client);

    resp.generateResponse(req);

    if (!client.getHeaderSent()) 
    {
        client.setResponseHeader(resp.createResponseHeader()); // Assurez-vous que cette fonction crée bien les en-têtes
        send(client.fetchConnectionSocket(), client.getResponseHeader().c_str(), client.getResponseHeader().length(), 0);
        client.setHeaderSent(true);
    }

    // Envoyer le corps de la réponse
    if (!client.getResponseBody().empty()) 
    {
        ssize_t bytesSent = send(client.fetchConnectionSocket(), client.getResponseBody().c_str(), client.getResponseBody().length(), 0);
        if (bytesSent < 0) {
            // Gestion d'erreur si l'envoi échoue
            std::cerr << "Failed to send response body." << std::endl;
        }
        else if (bytesSent < (ssize_t)client.getResponseBody().length()) {
            // Gérer le cas où tous les bytes n'ont pas été envoyés
            std::cerr << "Not all data was sent in the response." << std::endl;
        }
    }

    close(client.fetchConnectionSocket()); // Fermez la connexion après avoir envoyé la réponse
}
