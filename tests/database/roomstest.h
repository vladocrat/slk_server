#pragma once

#include <QObject>

#include "databasecontroller.h"

class RoomsTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void roomByIdTest();
    void roomUsersById();

private:
    slk::DatabaseController m_controller;
};

