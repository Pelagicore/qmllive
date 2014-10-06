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

#include "logview.h"

LogView::LogView(bool createLogger, QWidget *parent)
    : QWidget(parent)
    , m_log(new QPlainTextEdit(this))
    , m_logger(0)
{
    m_log->setReadOnly(true);
    m_log->setMaximumBlockCount(1000);
    m_log->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_log);
    setLayout(layout);

    if (createLogger) {
        m_logger = new Logger(this);
        connect(m_logger, SIGNAL(message(int,QString)), this, SLOT(appendToLog(int,QString)));
    }
}

void LogView::setIgnoreMessages(bool ignoreMessages)
{
    if (m_logger)
        m_logger->setIgnoreMessages(ignoreMessages);
}

void LogView::appendToLog(int type, const QString &msg, const QUrl &url, int line, int column)
{
    QMutexLocker l(&m_mutex);

    qreal baseValue = m_log->palette().color(QPalette::Base).valueF();
    QColor color = m_log->palette().color(QPalette::Text);

    switch (type) {
    case QtWarningMsg: // yellow
        color = baseValue < 0.5f ? QColor(255, 255, 128) : QColor(140, 140, 0);
        break;
    case QtCriticalMsg: // red
        color = baseValue < 0.5f ? QColor(255, 64, 64) : QColor(165, 0, 0);
        break;
    case QtFatalMsg: // red
        color = baseValue < 0.5f ? QColor(255, 64, 64) : QColor(165, 0, 0);
        break;
    case InternalInfo: // green
        color = baseValue < 0.5f ? QColor(96, 255, 96) : QColor(128, 0, 0);
        break;
    case InternalError: // purple
        color = baseValue < 0.5f ? QColor(196, 128, 196) : QColor(96, 0, 96);
        break;
    default:
        break;
    }

    QString s;
    if (url.isValid()) {
        s.append(url.isLocalFile() ? url.toLocalFile() : url.toString());
        s.append(QLatin1Char(':'));
    }
    if (line > 0) {
        s.append(QString::number(line));
        s.append(QLatin1Char(':'));
    }
    Q_UNUSED(column);
    if (!s.isEmpty())
        s.append(QLatin1Char(' '));

    s.append(QString::fromLatin1("<b><font color=\"%2\">%1</font></b>")
             .arg(msg)
             .arg(color.name()));

    m_log->appendHtml(s);
}

void LogView::appendToLog(const QList<QQmlError> &errors)
{
    foreach (const QQmlError &err, errors) {
        if (!err.isValid())
            continue;

        QtMsgType type = QtDebugMsg;

        if (err.description().contains(QString::fromLatin1("error"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not installed"), Qt::CaseInsensitive) ||
            err.description().contains(QString::fromLatin1("is not a type"), Qt::CaseInsensitive))
            type = QtCriticalMsg;
        else if (err.description().contains(QString::fromLatin1("warning"), Qt::CaseInsensitive))
            type = QtWarningMsg;

        appendToLog(type, err.description(), err.url(), err.line(), err.column());
    }
}

void LogView::clear()
{
    QMutexLocker l(&m_mutex);
    m_log->clear();
}

