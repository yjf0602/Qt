#include "fcombobox.h"
#include <QMouseEvent>

FComboBox::FComboBox(QWidget *parent) :
    QComboBox(parent)
{

}

void FComboBox::mousePressEvent(QMouseEvent* event)
{
    emit pressed();
    QComboBox::mousePressEvent(event);
}








