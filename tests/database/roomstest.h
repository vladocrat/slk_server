#pragma once

#include <QObject>

#include "databasecontroller.h"

class RoomsTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void roomById();
    void roomByName();
    void roomUsersById();
    void roomUsersByName();

private:
    slk::RoomData testRoom();

    slk::DatabaseController m_controller;
};

