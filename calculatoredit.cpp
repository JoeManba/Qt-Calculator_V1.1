#include "calculatoredit.h"



CalculatorEdit::CalculatorEdit(QWidget *parent):QLineEdit(parent)
{

}

void CalculatorEdit:: focusInEvent(QFocusEvent *e)
{
    emit focusInByteEdit();
}

void CalculatorEdit:: focusOutEvent(QFocusEvent *e)
{
    emit focusOutByteEdit();
}

CalculatorEdit::~CalculatorEdit()
{

}
