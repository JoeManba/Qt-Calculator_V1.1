#include "rpnmanager.h"
#include <qmath.h>

//逆波兰表达式计算
//支持的双目运算符有:( ) ^ * / % + - << >> #(a#b 取a的第b个字节 b[0,3] 0从低位开始)
//支持的单目运算符有:+ - sin cos exp sqrt log ln abs tg ctg asin acos atg

RPNManager* RPNManager::instance=0;

RPNManager::RPNManager()

{

}

RPNManager::~RPNManager()
{

}



/// //是否为数字（0-9）或者小数点
/// \brief RPNManager::isDigitOrDot
/// \param c
/// \return
///
bool RPNManager::isDigitOrDot(QChar c)

{

    return (('0' <= c) && (c <= '9')) || (c == '.');

}

/// 判断是否是('a'~'z','A'~'Z'）字符
/// \brief RPNManager::isLetter
/// \param c
/// \return
///
bool RPNManager::isLetter(QChar c)
{
    return (('a' <= c) && (c <= 'z'))
            || (('A' <= c) && (c <= 'Z'));
}

/// 判断是否为非法字符
/// \brief RPNManager::isChineseLetter
/// \param c
/// \return
///
bool RPNManager::isBadChar(QChar c)
{
    ushort uni = c.unicode();


    if(uni > 0x007E || uni < 0x0021)//非法字符
    {
        return true;
    }
    else
    {
        return false;
    }
}

/// 是否为操作符或者括号
/// \brief RPNManager::isSymbol
/// \param c
/// \return
///
bool RPNManager::isSymbol(QChar c)

{

    return isOperator(c) || (c == '(') || (c == ')');

}

/// 是否为正负号
/// \brief RPNManager::isSign
/// \param c
/// \return
///
bool RPNManager::isSign(QChar c)

{

    return (c == '+') || (c == '-');

}


/// 是否为一个合法的数字
/// \brief RPNManager::isNumber
/// \param s
/// \return
///
bool RPNManager::isNumber(QString s)

{

    bool ret = false;

    s.toDouble(&ret);

    return ret;

}

/// 是否是运算符
/// \brief RPNManager::isOperator
/// \param s
/// \return
///
bool RPNManager::isOperator(QString s)

{

    return (s == "+") || (s == "-") || (s == "*")
            || (s == "/")||(s=="^")||(s=="%")||(s=="<")||(s==">")||(s=="#");

}

/** 是否是运算符
 * @brief RPNManager::isLetterOperator
 * @param s
 * @return
 */
bool RPNManager::isLetterOperator(QString s)
{
    return (s=="sqrt")||(s=="sin")||(s=="cos")||
            (s=="exp")||(s=="log")||(s=="abs")||
            (s=="ctg")||(s=="asin")||(s=="acos")||
            (s=="ln")||(s=="tg")||(s=="atg");
}

/// 是否为左括号
/// \brief RPNManager::isLeft
/// \param s
/// \return
///
bool RPNManager::isLeft(QString s)

{

    return (s == "(");

}

////是否为右括号
/// \brief RPNManager::isRight
/// \param s
/// \return
///
bool RPNManager::isRight(QString s)

{

    return (s == ")");

}


