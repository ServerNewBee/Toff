//
// Created by tywang on 17-5-1.
//

#include "simple_http_server.h"

using namespace std;

typedef struct
{
    int sock;
    HttpRequest* req;
    string *vbuf;
    string *fbuf;
    bool onEOM;
} Msg;

int method_transform(unsigned int method_int, string& method)
{
    method = http_method_str((enum http_method) method_int);
    return 0;
}

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
    method_transform(parser->method, msg->req->mMethod);
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

int SimpleHttpServer::process(int sock)
{
    // Parser Settings
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

    // Data Flow : thread -> callback
    Msg msg_mem;
    HttpRequest req;
    HttpResponse res;
    string fbuf, vbuf;
    Msg *msg = &msg_mem;
    msg->sock = sock;
    msg->onEOM = false;
    msg->req = &req;
    msg->fbuf = &fbuf;
    msg->vbuf = &vbuf;
    
    // Parser Construct
    http_parser http_parser_mem;
    http_parser *parser = &http_parser_mem;
    http_parser_init(parser, HTTP_REQUEST); 
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
        // Data Flow : callback -> thread
        int nparsed = http_parser_execute(parser, &settings, buf, recved);
        if (HTTP_PARSER_ERRNO(parser) != http_errno::HPE_OK)
        {
            // parser error
        }
        else if (parser->upgrade)
        {
            // this is not what we expected
        }
        else if (nparsed != recved)
        {
            // this is a parser error
        }
        Msg *msg = (Msg*)parser->data;
        if (msg->onEOM)
            break;
    }
    handler(req, res, this_ptr);
    send(sock, raw, strlen(raw), 0);
    close(sock);
    return 0;
}

int SimpleHttpServer::start()
{
    // Step 3 Listen & Accept
    socklen_t clilen;
    struct sockaddr_in cli_addr;
    int newsockfd;
    listen(sockfd,5);
    while(!isInterrupted)
    {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        shared_ptr<thread> pThread(new thread(&SimpleHttpServer::process, this, newsockfd));
        pThread->detach();
    }
    puts("Interrupted");
    close(sockfd);
    return 0;
}

int SimpleHttpServer::Start()
{
    shared_ptr<thread> pThread(new thread(&SimpleHttpServer::start, this));
    pThread->detach();
    puts("Started");
}

int SimpleHttpServer::Join()
{
    isInterrupted = true;
}

int SimpleHttpServer::Init(int portno, int _threadNum, std::function<int(HttpRequest&, HttpResponse&, void*)> _handler, void* _this_ptr)
{
    // Step 0 Set up
    threadNum = _threadNum;
    handler = _handler;
    this_ptr = _this_ptr;
    // Step 1 Init Socket and Address
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
    // Step 2 Bind to Port
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
        return error("ERROR on binding");
    puts("Init Over");
    return 0;
}

