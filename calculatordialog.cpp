#include "calculatordialog.h"
#include "inputmethodcnmanager.h"
#include "rpnmanager.h"
#include "softkeyboard.h"
#include "ui_calculatordialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <qlineedit.h>
#include <QDesktopWidget>
#include <QString>
#include <QDebug>
#define WINDOW_HEIGHT QApplication::desktop()->availableGeometry().height()//QApplication::desktop()->screenGeometry().height()

CalculatorDialog::CalculatorDialog(QWidget *parent) :
    QDialog(parent)
{

    QWidget *titleBar=new QWidget(this);
    titleBar->setFixedHeight(60);

    titleBar->setStyleSheet("color:white;"
                            "background-color:rgb(80,80,80);"
                            "font-size:32px");

    QLabel *title=new QLabel(titleBar);
    title->setFixedHeight(60);
    title->setText(tr("公式计算器"));

    QPushButton *closeBtn=new QPushButton(QIcon(":/img/Resources/Close.png"),"",titleBar);
    closeBtn->setFixedSize(60,60);
    closeBtn->setIconSize(QSize(50,50));
    connect(closeBtn,SIGNAL(pressed()),this,SLOT(close()));

    QHBoxLayout *titleBarLayout=new QHBoxLayout;
    titleBarLayout->addWidget(title);
    titleBarLayout->addStretch();
    titleBarLayout->addWidget(closeBtn);
    titleBarLayout->setContentsMargins(8,0,0,0);
    titleBar->setLayout(titleBarLayout);

    QLabel *inputLabel = new QLabel("请输入：",this);
    inputLabel->setStyleSheet("color:white;"
                              "font-size:32px");
    inputEdit = new QLineEdit(this);
    inputEdit->setFixedSize(500,35);
    inputEdit->setStyleSheet("color:white;"
                              "font-size:26px");
    inputEdit->installEventFilter(this);
    keyboard = new SoftKeyboard(this->inputEdit);

    QPushButton *calculateBtn = new QPushButton("计算",this);
    calculateBtn->setStyleSheet("color:white;"
                              "font-size:26px");
    calculateBtn->setFixedSize(80,35);
    QLabel *resultTipLabel = new QLabel("结  果：",this);
    resultTipLabel->setStyleSheet("color:white;"
                              "font-size:32px");
    resulLabel = new QLabel(this);
    resulLabel->setStyleSheet("color:white;"
                              "font-size:32px");

    QGridLayout *calculatorLayout = new QGridLayout();
    calculatorLayout->addWidget(inputLabel,0,0);
    calculatorLayout->addWidget(inputEdit,0,1);
    calculatorLayout->addWidget(calculateBtn,0,2);
    calculatorLayout->addWidget(resultTipLabel,1,0);
    calculatorLayout->addWidget(resulLabel,1,1);

    connect(calculateBtn,SIGNAL(pressed()),this,SLOT(calculatorResult()));
//    connect(inputEdit,SIGNAL(focusInEdit()),this,SLOT(showKeyboard()));


    /*----MainLayout-----*/
    QVBoxLayout *mainLayout=new QVBoxLayout;
    mainLayout->addWidget(titleBar);
    mainLayout->addLayout(calculatorLayout);


    mainLayout->setMargin(15);
    mainLayout->setContentsMargins(0,0,0,10);
    setLayout(mainLayout);
    setStyleSheet("background-color:gray;");
    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);//一定要加上

}

void CalculatorDialog:: calculatorResult(){

//    hidenKeyboard();
    resulLabel->setText(inputEdit->text());
    QString temStr;
    QString forluma = inputEdit->text();
    QVector<QString> parameterValueVec;
    parameterValueVec.append("12");
    parameterValueVec.append("1");
    parameterValueVec.append("2");
//    int currentIndex = 0;
    double result=0;
    QString errorMsg;

    bool re = RPNManager::getInstance()->getResult(forluma,parameterValueVec,0,result,errorMsg);
    if(re)
    {
        resulLabel->setText(temStr.setNum(result));
        resulLabel->setStyleSheet("color:white;"
                                  "font-size:32px");
    }else
    {
        resulLabel->setText(errorMsg);
        resulLabel->setStyleSheet("color:red;"
                                  "font-size:32px");
    }
    hidenKeyboard();
}

bool CalculatorDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == inputEdit)
    {
        if(event->type() == QEvent::FocusIn)
        {
            showKeyboard();
            inputEdit->clearFocus();//防止inputEdit重新获得焦点再一次激发事件
        }
        else if(event->type() == QEvent::FocusOut)
        {

        }

    }

    return QWidget::eventFilter(watched, event);
}

int i=0;
void CalculatorDialog:: showKeyboard(){
    QString temStr;
    keyboard->show();
    qDebug()<<"Fouce in:"<<temStr.setNum(i++);
    keyboard->move(keyboard->x(), WINDOW_HEIGHT - keyboard->height());
}

int j=0;
void CalculatorDialog:: hidenKeyboard(){
    QString temStr;
    keyboard->quitBtnSlot();

    qDebug()<<"Fouce out:"<<temStr.setNum(j++);
}

CalculatorDialog::~CalculatorDialog()
{
    keyboard->destroyed();

}
