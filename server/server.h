#pragma once

#include <QTcpServer>

#include "utils.h"

namespace slk {

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    ~Server();
    
private:
    DECLARE_PIMPL_EX(Server)
};

} //! slk
