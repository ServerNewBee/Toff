#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include <string>
#include <map>

class HttpRequest
{
public:
    HttpRequest() {}
    virtual ~HttpRequest() {}

    std::string mMethod;                                // request method, like: GET, POST etc.
    std::string mUrl;                                   // url eg. "/control/restart/role"
    std::string mContent;                               // the POST data

    std::string mRemoteHost;                            // the remote host eg. 192.168.0.111 or localhost
    int mRemotePort;

    std::map<std::string, std::string> mParameter;      // parameter which is the part after '?' in url
    std::map<std::string, std::string> mHeaders;        // the http request headers. Note header variables cannot be more than 30 !
};

#endif /* HTTP_REQUEST_H_ */
