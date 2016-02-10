#include "SpinBox.hpp"

SpinBox::SpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
    setDecimals(3);
}

QString SpinBox::textFromValue(double value) const
{
    double n = value;
    int precision = 0;

    for(; precision < decimals(); ++precision)
    {
        if(std::abs(std::floor(n) - n) < std::pow(10, -decimals()))
           break;
        n *= 10;
    }

    return QWidget::locale().toString(value, 'f', precision);
}

bool SpinBox::valueEquals(double n) const
{
    return std::abs(value() - n) < std::pow(10, -decimals());
}
