#pragma once

#include "databasesettings.h"
#include "utils.h"

namespace slk
{

class DatabaseController final
{
    DatabaseController(const DatabaseSettings& settings);
    DatabaseController();
    ~DatabaseController();

    bool connect();
    bool connect(const DatabaseSettings& settings);

private:
    DECLARE_PIMPL
};

} //! slk