/// 检查公式
/// \brief RPNManager::checkExpression
/// \param exp
/// \return
///
bool RPNManager::checkFormulaExp(QString& exp,QVector<QString> &formaulaVec,QString &errorMsg,int dataSize)
{

    bool ret = false;
    int left = 0;
    int right = 0;


    ret = splitFormula(exp,formaulaVec,errorMsg);
    if(!ret)
    {
        return false;
    }

    for(int i=0;i<formaulaVec.size();i++)
    {
        if(i<formaulaVec.size()-1)
        {
            if(isLetterOperator(formaulaVec[i])&&formaulaVec[i+1]!="(")
            {
                errorMsg = formaulaVec[i]+" less '(',please check";
                return false;
            }
        }

        if(isLeft(formaulaVec[i])) left++;
        if(isRight(formaulaVec[i])) right++;

        if(formaulaVec[i].contains(PLACEHOLDER)&&formaulaVec[i].contains("+"))
        {
            formaulaVec[i].replace("+","");
        }
        if(formaulaVec[i].contains(PLACEHOLDER)&&formaulaVec[i].contains("-"))
        {
            formaulaVec[i].replace("-","");
        }

        if(!isLetterOperator(formaulaVec[i])&&!isSymbol(*formaulaVec[i].data())
                &&!isNumber(formaulaVec[i]))
        {
            if(formaulaVec[i].contains(PLACEHOLDER))
            {

                if(formaulaVec[i].trimmed().length()<=3)
                {

                }
                else
                {
                    QString num="";
                    QString parameter = formaulaVec[i];
                    for(int j=3;j<parameter.length();j++)
                    {
                        if((parameter[j]<'0') || (parameter[j] > '9'))
                        {
                            errorMsg = PLACEHOLDER + " include other letter";
                            return false;
                        }
                        num += parameter[j];
                    }
                    int index = num.trimmed().toInt();
                    if(index>dataSize-1)
                    {
                        errorMsg = PLACEHOLDER + " Index Out Of Bounds Exception";
                        return false;
                    }
                }
            }
            else
            {
                errorMsg = "Include other unknow parameter,Please check!\n"
                           "Tips:The format of the placeholder is:"+PLACEHOLDER+"0,"+PLACEHOLDER+"1...";
                return false;
            }
        }
    }
    if(left!=right)
    {
        errorMsg = "Please check '(' or ')'";
        return false;
    }
    QVector<QString> repolistVec = getRepolish(formaulaVec);
    if(repolistVec.count()<=0)
    {
        errorMsg = "Please enter the calculation formula ";
        return false;
    }

    ret = checkRPN(repolistVec,errorMsg);
    return ret;
}

/// <summary>
/// 验证得到的逆波兰表达式列表的正确性
/// </summary>
/// <param name="exp">表达式列表</param>
/// <returns>正确返回true，错误返回false</returns>
bool RPNManager:: checkRPN(QVector<QString>& rpns,QString &errorMsg)
{
    if(rpns.count()<=0)
    {
        errorMsg = "Please enter the calculation formula ";
        return false;
    }
    QStack<QString> result;  //运算符

    try
    {
        for (int i = 0; i < rpns.size(); i++)
        {
            if (isNumber(rpns[i])||rpns[i].contains(PLACEHOLDER))
            {
                result.push(rpns[i]);
            }

            else
            {
                if(isLetter(rpns[i][0]))
                {
                    if(!isLetterOperator(rpns[i])) return false;
                }
                int level = Priority(rpns[i]);  //获得运算符的优先级
                if (level == 4)                 //说明是单目运算符
                {
                    if(result.empty()) return false;
                    QString topItem = result.top();  //弹出栈顶元素
                    result.pop();
                    result.push(rpns[i] + "(" + topItem + ")");   //压入栈
                }
                else   //说明是双目运算符
                {
                    if(result.empty())
                    {
                        errorMsg = "Wrong formula";
                        return false;
                    }
                    QString topItem1 = result.top(); result.pop();  //弹出栈顶元素
                    if (result.empty())
                    {
                        errorMsg = "Wrong formula";
                        return false;
                    }
                    QString topItem2 = result.top(); result.pop();  //弹出栈顶元素
                    result.push("(" + topItem2 + rpns[i] + topItem1 + ")");   //压入栈
                }
            }
        }
        //看看result最后是不是还剩一个元素
        result.pop();
        if (result.empty())return true;
        errorMsg = "Wrong formula";
        return false;
    }
    catch (...)
    {
        errorMsg = "Wrong formula";
        return false;
    }

}

