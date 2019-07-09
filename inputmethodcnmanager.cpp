#include "inputmethodcnmanager.h"

#include <QMessageBox>
#include <qfile.h>
#include <QDebug>
#include <qmessagebox.h>

InputMethodCNManager* InputMethodCNManager::instance=0;
InputMethodCNManager::InputMethodCNManager()
{

}

void InputMethodCNManager:: initChinese()
{
    pinyin_file.setFileName(":/dict/ziku/liimoupinyin.dat");
    if(!pinyin_file.open(QIODevice::ReadOnly| QIODevice::Text))
    {
        QMessageBox::warning(0,"输入法警告","无法加载中文输入法！");
    }
    while (!pinyin_file.atEnd())
    {
        char buf[1024];
        qint64 len = pinyin_file.readLine(buf, sizeof(buf));
        if (len <= 0)
        {
            break;
        }
        QString str = QString::fromLocal8Bit(buf).split("\n").at(0);


        QString tempKey;
        QString tempValue;

        for(int i = 0 ; i < str.count() ; i++)
        {
            QChar cha = str.at(i);
            ushort uni = cha.unicode();


            if(uni >= 0x4E00 && uni <= 0x9FA5)//中文字符
            {
                tempValue.append(cha);

            }
            else if(uni >= 0x0061 && uni <= 0x007A)//a-z
            {
                tempKey.append(cha);
            }
            else if(uni >= 0x0041 && uni <= 0x005A)//A-Z
            {
                tempKey.append(cha);
            }
            else{//其他

            }
        }

        pinyin_map.insert(tempKey,tempValue);
    }
}

QList<QString> InputMethodCNManager::matchPinyin(QString gemfield)
{
    QList<QString> tempList;
    tempList = pinyin_map.values(gemfield);
    return tempList;
}
