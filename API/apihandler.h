#pragma once

#include <QObject>

#include "pimpl.h"

namespace slk
{

class ApiHandler final : public QObject
{
    Q_OBJECT
public:
    ApiHandler();
    ~ApiHandler();

    bool initialize();

private:
    DECLARE_PIMPL
};

} //! slk
