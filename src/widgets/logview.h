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

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtQuick>
#include <QtWidgets>

#include "logger.h"


class LogView : public QWidget
{
    Q_OBJECT
public:
    enum {
        InternalInfo = QtDebugMsg - 1,
        InternalError = QtDebugMsg - 2
    };

    explicit LogView(bool createLogger = true, QWidget *parent = 0);
public slots:
    void setIgnoreMessages(bool ignoreMessages);
    void clear();
    void appendToLog(int type, const QString &msg, const QUrl &url = QUrl(), int line = -1, int column = -1);
    void appendToLog(const QList<QQmlError> &errors);

private:
    QPlainTextEdit *m_log;

    QMutex m_mutex;
    Logger* m_logger;
};
