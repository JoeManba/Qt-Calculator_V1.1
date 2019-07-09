#ifndef CHINESEMANAGER_H
#define CHINESEMANAGER_H
#include <QMultiMap>
#include <QMutex>
#include <QString>
#include <qfile.h>
#include <qstringlist.h>

class InputMethodCNManager
{
private:
    InputMethodCNManager();
    static InputMethodCNManager *instance;
    void initChinese();
    //中文输入法字库
    QFile pinyin_file;

    QMultiMap<QString,QString> pinyin_map;



public:
    static InputMethodCNManager* GetInstance()
    {
        static QMutex mutex;
        if (!instance) {
            QMutexLocker locker(&mutex);
            if (!instance)
                instance = new InputMethodCNManager();
            instance->initChinese();
        }

        return instance;
    }
    QList<QString> matchPinyin(QString field);
};

#endif // CHINESEMANAGER_H
