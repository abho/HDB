#include "hdb.h"
#include <QDebug>
HDB::HDB(QObject *parent) :
    QObject(parent)
{
    mDB = QSqlDatabase::addDatabase("QSQLITE");

}

void HDB::startTransaction()
{
    mDB.transaction();
}

void HDB::stopTransaction()
{
    mDB.commit();
}

bool HDB::open(const QString &name)
{
    mDB.setDatabaseName(name);
   return  mDB.open();
}

bool HDB::createTableIfNoExist(const QString &tableName, const QList<QPair<QString, Typ> > &list, const QStringList &primary)
{   
    QSqlQuery q;
    QString befehl("create table if not exists " +tableName+" (");
    QHash<QString,Typ> hash;
    Typ t;
    for (int var = 0; var < list.size(); ++var) {
        befehl.append(list.at(var).first+" ");
        t = list.at(var).second;
        switch(t){
        case HDB::Int:
            befehl.append("int,");
            break;
        case HDB::Char:
            befehl.append("varchar(100),");
            break;
        case HDB::Real:
            befehl.append("double,");
            break;

        }
        hash.insert(list.at(var).first,t);
    }
    mTables.insert(tableName,hash);
    if(primary.isEmpty())
    befehl.chop(1);
    else {
        befehl.append("PRIMARY KEY (");
        for (int var = 0; var < primary.count(); ++var) {
            befehl.append(primary.at(var)+",");
        }
        befehl.chop(1);
        befehl.append(")");
    }
    befehl.append(")");
    return q.exec(befehl);
}

bool HDB::insert(const QString &tableName, const QList<QPair<QString, QVariant> > &list, bool replace)
{
    const QHash<QString,Typ> &hash = mTables.value(tableName);
        QString befehl;
    if(replace){
        befehl.append("replace " );
    }else {
        befehl.append("insert ");
    }
   befehl.append("into "+tableName+" (");
    QString values("values (");
    for (int var = 0; var < list.size(); ++var) {
        befehl.append(list.at(var).first+", ");
        values.append("?, ");
    }
    befehl.chop(2);
    befehl.append(")");
    values.chop(2);
    values.append(")");

    QSqlQuery q;
    q.prepare(befehl+values);
    Typ t;
    for (int var = 0; var < list.size(); ++var) {
        const QPair<QString,QVariant> &pair = list.at(var);
        t= hash.value(pair.first);
        switch(t){
        case HDB::Int:
            q.bindValue(var,pair.second.toInt());
            break;
        case HDB::Char:
            q.bindValue(var,pair.second.toString());
            break;
        case HDB::Real:
            q.bindValue(var,pair.second.toDouble());
            break;
        }
    }
    qDebug() << Q_FUNC_INFO << befehl << q.boundValues();
    bool ret = q.exec();
    if(!ret)
        mLastError = q.lastError().text();
    return ret;

}

QSqlQuery  HDB::select(const QStringList &tableNames,bool &ok ,const QHash<QString,QStringList> &columnNames,const QString &where)
{
    ok = true;
    QString befehl;
    befehl.append("select ");
    for (int var = 0; var < tableNames.size(); ++var) {
        const QString &name = tableNames.at(var);
        const QStringList& l = columnNames.value(name);
        if(l.isEmpty()){
            befehl.append(name+".*,");
        }else {
            for (int i = 0; i < l.size(); ++i) {
                befehl.append(name+"."+l.at(i)+",");
            }
        }
    }
    befehl.chop(1);
    befehl.append(" from ");
    for (int var = 0; var < tableNames.size(); ++var) {
        befehl.append(tableNames.at(var)+",");
    }
    befehl.chop(1);
    if(!where.isEmpty()){
        befehl.append(" where "+where);
    }

    QSqlQuery query;
    if(!query.exec(befehl)){
        ok = false;
        qDebug() << query.lastError().text();
    }
    return query;
}

bool HDB::update(const QString &tableName, const QList<QPair<QString, QVariant> > &list, const QString &where)
{


qDebug() << Q_FUNC_INFO << where;
    const QHash<QString,Typ> &hash = mTables.value(tableName);

    QString befehl("update "+tableName+" set ");

    for (int var = 0; var < list.size(); ++var) {
        befehl.append(list.at(var).first+" = ?,");
    }
    befehl.chop(1);
    if(!where.isEmpty())
        befehl.append(" where "+where);
    QSqlQuery q;
    q.prepare(befehl);
    Typ t;
    for (int var = 0; var < list.size(); ++var) {
        const QPair<QString,QVariant> &pair = list.at(var);
        t= hash.value(pair.first);
        switch(t){
        case HDB::Int:
            q.bindValue(var,pair.second.toInt());
            break;
        case HDB::Char:
            q.bindValue(var,pair.second.toString());
            break;
        case HDB::Real:
            q.bindValue(var,pair.second.toDouble());
            break;
        }
    }
    qDebug() << Q_FUNC_INFO << befehl;
    return q.exec();
}

bool HDB::deleteRow(const QString &tableName, const QString &where)
{
     QString befehl("delete from "+tableName);
     if(!where.isEmpty())
         befehl.append(" where "+where);
     return QSqlQuery(befehl).exec();
}

QSqlError  HDB::lastError()
{
    return mLastError;
}




