//
// Created by wpliu on 17-5-1.
//

#ifndef TOFF_ACCEPTOR_H
#define TOFF_ACCEPTOR_H

namespace toff
{
class Acceptor
{
public:
    void listen();

private:
    void handleRead();
};
}
#endif //TOFF_ACCEPTOR_H
