#include "customedit.h"

CustomEdit::CustomEdit(QWidget *parent) : QLineEdit(parent)
{

}
CustomEdit::~CustomEdit()
{

}

void CustomEdit:: focusInEvent(QFocusEvent *e)
{
    emit focusInEdit();
}

void CustomEdit:: focusOutEvent(QFocusEvent *e)
{
    emit focusOutEdit();
}

