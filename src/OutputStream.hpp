#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <QListWidget>

#include <iostream>

class ListWidget : public QListWidget
{
    Q_OBJECT

    public slots:
        void addLine(QString line, QColor color);

    private:
        const int maxCharCount = 66;
        const int maxItemCount = 300;
};

class OutputStream : public std::basic_streambuf<char>
{
    public:
        OutputStream(std::ostream &stream, ListWidget *outputList, const QColor &color);

        void addLine(const QString &line);
        void scrollToBottom();


    protected:
        virtual int_type overflow(int_type ch) override;
        virtual std::streamsize xsputn(char const *s, std::streamsize count) override;

    private:
        QString m_buffer;
        ListWidget *m_output;
        QColor m_color;
};


#endif // OUTPUTSTREAM_H
