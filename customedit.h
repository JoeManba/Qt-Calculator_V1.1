#ifndef CUSTOMEDIT_H
#define CUSTOMEDIT_H

#include <QLineEdit>

class CustomEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomEdit(QWidget *parent = nullptr);
    ~CustomEdit();

protected:
   virtual void focusInEvent(QFocusEvent *e);
   virtual void focusOutEvent(QFocusEvent *e) ;
signals:
    void focusInEdit();
    void focusOutEdit();
};

#endif // CUSTOMEDIT_H
