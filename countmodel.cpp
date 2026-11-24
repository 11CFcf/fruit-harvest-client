#include "countmodel.h"

#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>

#include <cmath>
#include <limits>

namespace {
constexpr quint16 ApiPort = 8080;
constexpr qint64 MaximumRequestBytes = 1024 * 1024;
constexpr int CameraTimeoutSeconds = 15;

QByteArray errorBody(const QString &message)
{
    return QJsonDocument(QJsonObject{{QStringLiteral("error"), message}})
        .toJson(QJsonDocument::Compact);
}
}

CountModel::CountModel(QObject *parent) : QObject(parent)
{
    connect(&m_httpServer, &QTcpServer::newConnection,
            this, &CountModel::acceptConnections);

    m_serverRunning = m_httpServer.listen(QHostAddress::AnyIPv4, ApiPort);
    if (!m_serverRunning)
        m_serverError = m_httpServer.errorString();

    m_cameraTimer.setInterval(1000);
    connect(&m_cameraTimer, &QTimer::timeout, this, &CountModel::updateCameraStatus);
    m_cameraTimer.start();
}

void CountModel::acceptConnections()
{
    while (m_httpServer.hasPendingConnections()) {
        auto *socket = m_httpServer.nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this,
                [this, socket]() { readRequest(socket); });
        connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
        if (socket->bytesAvailable() > 0)
            readRequest(socket);
    }
}

void CountModel::readRequest(QTcpSocket *socket)
{
    QByteArray buffer = socket->property("fruitApiBuffer").toByteArray();
    buffer += socket->readAll();
    if (buffer.size() > MaximumRequestBytes + 16384) {
        sendJson(socket, 413, errorBody(QStringLiteral("request too large")));
        return;
    }

    const qsizetype headerEnd = buffer.indexOf("\r\n\r\n");
    if (headerEnd < 0) {
        socket->setProperty("fruitApiBuffer", buffer);
        return;
    }

    const QList<QByteArray> lines = buffer.left(headerEnd).split('\n');
    if (lines.isEmpty()) {
        sendJson(socket, 400, errorBody(QStringLiteral("invalid HTTP request")));
        return;
    }
    const QList<QByteArray> requestParts = lines.first().trimmed().split(' ');
    if (requestParts.size() < 2) {
        sendJson(socket, 400, errorBody(QStringLiteral("invalid HTTP request line")));
        return;
    }

    const QByteArray method = requestParts.at(0).toUpper();
    const QByteArray path = requestParts.at(1).split('?').first();
    qint64 contentLength = 0;
    for (qsizetype i = 1; i < lines.size(); ++i) {
        const QByteArray line = lines.at(i).trimmed();
        const qsizetype colon = line.indexOf(':');
        if (colon > 0 && line.left(colon).trimmed().toLower() == "content-length") {
            bool ok = false;
            contentLength = line.mid(colon + 1).trimmed().toLongLong(&ok);
            if (!ok || contentLength < 0) {
                sendJson(socket, 400, errorBody(QStringLiteral("invalid content length")));
                return;
            }
        }
    }
    if (contentLength > MaximumRequestBytes) {
        sendJson(socket, 413, errorBody(QStringLiteral("request too large")));
        return;
    }

    const qsizetype bodyStart = headerEnd + 4;
    if (buffer.size() - bodyStart < contentLength) {
        socket->setProperty("fruitApiBuffer", buffer);
        return;
    }

    if (path != "/api/counts") {
        sendJson(socket, 404, errorBody(QStringLiteral("not found")));
        return;
    }

    if (method == "OPTIONS") {
        sendJson(socket, 204, QByteArray());
        return;
    }
    if (method == "GET") {
        QJsonObject response{
            {QStringLiteral("apples"), m_apples},
            {QStringLiteral("oranges"), m_oranges},
            {QStringLiteral("camera_connected"), m_cameraOnline},
            {QStringLiteral("updated_at"), m_lastUpload.isValid()
                 ? m_lastUpload.toString(Qt::ISODate)
                 : QString()}
        };
        sendJson(socket, 200, QJsonDocument(response).toJson(QJsonDocument::Compact));
        return;
    }
    if (method != "POST") {
        sendJson(socket, 405, errorBody(QStringLiteral("method not allowed")));
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(buffer.mid(bodyStart, contentLength), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        sendJson(socket, 400, errorBody(QStringLiteral("expected a JSON object")));
        return;
    }

    const QJsonObject payload = document.object();
    const QJsonValue applesValue = payload.value(QStringLiteral("apples"));
    const QJsonValue orangesValue = payload.value(QStringLiteral("oranges"));
    const auto validCount = [](const QJsonValue &value) {
        if (!value.isDouble())
            return false;
        const double number = value.toDouble();
        return number >= 0 && number <= std::numeric_limits<int>::max()
            && std::floor(number) == number;
    };
    if (!validCount(applesValue) || !validCount(orangesValue)) {
        sendJson(socket, 400, errorBody(QStringLiteral("apples and oranges must be non-negative integers")));
        return;
    }

    m_apples = applesValue.toInt();
    m_oranges = orangesValue.toInt();
    m_lastUpload = QDateTime::currentDateTime();
    m_lastUpdated = m_lastUpload.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    emit countsChanged();
    if (!m_cameraOnline) {
        m_cameraOnline = true;
        emit cameraStatusChanged();
    }

    const QJsonObject response{
        {QStringLiteral("apples"), m_apples},
        {QStringLiteral("oranges"), m_oranges}
    };
    sendJson(socket, 200, QJsonDocument(response).toJson(QJsonDocument::Compact));
}

void CountModel::sendJson(QTcpSocket *socket, int status, const QByteArray &body)
{
    QByteArray reason;
    switch (status) {
    case 200: reason = "OK"; break;
    case 204: reason = "No Content"; break;
    case 400: reason = "Bad Request"; break;
    case 404: reason = "Not Found"; break;
    case 405: reason = "Method Not Allowed"; break;
    case 413: reason = "Payload Too Large"; break;
    default: reason = "Error"; break;
    }

    QByteArray response = "HTTP/1.1 " + QByteArray::number(status) + " " + reason + "\r\n";
    response += "Content-Type: application/json; charset=utf-8\r\n";
    response += "Access-Control-Allow-Origin: *\r\n";
    response += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    response += "Access-Control-Allow-Headers: Content-Type\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n\r\n";
    response += body;
    socket->write(response);
    socket->disconnectFromHost();
}

void CountModel::updateCameraStatus()
{
    const bool online = m_lastUpload.isValid()
        && m_lastUpload.secsTo(QDateTime::currentDateTime()) <= CameraTimeoutSeconds;
    if (m_cameraOnline == online)
        return;
    m_cameraOnline = online;
    emit cameraStatusChanged();
}
