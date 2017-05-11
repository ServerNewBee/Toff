#ifndef HTTP_RESPONSE_H_
#define HTTP_RESPONSE_H_

#include <string>
#include <map>

class HttpResponse
{
public:
    HttpResponse() {}
    virtual ~HttpResponse() {}

    std::string mResponseData;
    int mStatus;
    std::map<std::string, std::string> mResponseHeaders;

    static const int STATUS_OK;
    static const int STATUS_BAD_REQUEST;
    static const int STATUS_INTERNAL_SERVER_ERROR;
    static const int STATUS_NOT_FOUND;
    static const int STATUS_METHOD_NOT_ALLOWED;
    static const int STATUS_REQUEST_TIMEOUT;
};


#endif /* HTTP_RESPONSE_H_ */
