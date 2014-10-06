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

#include "workspacedelegate.h"

WorkspaceDelegate::WorkspaceDelegate(FileSystemModel* model, QObject *parent) :
    QStyledItemDelegate(parent),
    m_model(model)
{
}

void WorkspaceDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    if (option) {
        if (m_model->isDir(index))
            return;

        QString path = m_model->filePath(index);
        foreach(QString type, m_model->allowedTypesFilter())
        {
            if (path.contains(QRegExp(type, Qt::CaseInsensitive, QRegExp::Wildcard)))
                return;
        }

        option->state &= ~QStyle::State_Enabled;

        QColor disabled = option->palette.color(QPalette::Disabled, QPalette::Text);
        QColor enabled = option->palette.color(QPalette::Normal, QPalette::Text);

        if (disabled == enabled) {
            QColor t = option->palette.color(QPalette::Disabled, QPalette::Text);
            QColor b = option->palette.color(QPalette::Disabled, QPalette::Background);

            QColor selection = QColor::fromRgb((t.red() + b.red()) / 2, (t.green() + b.green()) / 2, (t.blue() + b.blue()) / 2);

            option->palette.setColor(QPalette::Disabled, QPalette::Text, selection);
        }
    }
}
