#pragma once

#include <string>

/*
host — Name of server to connect to, or the full file path (beginning with a slash) to a Unix-domain socket on the local machine. Defaults to "/tmp". Equivalent to (but overrides) environment variable PGHOST.
hostaddr — IP address of a server to connect to; mutually exclusive with "host".
port — Port number at the server host to connect to, or socket file name extension for Unix-domain connections. Equivalent to (but overrides) environment variable PGPORT.
dbname — Name of the database to connect to. A single server may host multiple databases. Defaults to the same name as the current user's name. Equivalent to (but overrides) environment variable PGDATABASE.
user — User name to connect under. This defaults to the name of the current user, although PostgreSQL users are not necessarily the same thing as system users.
requiressl — If set to 1, demands an encrypted SSL connection (and fails if no SSL connection can be cre
*/

namespace slk
{

struct DatabaseSettings
{
    std::string host;
    std::string hostAddr;
    uint16_t port;
    std::string dbName;
    std::string user;
    bool requireSsl;
};

} //! slk
