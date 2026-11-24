#pragma once

#include <QObject>
#include <QTcpServer>
#include <QTimer>
#include <QDateTime>

class QTcpSocket;

class CountModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int apples READ apples NOTIFY countsChanged)
    Q_PROPERTY(int oranges READ oranges NOTIFY countsChanged)
    Q_PROPERTY(int total READ total NOTIFY countsChanged)
    Q_PROPERTY(bool serverRunning READ serverRunning NOTIFY serverStatusChanged)
    Q_PROPERTY(bool cameraOnline READ cameraOnline NOTIFY cameraStatusChanged)
    Q_PROPERTY(QString lastUpdated READ lastUpdated NOTIFY countsChanged)
    Q_PROPERTY(QString serverError READ serverError NOTIFY serverStatusChanged)

public:
    explicit CountModel(QObject *parent = nullptr);

    int apples() const { return m_apples; }
    int oranges() const { return m_oranges; }
    int total() const { return m_apples + m_oranges; }
    bool serverRunning() const { return m_serverRunning; }
    bool cameraOnline() const { return m_cameraOnline; }
    QString lastUpdated() const { return m_lastUpdated; }
    QString serverError() const { return m_serverError; }

signals:
    void countsChanged();
    void serverStatusChanged();
    void cameraStatusChanged();

private:
    void acceptConnections();
    void readRequest(QTcpSocket *socket);
    void sendJson(QTcpSocket *socket, int status, const QByteArray &body);
    void updateCameraStatus();

    QTcpServer m_httpServer;
    QTimer m_cameraTimer;
    int m_apples = 0;
    int m_oranges = 0;
    bool m_serverRunning = false;
    bool m_cameraOnline = false;
    QDateTime m_lastUpload;
    QString m_lastUpdated = QStringLiteral("等待摄像头上传数据");
    QString m_serverError;
};
