#ifndef PINYININPUTMETHOD_H
#define PINYININPUTMETHOD_H

#include <QObject>
#include <QStringList>

class GooglePinyinManager: public QObject
{
    Q_OBJECT
public:
    GooglePinyinManager();
    ~GooglePinyinManager();

signals:
    void pinyinTextChanged(const QString&);

public:
    void SearchCN(const QString &gemfield);
    void BinarySearchEN(const QString &gemfield);
    void Matching(const QString &gemfield, bool isEn = true);
    QStringList ChinieseModel;

private:
    QStringList lstEN;
};

#endif
