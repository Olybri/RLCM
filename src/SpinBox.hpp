#ifndef SPINBOX_H
#define SPINBOX_H

#include <QDoubleSpinBox>

class SpinBox : public QDoubleSpinBox
{
    public:
        SpinBox(QWidget *parent = nullptr);

        virtual QString	textFromValue(double value) const override;

        bool valueEquals(double n) const;
};

#endif // SPINBOX_H
