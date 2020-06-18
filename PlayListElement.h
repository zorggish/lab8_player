#ifndef PLAYLISTELEMENT_H
#define PLAYLISTELEMENT_H

#include <QUrl>
#include <QString>

class PlayListElement
{
public:
    QUrl path;
    QString filename;
    qint64 seconds;
};

#endif // PLAYLISTELEMENT_H
