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
#include <QtCore/QString>
#include <QtTest/QtTest>
#include "../../src/sync/syncengine.h"

class TestSync : public QObject
{
    Q_OBJECT

public:
    TestSync() {}

private Q_SLOTS:
    void rsync() {
        SyncEngine engine;
        engine.setProgram("/usr/bin/rsync");
        engine.setLogin("jryannel");
        engine.setMachine("127.0.0.1");
        engine.setRemoteBasePath("/Users/jryannel/temp/temp1");
        engine.setLocalBasePath("/Users/jryannel/temp/temp2");
//        engine.setDryRun(true);
        engine.start();
        engine.wait();
    }
};

QTEST_APPLESS_MAIN(TestSync)
#include "tst_testsync.moc"
