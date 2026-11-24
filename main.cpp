#include "countmodel.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("智能采摘 · 果实计数");
    app.setOrganizationName("FruitHarvest");

    CountModel counts;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("countModel", &counts);
    engine.loadFromModule("FruitHarvest", "Main");
    if (engine.rootObjects().isEmpty())
        return -1;
    return app.exec();
}
