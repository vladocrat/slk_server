#pragma once

#include <QObject>

#include "databasecontroller.h"

class UsersTest : public QObject
{
   Q_OBJECT

private slots:
   void initTestCase();
   void userExists();
   void userByEmail();
   void userByUsername();
   void userLogIn();
   void userIdByUsername();

private:
   slk::DatabaseController m_controller;
};
