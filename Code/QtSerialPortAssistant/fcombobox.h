#ifndef FCOMBOBOX_H
#define FCOMBOBOX_H

#include <QComboBox>

class FComboBox : public QComboBox
{
    Q_OBJECT
public:
    FComboBox(QWidget *parent = 0);

signals:
    pressed(void);

protected:
    void mousePressEvent(QMouseEvent* event);


};

#endif // FCOMBOBOX_H
