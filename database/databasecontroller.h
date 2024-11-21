#pragma once

#include "databasesettings.h"
#include "pimpl.h"

namespace slk
{

class DatabaseController final
{
public:
    DatabaseController(const DatabaseSettings& settings);
    DatabaseController();
    ~DatabaseController();

    bool connect();
    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    DECLARE_PIMPL
};

} //! slk
