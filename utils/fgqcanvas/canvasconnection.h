//
// Copyright (C) 2017 James Turner  zakalawe@mac.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef CANVASCONNECTION_H
#define CANVASCONNECTION_H

#include <memory>

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QUrl>
#include <QRectF>

class LocalProp;
class QNetworkAccessManager;

class CanvasConnection : public QObject
{
    Q_OBJECT

    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)

// QML exposed versions of the destination rect
    Q_PROPERTY(QPointF center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(QSizeF size READ size WRITE setSize NOTIFY sizeChanged)

    Q_PROPERTY(QUrl webSocketUrl READ webSocketUrl NOTIFY webSocketUrlChanged)
    Q_PROPERTY(QString rootPath READ rootPath NOTIFY rootPathChanged)
public:
    explicit CanvasConnection(QObject *parent = nullptr);
    ~CanvasConnection();

    void setNetworkAccess(QNetworkAccessManager *dl);
    void setRootPropertyPath(QByteArray path);

    enum Status
    {
        NotConnected,
        Connecting,
        Connected,
        Closed,
        Reconnecting,
        Error
    };

    Status status() const
    {
        return m_status;
    }

    QJsonObject saveState() const;
    bool restoreState(QJsonObject state);

    void connectWebSocket(QByteArray hostName, int port);
    QPointF center() const;

    QSizeF size() const;

    LocalProp* propertyRoot() const;

    QUrl webSocketUrl() const
    {
        return m_webSocketUrl;
    }

    QString rootPath() const
    {
        return QString::fromUtf8(m_rootPropertyPath);
    }

public Q_SLOTS:
    void reconnect();

    // not on iOS / Android - requires widgets
    void showDebugTree();

    void setCenter(QPointF center);

    void setSize(QSizeF size);

signals:
    void statusChanged(Status status);

    void centerChanged(QPointF center);

    void sizeChanged(QSizeF size);

    void rootPathChanged();

    void webSocketUrlChanged();

    void updated();
private Q_SLOTS:
    void onWebSocketConnected();
    void onTextMessageReceived(QString message);
    void onWebSocketClosed();

private:
    void setStatus(Status newStatus);
    LocalProp *propertyFromPath(QByteArray path) const;

    QUrl m_webSocketUrl;
    QByteArray m_rootPropertyPath;
    QRectF m_destRect;

    QWebSocket m_webSocket;
    QNetworkAccessManager* m_netAccess = nullptr;
    std::unique_ptr<LocalProp> m_localPropertyRoot;
    QHash<int, LocalProp*> idPropertyDict;
    Status m_status = NotConnected;
    QString m_rootPath;
};

#endif // CANVASCONNECTION_H