#include "roomstest.h"

#include <QtTest/QTest>

#include "configurationcontroller.h"
#include "roomdata.h"
#include "userdata.h"

void RoomsTest::initTestCase()
{
    const auto settings = slk::ConfigurationController::getDBSettings();
    qDebug() << settings.host << " " << settings.port;
    QVERIFY(m_controller.connect(settings));
}

void RoomsTest::roomByIdTest()
{
    const uint32_t testRoomId = 3; //! Test room Id
    const auto res = m_controller.getRoomById(testRoomId);

    QVERIFY(res);

    slk::RoomData data;
    data.name = "testRoom";
    data.creator_id = 2; //! test acoount id
    data.guid = "{test-guid-lsdlada241wd}";

    QVERIFY(data.name == res.value().name);
}

void RoomsTest::roomUsersById()
{
    const uint32_t testRoomId = 3;
    const auto res = m_controller.getRoomUsers(testRoomId);

    QVERIFY(res);

    slk::UserData user;
    user.username = "test";
    user.mail = "test@gmail.com";

    QVERIFY(user.username == res.value()[0].username);
    QVERIFY(user.mail == res.value()[0].mail);
}

