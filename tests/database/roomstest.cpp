#include "roomstest.h"

#include <QtTest/QTest>

#include "configurationcontroller.h"
#include "roomdata.h"
#include "userdata.h"

namespace
{
static const constexpr uint32_t TEST_ROOM_ID = 3;
static const constexpr uint32_t TEST_ACCOUNT_ID = 2;
static const constexpr char*  TEST_ROOM_NAME = "testRoom";
static const constexpr char*  TEST_ROOM_GUID = "{test-guid-lsdlada241wd}";
}

void RoomsTest::initTestCase()
{
    const auto settings = slk::ConfigurationController::getDBSettings();
    qDebug() << settings.host << " " << settings.port;
    QVERIFY(m_controller.connect(settings));
}

void RoomsTest::roomById()
{
    const std::string testRoomName = "testRoom";
    const auto res = m_controller.getRoom(testRoomName);

    QVERIFY(res);

    slk::RoomData data = testRoom();

    QVERIFY(data == res.value());
}

void RoomsTest::roomByName()
{
    const uint32_t testRoomId = TEST_ROOM_ID;
    const auto res = m_controller.getRoom(testRoomId);

    QVERIFY(res);

    slk::RoomData data = testRoom();

    QVERIFY(data == res.value());
}

void RoomsTest::roomUsersById()
{
    const uint32_t testRoomId = TEST_ROOM_ID;
    const auto res = m_controller.getRoomUsers(testRoomId);

    QVERIFY(res);

    slk::UserData user;
    user.username = "test";
    user.mail = "test@gmail.com";

    QVERIFY(user.username == res.value()[0].username);
    QVERIFY(user.mail == res.value()[0].mail);
}

void RoomsTest::roomUsersByName()
{
    const std::string testRoomName = "testRoom";
    const auto res = m_controller.getRoomUsers(testRoomName);

    QVERIFY(res);

    slk::UserData user;
    user.username = "test";
    user.mail = "test@gmail.com";

    QVERIFY(user.username == res.value()[0].username);
    QVERIFY(user.mail == res.value()[0].mail);
}

slk::RoomData RoomsTest::testRoom()
{
    slk::RoomData data;
    data.id = TEST_ROOM_ID;
    data.name = TEST_ROOM_NAME;
    data.creator_id = TEST_ACCOUNT_ID;
    data.guid = TEST_ROOM_GUID;
    return data;
}

