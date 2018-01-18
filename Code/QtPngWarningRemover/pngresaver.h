#ifndef PNGRESAVER_H
#define PNGRESAVER_H

#include <QObject>


class PngResaver : public QObject
{
    Q_OBJECT
public:
    explicit PngResaver(QObject *parent = nullptr);
    QStringList get_png_file_list(QString path);

signals:
    void process_signal(int percentage);

public slots:
    void start_process(QString path);
    void stop_process(void);

private:
    bool stop_flag;

};

#endif // PNGRESAVER_H
