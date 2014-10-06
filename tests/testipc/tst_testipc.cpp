/*
  Copyright (c) 2014, Pelagicore AG.
  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtCore>
#include <QtTest>
#include <QtNetwork>

#include "ipc/ipcserver.h"
#include "ipc/ipcclient.h"

class TestIpc : public QObject
{
    Q_OBJECT

public:
    TestIpc() {}
public slots:
    void handleCall(QString method, QByteArray data) {
        if(method == "echo(QString)") {
            QDataStream stream(data);
            QString message;
            stream >> message;
            qDebug() << method << ": " << message;
        }
        if(method == "sendFile(QString,QByteArray)") {
            QDataStream stream(data);
            QString path;
            stream >> path;
            QByteArray body;
            stream >> body;
            qDebug() << method << ": " << path;
        }
    }

private Q_SLOTS:
    void call() {
        IpcServer peer1;
        peer1.listen(10234);
        connect(&peer1, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
        IpcClient peer2;
        peer2.setDestination("127.0.0.1", 10234);
        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::ReadWrite);
        stream << QString("Hello IPC!");
        peer2.send("echo(QString)", bytes);
        QSignalSpy received(&peer1, SIGNAL(received(QString,QByteArray)));
        QTest::qWait(100);
        QVERIFY(received.count() == 1);
    }

    void sendFile() {
        IpcServer peer1;
        peer1.listen(10234);
        connect(&peer1, SIGNAL(received(QString,QByteArray)), this, SLOT(handleCall(QString,QByteArray)));
        IpcClient peer2;
        peer2.setDestination("127.0.0.1", 10234);
        QByteArray bytes;
        QDataStream stream(&bytes, QIODevice::ReadWrite);
        QString filePath("tst_testipc.cpp");
        stream << filePath;
        stream << QString("hello").toLatin1();
        peer2.send("sendFile(QString,QByteArray)", bytes);
        QSignalSpy received(&peer1, SIGNAL(received(QString,QByteArray)));
        QTest::qWait(100);
        QVERIFY(received.count() == 1);
    }
};

QTEST_MAIN(TestIpc)

#include "tst_testipc.moc"
