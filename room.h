#pragma once

#include <QObject>

#include "utils/utils.h"

namespace slk {

class Room : public QObject
{
    Q_OBJECT
public:
    Room();
    ~Room();
    
private:
    DECLARE_PIMPL_EX(Room)
};

} //! slk
