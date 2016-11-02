#include "OutputStream.hpp"

void ListWidget::addLine(QString line, QColor color)
{
    for(int i = maxCharCount; i < line.size(); i += maxCharCount + 1)
        line.insert(i, '\n');

    addItem(line);
    item(count() - 1)->setTextColor(color);

    if(count() > maxItemCount)
        delete item(0);
}

OutputStream::OutputStream(std::ostream &stream, ListWidget *outputList, const QColor &color)
    : m_output(outputList), m_color(color)
{
    stream.rdbuf(this);
}

void OutputStream::addLine(const QString &line)
{
    QMetaObject::invokeMethod(m_output, "addLine", Qt::QueuedConnection, Q_ARG(QString, line), Q_ARG(QColor, m_color));
}

void OutputStream::scrollToBottom()
{
    QMetaObject::invokeMethod(m_output, "scrollToBottom", Qt::QueuedConnection);
}

OutputStream::int_type OutputStream::overflow(OutputStream::int_type ch)
{    
    if(ch == '\n')
    {
        addLine(m_buffer);
        scrollToBottom();

        m_buffer.clear();
    }
    return ch;
}

std::streamsize OutputStream::xsputn(char const *s, std::streamsize count)
{
    QString str = QString(s).left(count);

    if(str.contains("\n"))
    {
        QStringList strList = str.split("\n");

        addLine(m_buffer + strList[0]);

        for(int i = 1; i < strList.size() - 1; ++i)
           addLine(strList[i]);

        scrollToBottom();

        m_buffer = strList.last();
    }
    else
        m_buffer += str;

    return count;
}