//// 分离符号和数字
/// \brief RPNManager::split
/// \param exp
/// \return
///
bool RPNManager::splitFormula(QString exp,QVector<QString> &formaulaVec,QString &errorMsg)
{

    QString num = "";
    QString pre = "";
    QString letter;

    for(int i=0; i<exp.length(); i++)
    {
        if(isBadChar(exp[i]))//非法字符校验
        {
            errorMsg = "The formula contains illegality characters: "+QString(exp[i]);

            return false;
        }

        if( isDigitOrDot(exp[i]))//数字或者小数点
        {
            if(!letter.isEmpty())
            {
                if(letter.contains(PLACEHOLDER))
                {
                    letter += exp[i];
                    continue;
                }
                formaulaVec.append(letter);
                letter.clear();
            }
            num += exp[i];
            pre = exp[i]; //为判断+，-是否为正负号，而使用的
        }

        else if( isLetter(exp[i]) )
        {
            if( !num.isEmpty() )
            {
                if(num=="+"||num=="-")
                {
                    letter += num;
                }
                else
                {
                    formaulaVec.append(num);//分离的数字入队列
                }

                num.clear();
            }
            letter += exp[i];
            if(isLetterOperator(letter))
            {
                formaulaVec.append(letter);//分离的specific Symbol入队列
                letter.clear();
            }
            pre = exp[i];
        }
        else if( isSymbol(exp[i]) )
        {
            if( !num.isEmpty() )
            {
                formaulaVec.append(num);//分离的数字入队列
                num.clear();
            }

            if( !letter.isEmpty() )
            {
                formaulaVec.append(letter);//分离的specific Symbol入队列
                letter.clear();
            }

            if( isSign(exp[i]) && ((pre == "") || (pre == "(") || isOperator(pre)||isLetterOperator(formaulaVec.last())) )
            {
                num += exp[i];//存储正负号
                pre = exp[i];//保存上一个字符
                continue;
            }
            if(pre =="<"&&exp[i]=="<")
            {
                formaulaVec[formaulaVec.size()-1] = "<<";
                pre = exp[i];//保存上一个字符
                continue;
            }

            if(pre ==">"&&exp[i]==">")
            {
                formaulaVec[formaulaVec.size()-1] = ">>";
                pre = exp[i];//保存上一个字符
                continue;
            }
            else
            {
                formaulaVec.append(""+exp[i]); // 运算符入队列
            }
            pre = exp[i];//保存上一个字符
        }
        else
        {
            errorMsg = "The formula contains illegality characters: "+QString(exp[i]);

            return false;
        }
    }
    if( !num.isEmpty() )
    {
        formaulaVec.append(num);//最后一个运算数
    }
    else if(!letter.isEmpty())
    {
        formaulaVec.append(letter);
    }

    if(formaulaVec.count()<=0)
    {
        errorMsg = "Please enter the calculation formula ";
        return false;
    }
    return true;
}

////*获取操作符优先级*/
/// \brief RPNManager::Priority
/// \param data
/// \return
///
int RPNManager::Priority(QString data)
{
    int priority;
    if(data == "(")
        priority = 1;
    else if(data == "+" || data == "-")
        priority = 2;
    else if(data == "*" || data == "/"
            ||data == "<<"||data == ">>"
            ||data=="%"||data=="^"||(data=="#"))
        priority = 3;
    else if(data == "sin" ||
            data == "cos" ||
            data == "exp" ||
            data == "sqrt" ||
            data == "log" ||
            data == "ln" ||
            data == "abs" ||
            data == "tg" ||
            data == "ctg" ||
            data == "asin" ||
            data == "acos" ||
            data == "atg" )
    {
        priority = 4;
    }
    else if (data == ")")
        priority = 5;
    else
        priority = -1;
    return priority;
}

