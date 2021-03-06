#ifndef XMLCONFIGMIGRATION_H
#define XMLCONFIGMIGRATION_H

#include <QObject>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "common.h"

class XMLConfigMigration : public QObject
{
    Q_OBJECT
public:
    explicit XMLConfigMigration(QXmlStreamReader *reader, QObject *parent = 0);

    bool requiresMigration();
    QString migrate();

protected:
    QXmlStreamReader *reader;
    int fileVersion;

private:
    QString readConfigToString();
    QString version0006Migration();

signals:

public slots:

};

#endif // XMLCONFIGMIGRATION_H
