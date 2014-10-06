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

#include "logger.h"
#include "stdio.h"

Logger *Logger::s_instance = 0;
bool Logger::s_ignoreMesssages = false;
QMutex Logger::m_mutex;

/*!
 * \class Logger
 * \brief Installs a qt messageHandler and receives all log messages
 * \group qmllive
 *
 * The intention is to use this class if you want to display the log into widget
 * or you want to preproccess the log messages itself before displaying it
 *
 * \sa RemoteLogger
 */

/*!
 * Standard constructor using \a parent as parent
 */
Logger::Logger(QObject *parent) :
    QObject(parent)
{
    if (s_instance) {
        qFatal("Cannot create more than one Logger");
    }
    s_instance = this;

    qInstallMessageHandler(messageHandler);
}

/*!
 * Standard destructor
 */
Logger::~Logger()
{
    qInstallMessageHandler(0);
}

/*!
 * When \a ignoreMessages set to true, the Logger ignores all incomming log messages
 */
void Logger::setIgnoreMessages(bool ignoreMessages)
{
    QMutexLocker l(&m_mutex);
    s_ignoreMesssages = ignoreMessages;
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (!s_instance) {
        printf("No Logger instance for log message:\n  %s\n", msg.toLatin1().constData());
        abort();
    }

    {
        QMutexLocker l(&m_mutex);
        if (s_ignoreMesssages)
            return;
    }

    emit s_instance->message(type, msg);

    // and output to stdout
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
    }
}

/*!
 * \fn void Logger::message(int type, const QString &msg)
 * Emitted for every incomming log message
 * \a type describes the type of the log message (QtMsgType).
 * \a msg is the debug message
 * This signal will not be emitted when ignoreMessages set to true
 * \sa setIgnoreMessages()
 */
