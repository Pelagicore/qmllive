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

#include "livenodeengine.h"

class BenchQuickView;
class WindowWidget;
class PreviewImageProvider;
class WorkspaceView;
class BenchLiveNodeEngine : public LiveNodeEngine
{
    Q_OBJECT

public:
    BenchLiveNodeEngine(QObject* parent = 0);

    void setWorkspaceView(WorkspaceView* view);
    void setWindowWidget(WindowWidget *widget);

public slots:
    void refresh();
    void reloadDocument();

signals:
    void viewChanged(BenchQuickView *view);

protected:
    void initPlugins();
    void reloadHelper();

    virtual QQuickView* initView();

private Q_SLOTS:
    QImage convertIconToImage(const QFileInfo& info, const QSize& requestedSize);
    void onHeightChanged(int height);
    void onWidthChanged(int width);
    void onSizeChanged(const QSize& size);

private:
    BenchQuickView *m_view;
    WindowWidget* m_ww;
    QPointer<PreviewImageProvider> m_imageProvider;
    WorkspaceView* m_workspaceView;
    bool m_ignoreCache;
    bool m_clipToRootObject;
};
