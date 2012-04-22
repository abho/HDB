#ifndef HDB_H
#define HDB_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtCore>
#include <QtGui>

class HDB : public QObject
{
    Q_OBJECT
public:
    enum Typ{
        Int,Char,Real
    };

    explicit HDB(QObject *parent = 0);
    void startTransaction();
    void stopTransaction();
    bool open(const QString &name );
    bool createTableIfNoExist(const QString &tableName,const QList<QPair<QString,Typ> > &list,const QStringList &primary = QStringList());
    bool insert(const QString &tableName,const QList<QPair<QString,QVariant> > &list,bool replace = false);
    QSqlQuery select(const QStringList &tableNames,bool &ok ,const QHash<QString,QStringList> &columnNames =QHash<QString,QStringList>(),const QString &where= QString());
    bool update(const QString &tableName,const QList<QPair<QString,QVariant> > &list, const QString & where);
    bool deleteRow(const QString &tableName,const QString &where);
    QSqlError lastError();

signals:

public slots:

private:
    QSqlDatabase mDB;
    QString mLastError;
    QHash<QString, QHash<QString, Typ> > mTables;

};

#endif // HDB_H
