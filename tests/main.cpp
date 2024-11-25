#include <QCoreApplication>
#include <QtTest>
#include <vector>

#include "database/userstest.h"
#include "database/roomstest.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    std::vector<QObject*> tests { new UsersTest{},
                                 new RoomsTest{}
    };

    int status = -1;
    std::ranges::for_each(tests, [&](QObject* test) {
        status = QTest::qExec(test, argc, argv);
    });

    return status;
}
