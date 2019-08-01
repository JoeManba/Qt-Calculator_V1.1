#ifndef CALCULATORDIALOG_H
#define CALCULATORDIALOG_H

#include "customedit.h"
#include "softkeyboard.h"

#include <QDialog>
#include <qlabel.h>
#include <qlineedit.h>


namespace Ui {
class CalculatorDialog;
}

class CalculatorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CalculatorDialog(QWidget *parent = 0);
    ~CalculatorDialog();

private:
    QLabel *resulLabel;//结果显示区
    QLineEdit *inputEdit;//公式编辑区
    SoftKeyboard *keyboard ;//软键盘
protected:
    bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void calculatorResult();
    void showKeyboard();
    void hidenKeyboard();
};

#endif // CALCULATORDIALOG_H
