#include "thumbnailtask.h"

#include <QDir>
#include <QProcess>
#include <QMovie>
#include <QFileInfo>

ThumbnailTask::ThumbnailTask(const QString &filePath, QSize size, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
    , m_size(size)
{
}

// Task for generating a thumbnail for a file
void ThumbnailTask::run()
{
    QFileInfo fileInfo(m_filePath);
    if(!fileInfo.isFile())
        return;

    // If the file is an image, load the thumbnail with QPixmap
    if (fileInfo.suffix().toLower() == "jpg" || fileInfo.suffix().toLower() == "png") {
        QPixmap pixmap(m_filePath);
        if (!pixmap.isNull()) {
            m_pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }
    }
    // 如果是动画文件，使用 QMovie 获取第一帧并生成缩略图
    else if (fileInfo.suffix().toLower() == "gif" || fileInfo.suffix().toLower() == "apng") {
        QMovie movie(m_filePath);
        movie.setCacheMode(QMovie::CacheNone);
        movie.jumpToFrame(0);
        QPixmap pixmap = movie.currentPixmap();
        if (!pixmap.isNull()) {
            m_pixmap = pixmap.scaled(m_size, Qt::KeepAspectRatio,Qt::SmoothTransformation);
        }
    }
    // If the file is a video, generate the thumbnail with ffmpeg
    else if (fileInfo.suffix().toLower() == "mp4" || fileInfo.suffix().toLower() == "avi") {
        //确保缩略图的文件夹存在
        QString dirName = fileInfo.path() + "/thumb";
        qDebug() << fileInfo.path() << "fileInfo.path()";
        QDir dir(dirName);
        if(!dir.exists()){
            if(dir.mkpath(dirName)){
                qDebug() << "Folder created successfully.";
            }else{
                qDebug() << "Failed to create folder.";
            }
        }

        QString thumbnail = dirName + "/" + fileInfo.baseName() + ".jpg";

        if(!QFileInfo::exists(thumbnail)){
            QProcess process;
            process.setWorkingDirectory(fileInfo.path());
            QString command = QString("ffmpeg -i \"%1.mp4\" -ss 00:00:00 -vframes 1 -s 640x360 \"thumb/%1.jpg\"").arg(fileInfo.baseName());
            process.start(command);
            if(!process.waitForFinished(-1)){
                qWarning() << "Failed to generate thumb image with ffmpeg";
                return;
            }
        }
        m_pixmap.load(thumbnail);
    }

    QImage image = m_pixmap.toImage();
    emit thumbnailGenerated(m_filePath, image);
//    emit finished(m_pixmap);
}