////*将获取到的分割好的表达式数组，转化为逆波兰表达式，存入数组repolish中*/
/// \brief RPNManager::getRepolish
/// \param forlumaVec
/// \return
///
QVector<QString> RPNManager::getRepolish(QVector<QString> forlumaVec)
{
    QVector<QString> repolish;
    QStack<QString> st2;
    int i = 0;
    for(int j = 0 ; j < forlumaVec.size() ; j++)
    {
        if(forlumaVec[j] != "(" && forlumaVec[j] != ")" &&
                forlumaVec[j] != "+" && forlumaVec[j] != "-" &&
                forlumaVec[j] != "*" && forlumaVec[j] != "/"&&
                forlumaVec[j] != "^" && forlumaVec[j] != "%"
                &&forlumaVec[j]!="#"
                && forlumaVec[j] != ">>"&&forlumaVec[j] != "<<"&&
                forlumaVec[j] != "sqrt"&& forlumaVec[j] != "sin"&&
                forlumaVec[j] != "cos" && forlumaVec[j] != "exp"&&
                forlumaVec[j] != "log" && forlumaVec[j] != "ln"&&
                forlumaVec[j] != "abs"&&forlumaVec[j] != "tg"&&
                forlumaVec[j] != "ctg"&& forlumaVec[j] != "asin"&&
                forlumaVec[j] != "acos"&& forlumaVec[j] != "atg")
        {
            repolish.append(forlumaVec[j]);
            i++;
        }

        else if(forlumaVec[j] == "(")
        {
            st2.push(forlumaVec[j]);
        }
        else if(forlumaVec[j] == ")")
        {
            while(st2.top() != "(")
            {
                repolish.append(st2.top());
                st2.pop();
            }
            if(st2.top() == "(")
                st2.pop();
        }
        else if(st2.empty() || Priority(forlumaVec[j]) > Priority(st2.top()))
        {
            st2.push(forlumaVec[j]);
        }
        else
        {
            while(Priority(forlumaVec[j]) <= Priority(st2.top()))
            {
                repolish.append(st2.top());
                i++;
                st2.pop();
                if(st2.empty())
                    break;
            }
            st2.push(forlumaVec[j]);
        }
    }
    while(!st2.empty())
    {
        repolish.append(st2.top());
        i++;
        st2.pop();
    }
    return repolish;
}

/** 计算公式值并显示
 * @brief RPNManager::getRepolishResult
 * @param repolish 公式
 * @param result 结果
 * @param errorMsg 错误信息
 * @return
 */
bool RPNManager::getRepolishResult(QVector<QString> repolish,double &result,QString &errorMsg)
{

    QStack <double> st;
    for(int m = 0 ; m < repolish.size() ; m ++)
    {
        if(repolish[m] != "+" && repolish[m] != "-" &&
                repolish[m] != "*" && repolish[m] != "/"&&
                repolish[m] != "^" && repolish[m] != "%"&&(repolish[m]!="#")
                && repolish[m] != ">>"&&repolish[m] != "<<"&&
                repolish[m] != "sqrt"&& repolish[m] != "sin"&&
                repolish[m] != "cos" && repolish[m] != "exp"&&
                repolish[m] != "log" && repolish[m] != "ln"
                && repolish[m] != "abs"&&repolish[m] != "tg"&&
                repolish[m] != "ctg"&& repolish[m] != "asin"&&
                repolish[m] != "acos"&& repolish[m] != "atg")
        {
            /*Qstring转化为double数据存入堆栈*/
            st.push(repolish[m].toDouble());
        }
        else
        {
            if(repolish[m] == "+")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push(b + a);
            }
            else if(repolish[m] == "-")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push(b - a);
            }
            else if(repolish[m] == "*")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push(b * a);
            }
            else if(repolish[m] == "/")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                if(a != 0)
                    st.push(b/a);
                else
                {
                    errorMsg = "0 不能做除数";
                    return false;
                }
            }
            else if(repolish[m] == "#")
            {
                int a = st.top();
                st.pop();
                int b = st.top();
                st.pop();
                if(a<0||a>3)
                {
                    errorMsg = "# Index Out Of Bounds Exception";
                    return false;

                }
                st.push((int)int2Bytes(b)[a]);
            }
            else if(repolish[m] == "^")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push(pow(b,a));
            }
            else if(repolish[m] == "%")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push((int)b%(int)a);
            }

            else if(repolish[m] == "<<")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push((int)b<<(int)a);
            }

            else if(repolish[m] == ">>")
            {
                double a = st.top();
                st.pop();
                double b = st.top();
                st.pop();
                st.push((int)b>>(int)a);
            }
            else if(repolish[m] == "sqrt")
            {
                double a = st.top();
                st.pop();
                if(a>=0)
                {
                    st.push(sqrt(a));
                }
                else
                {
                    errorMsg = "can't sqrt minus";
                    return false;
                }
            }

            else if(repolish[m] == "sin")
            {
                double a = st.top();
                st.pop();
                st.push(sin(a));
            }
            else if(repolish[m] == "cos")
            {
                double a = st.top();
                st.pop();
                st.push(cos(a));

            }
            else if(repolish[m] == "exp")
            {
                double a = st.top();

                st.push(exp(a));

            }
            else if(repolish[m] == "log")
            {
                double a = st.top();
                st.pop();
                st.push(log10(a));

            }
            else if(repolish[m] == "ln")
            {
                double a = st.top();
                st.pop();
                st.push(log(a));

            }
            else if(repolish[m] == "tg")
            {
                double a = st.top();
                st.pop();
                st.push(tan(a));
            }
            else if(repolish[m] == "abs")
            {
                double a = st.top();
                st.pop();
                st.push(abs(a));
            }
            else if(repolish[m] == "ctg")
            {
                double a = st.top();
                st.pop();
                st.push(atan(a));
            }
            else if(repolish[m] == "asin")
            {
                double a = st.top();
                st.pop();
                st.push(asin(a));
            }
            else if(repolish[m] == "acos")
            {
                double a = st.top();
                st.pop();
                st.push(acos(a));

            }
            else if(repolish[m] == "atg")
            {
                double a = st.top();
                st.pop();
                st.push(1.0/tan(a));
            }
        }
    }
    //    return st.top();
    result = st.top();
    return true;
}

