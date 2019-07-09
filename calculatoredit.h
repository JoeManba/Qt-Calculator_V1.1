#ifndef CALCULATOREDIT_H
#define CALCULATOREDIT_H
#include <QLineEdit>

class CalculatorEdit : public QLineEdit
{
    Q_OBJECT
public:
    CalculatorEdit(QWidget *parent = nullptr);
    ~CalculatorEdit();
//   virtual  void focusInEvent(QFocusEvent *e) ;
protected:
   virtual void focusInEvent(QFocusEvent *e);
   virtual void focusOutEvent(QFocusEvent *e) ;


signals:
    void focusInByteEdit();
    void focusOutByteEdit();
};

#endif // CALCULATOREDIT_H
