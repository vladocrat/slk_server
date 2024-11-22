#include "userstest.h"

#include <QtTest/QTest>

#include "hash.h"
#include "userdata.h"
#include "configurationcontroller.h"

void UsersTest::initTestCase()
{
    const auto settings = slk::ConfigurationController::getDBSettings();
    qDebug() << settings.host << " " << settings.port;
    QVERIFY(m_controller.connect(settings));
}

void UsersTest::userExists()
{
    slk::UserData data;
    data.username = "test";
    data.password_hash = slk::createHash("password123!");
    data.mail = "test@gmail.com";

    QVERIFY(m_controller.userExists(data));
}

void UsersTest::userByEmail()
{
    const auto user = m_controller.getUserByEmail("test@gmail.com");
    QVERIFY(user);
    QVERIFY(user->username == "test");
    QVERIFY(user->mail == "test@gmail.com");
}

void UsersTest::userByUsername()
{
    const auto user = m_controller.getUserByUsername("test");
    QVERIFY(user);
    QVERIFY(user->username == "test");
    QVERIFY(user->mail == "test@gmail.com");
}

void UsersTest::userLogIn()
{
    slk::UserData data;
    data.username = "test";
    data.password_hash = slk::createHash("password123!");
    data.mail = "test@gmail.com";

    QVERIFY(m_controller.logIn(data));
}