QByteArray RPNManager::  int2Bytes(int value)
{
    QByteArray src ;
    src.resize(4);
    src[0] =  value & 0xFF;
    src[1] =  (value>>8) & 0xFF;
    src[2] =  (value>>16) & 0xFF;
    src[3] =  (value>>24) & 0xFF;
    return src;
}

/// 获取计算结果
/// \brief RPNManager::getResult
/// \param forluma 公式
/// \param parameterValueVec ans 对应值
/// \param currentIndex 当前index
/// \param result 逆波兰式计算结果
/// \param errorMsg 错误信息
/// \return 是否计算成功
///
bool RPNManager:: getResult(QString forluma,QVector<QString> &parameterValueVec,int currentIndex,double &result,QString &errorMsg)
{

    if(forluma.length()<=0){
        return false;
    }
    //分离符号和数字
    QVector<QString> forlumaVec;
    bool check = checkFormulaExp(forluma,forlumaVec,errorMsg,parameterValueVec.count());
    if(!check)
    {
        return false;
    }


//    QVector<QString> forlumaVec = splitFormula(forluma);
    for(int i=0;i<forlumaVec.size();i++)
    {
        bool isMinus = false;
        if(forlumaVec[i].contains(PLACEHOLDER)&&forlumaVec[i].contains("+"))
        {
            forlumaVec[i].replace("+","");
        }
        if(forlumaVec[i].contains(PLACEHOLDER)&&forlumaVec[i].contains("-"))
        {
            isMinus = true;
            forlumaVec[i].replace("-","");
        }

        if(!isLetterOperator(forlumaVec[i])&&!isSymbol(*forlumaVec[i].data())
                &&!isNumber(forlumaVec[i]))
        {
            if(forlumaVec[i].contains(PLACEHOLDER))
            {

                if(forlumaVec[i].trimmed().length()<=3)
                {
                    forlumaVec[i] = parameterValueVec[currentIndex];
                    if(isMinus) forlumaVec[i] = "-" + parameterValueVec[currentIndex];

                }
                else
                {
                    QString num="";
                    QString parameter = forlumaVec[i];
                    for(int j=3;j<parameter.length();j++)
                    {
                        if((parameter[j]<'0') || (parameter[j] > '9'))
                        {
                            errorMsg = PLACEHOLDER + " include other letter";
                            return false;
                        }
                        num += parameter[j];
                    }
                    int index = num.trimmed().toInt();
                    if(index>parameterValueVec.size()-1)
                    {
                        errorMsg = PLACEHOLDER+num+ " Index Out Of Bounds Exception";
                        return false;
                    }
                    forlumaVec[i] = parameterValueVec[index];
                    if(isMinus)
                    {
                        forlumaVec[i] = QString::number(0-forlumaVec[i].toInt());
                    }
                }
            }
            else
            {
                errorMsg = "Include other unknow parameter,please check";
                return false;
            }
        }
    }

    QVector<QString> repolishVec = getRepolish(forlumaVec);
    return getRepolishResult(repolishVec,result,errorMsg);
}
