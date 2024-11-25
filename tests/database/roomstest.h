#pragma once

#include <QObject>

#include "databasecontroller.h"

class RoomsTest : public QObject
{
    Q_OBJECT

public:
    static slk::RoomData testRoom();

private slots:
    void initTestCase();
    void roomById();
    void roomByName();
    void roomUsersById();
    void roomUsersByName();

private:
    slk::DatabaseController m_controller;
};

