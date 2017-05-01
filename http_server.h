//
// Created by tywang on 17-5-1.
//

#ifndef TOFF_HTTP_SERVER_H
#define TOFF_HTTP_SERVER_H

#include <vector>

class HTTPServer
{
public:
    struct IPConfig
    {

    };
    /**
     * Bind server to the following addresses. Can be called from any thread.
     * Once a binding fails, the process will not continue.
     * @param addrs
     * @return
     */
    int bind(std::vector<IPConfig>& addrs);
    /**
     * Start HTTPServer.
     * Note this is a blocking call and the current thread will be used to listen for incoming connections.
     * `onSuccess` callback will be invoked in new thread
     * @param onSuccess
     * @return
     */
    int start(std::function<void()> onSuccess = nullptr);
    /**
     * Stop HTTPServer.
     * Can be called from any thread, but only after start() has called onSuccess.
     * Server will stop listening for new connections and will wait for running requests to finish.
     * @return
     */
    int stop();
    std::vector<IPConfig> addresses() const {
        return addresses_;
    }
private:
    std::vector<IPConfig> addresses_;
};

#endif //TOFF_HTTP_SERVER_H
