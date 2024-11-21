#pragma once

#include "pimpl.h"
#include "databasesettings.h"

namespace slk
{

class Database
{
public:
    Database();
    ~Database();

    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    DECLARE_PIMPL
};

} //! slk
