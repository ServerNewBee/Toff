// System
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
// C++
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
// Lib
#include "http_parser/http_parser.h"
#include "http_types/http_request.h"
#include "http_types/http_response.h"

#define glog cerr

using namespace std;

typedef struct
{
    int sock;
    HttpRequest* req;
    string *vbuf;
    string *fbuf;
    bool onEOM;
} Msg;

int on_message_begin_cb(http_parser* parser)
{
    return 0;
}
int on_url_cb(http_parser* parser, const char *at, size_t length)
{
    Msg *msg = (Msg*) parser->data;
    msg->req->mUrl.append(at, length);
    return 0;
}
int on_header_field_cb(http_parser* parser, const char *at, size_t length)
{
    Msg *msg = (Msg*) parser->data;
    HttpRequest* req = msg->req;
    if (msg->vbuf->size() > 0)
    {
        req->mHeaders[*(msg->fbuf)] = *(msg->vbuf);
        msg->fbuf->clear();
        msg->vbuf->clear();
    }
    msg->fbuf->append(at, length);
    return 0;
}
int on_header_value_cb(http_parser* parser, const char *at, size_t length)
{
    Msg *msg = (Msg*) parser->data;
    msg->vbuf->append(at, length);
    return 0;
}
int on_headers_complete_cb(http_parser* parser)
{
    Msg *msg = (Msg*) parser->data;
    HttpRequest* req = msg->req;
    if (msg->vbuf->size() > 0)
    {
        req->mHeaders[*(msg->fbuf)] = *(msg->vbuf);
        msg->fbuf->clear();
        msg->vbuf->clear();
    }
    return 0;
}
int on_body_cb(http_parser* parser, const char *at, size_t length)
{
    Msg *msg = (Msg*) parser->data;
    msg->req->mContent.append(at, length);
    return 0;
}
int on_message_complete_cb(http_parser* parser)
{
    Msg *msg = (Msg*) parser->data;
    msg->onEOM = true;
    return 0;
}
/* When on_chunk_header is called, the current chunk length is stored
* in parser->content_length.
*/
int on_chunk_cb(http_parser* parser)
{
    return 0;
}
int on_chunk_complete_cb(http_parser* parser)
{
    return 0;
}

int error(const char* error_msg)
{
    puts(error_msg);
    return -1;
}

int method_transform(unsigned int method_int, string& method)
{
    method = http_method_str((enum http_method) method_int);
    return 0;
}

const char* raw = "HTTP/1.1 301 Moved Permanently\r\n"
"Location: http://www.google.com/\r\n"
"Content-Type: text/html; charset=UTF-8\r\n"
"Date: Sun, 26 Apr 2009 11:11:49 GMT\r\n"
"Expires: Tue, 26 May 2009 11:11:49 GMT\r\n"
"Cache-Control: public, max-age=2592000\r\n"
"Server: gws\r\n"
"Content-Length: 219\r\n"
"\r\n"
"<HTML><HEAD><meta http-equiv=\"content-type\" content=\"text/html;charset=utf-8\">\n"
"<TITLE>301 Moved</TITLE></HEAD><BODY>\n"
"<H1>301 Moved</H1>\n"
"The document has moved\n"
"<A HREF=\"http://www.google.com/\">here</A>.\r\n"
"</BODY></HTML>\r\n";

void httpParserThread(int sock)
{
    http_parser_settings settings; 
    http_parser_settings_init(&settings);
    settings.on_message_begin = on_message_begin_cb;
    settings.on_url = on_url_cb;
    settings.on_header_field = on_header_field_cb;
    settings.on_header_value = on_header_value_cb;
    settings.on_headers_complete = on_headers_complete_cb;
    settings.on_body = on_body_cb;
    settings.on_message_complete = on_message_complete_cb;
    settings.on_chunk_header = on_chunk_cb;
    settings.on_chunk_complete = on_chunk_complete_cb;

    // Watch Out ! Memory Leak !
    http_parser *parser = new http_parser();
    http_parser_init(parser, HTTP_REQUEST); 
    
    HttpRequest req;
    HttpResponse res;

    // Data Flow : thread -> callback
    Msg *msg = new Msg();
    msg->sock = sock;
    msg->onEOM = false;
    msg->req = &req;
    msg->fbuf = new string();
    msg->vbuf = new string();
    parser->data = msg;

    const int buf_len = 80 * 1024;
    char buf[buf_len];
    while(1)
    {
        int recved = recv(sock, buf, buf_len, 0);
        if (recved < 0)
        {
            // recv error
        }
        puts(buf);
        // Data Flow : callback -> thread
        printf("recved %d\n", recved);
        int nparsed = http_parser_execute(parser, &settings, buf, recved);
        printf("nparsed %d\n", nparsed);
        printf("error %s\n", http_errno_name(HTTP_PARSER_ERRNO(parser)));
        //if (HTTP_PARSER_ERRNO(parser) == http_errno::HPE_OK) break;
        if (parser->upgrade)
        {
            // this is not what we expected
        } else if (nparsed != recved)
        {
            // this is a parser error
        }
        Msg *msg = (Msg*)parser->data;
        if (msg->onEOM)
            break;
    }
    method_transform(parser->method, msg->req->mMethod);
    /*
    //OnRequestComplete(res, req);
    cout << "***************************" << endl;
    msg = (Msg*) parser->data;
    cout << msg->req->mUrl << endl;
    cout << msg->req->mMethod << endl;
    for (auto i : msg->req->mHeaders)
    {
        cout << i.first << endl;
        cout << i.second << endl;
    }
    cout << msg->req->mContent << endl;
    cout << "***************************" << endl;
    */
    send(sock, raw, strlen(raw), 0);
    close(sock);
}

int start(int portno)
{
    // Step 1 Init Socket
    int sockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        return error("ERROR opening socket");
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return error("setsockopt(SO_REUSEADDR) failed");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        return error("ERROR on binding");
    // Step 3 Listen & Accept
    listen(sockfd,5);
    while(1)
    {
        int newsockfd;
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        shared_ptr<thread> pThread;
        //httpParserThread(newsockfd);
        pThread.reset(new thread(httpParserThread, newsockfd));
        pThread->detach();
    }
    close(sockfd);
    return 0;
}

int main()
{
    start(9501);
    return 0;
}
