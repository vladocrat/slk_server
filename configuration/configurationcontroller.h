#pragma once

namespace slk
{

struct DatabaseSettings;

class ConfigurationController
{
public:
    ConfigurationController();

    static DatabaseSettings getDBSettings();
};

} //! slk
