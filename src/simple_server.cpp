// C++
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

// dep
#include "http_types/http_request.h"
#include "http_types/http_response.h"
#include "simple_http_server.h"

class PrinterService
{
public:
    static int handler(HttpRequest &req, HttpResponse &res, void *this_ptr)
    {
        PrinterService* self = static_cast<PrinterService*>(this_ptr);
        self->printer(req, res);
    }
    int printer(HttpRequest &req, HttpResponse &res)
    {
        cout << "***************************" << endl;
        cout << "url: " << req.mUrl << endl;
        cout << "method: " << req.mMethod << endl;
        for (auto i : req.mHeaders)
        {
            cout << "header field: " << i.first << endl;
            cout << "header value: " << i.second << endl;
        }
        cout << "content: " << req.mContent << endl;
        cout << "***************************" << endl;
    }
    void run()
    {
        SimpleHttpServer server;
        server.Init(9501, 128, PrinterService::handler, this);
        server.Start();
        while(1);
    }
};

int main()
{
    PrinterService service;
    service.run();
    return 0;
}
