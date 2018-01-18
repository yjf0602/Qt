#include "pngresaver.h"
#include <QDir>
#include <QFileInfoList>
#include <QImage>

PngResaver::PngResaver(QObject *parent) : QObject(parent)
{

}

QStringList PngResaver::get_png_file_list(QString path)
{
    QDir dir(path);

    QStringList png_list;

    QFileInfoList file_list = dir.entryInfoList(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
    QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);

    for(int i=0; i!=file_list.size(); i++)
    {
        QString absolute_file_path = file_list.at(i).absoluteFilePath();
        if(absolute_file_path.endsWith("png", Qt::CaseInsensitive))
        {
            png_list.append(absolute_file_path);
        }
    }

    for(int i=0; i!=folder_list.size(); i++)
    {
        QString sub_path = folder_list.at(i).absoluteFilePath();
        png_list.append(get_png_file_list(sub_path));
    }

    return png_list;
}

void PngResaver::start_process(QString path)
{
    stop_flag = false;

    emit process_signal(0);

    QStringList png_url_list = get_png_file_list(path);

    int num = png_url_list.size();

    for(int i=0; i<num; i++)
    {
        QImage img(png_url_list.at(i));

        img.save(png_url_list.at(i));

        emit process_signal((i+1)*100/num);

        if(stop_flag)
            break;
    }

    emit process_signal(100);
}

void PngResaver::stop_process()
{
    stop_flag = true;
}
