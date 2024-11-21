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

    int getValue();
    bool connect();
    bool connect(const DatabaseSettings& settings);
    bool close();

private:
    void prepareAllStatements();

private:
    DECLARE_PIMPL
};

} //! slk
