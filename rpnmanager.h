#ifndef QCALCULATORDEC_H
#define QCALCULATORDEC_H

#include <QString>

#include <QStack>

#include <QMutex>
#include <QQueue>

class RPNManager
{
private:

    //占位符
    const QString PLACEHOLDER = "ans";

    static RPNManager *instance;


    //数字或小数点
    bool isDigitOrDot(QChar c);
    //判断是否是('a'~'z','A'~'Z'）字符
    bool isLetter(QChar c);

    //判断是否为非法字符
    bool isBadChar(QChar c);

    //判断是否为操作符或者括号
    bool isSymbol(QChar c);

    // 判断是否为正负号
    bool isSign(QChar c);

    // 判断是否是数字
    bool isNumber(QString s);

    // 判断是否是运算符
    bool isOperator(QString s);

    // 判断是否是运算符
    bool isLetterOperator(QString s);

    //判断s是否为左括号
    bool isLeft(QString s);

    //判断s是否为右括号
    bool isRight(QString s);

    //获取操作符优先级
    int Priority(QString data);
    // int 转 byte
    QByteArray int2Bytes(int value) ;

    // 将获取到的分割好的表达式数组，转化为逆波兰表达式，并返回
    QVector<QString> getRepolish(QVector<QString> forlumaVec);

    //分离符号和数字
    bool splitFormula( QString exp,QVector<QString> &formaulaVec,QString &errorMsg);

    //验证得到的逆波兰表达式列表的正确性
    bool checkRPN(QVector<QString>& rpns,QString &errorMsg);

    //计算公式的值
    bool getRepolishResult(QVector<QString> repolish,double &result,QString &errorMsg);
    RPNManager();

public:

    ~RPNManager();

    static RPNManager* getInstance()
    {
        static QMutex mutex;
        if (!instance) {
            QMutexLocker locker(&mutex);
            if (!instance)
                instance = new RPNManager();
        }
        return instance;
    }

    //检查公式
    bool checkFormulaExp(QString & exp,QVector<QString> &formaulaVec,QString &errorMsg,int dataSize);

    // 获取计算结果
    bool getResult(QString forluma,QVector<QString> &parameterValueVec,int currentIndex,double &result,QString &errorMsg);
};

#endif // QCALCULATORDEC_H
