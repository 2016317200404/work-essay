#include "customiconprovider.h"
#include <QDebug>
#include <QThread>
#include <QPainter>

CustomIconProvider::CustomIconProvider()
    : QObject(), QFileIconProvider()
{
    m_Cthumbnails.setMaxCost(1000);
}

QIcon CustomIconProvider::icon(const QFileInfo &info) const
{
    if (info.isFile()) {
        QString ext = info.suffix().toLower();
        if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif" ||
            ext == "mp4" || ext == "avi" || ext == "mov" || ext == "wmv") {
            if (m_Cthumbnails.contains(info.absoluteFilePath())) {
                m_Lock.lock();
                QIcon icon((*m_Cthumbnails[info.absoluteFilePath()]));
                m_Lock.unlock();
                return icon;
            } else {
                emit requireThumbnail(info.absoluteFilePath());

                QPixmap pixmap(128, 128);
                pixmap.fill(Qt::transparent);

                return QIcon(pixmap);
            }
        }
    }

    // 对于其他文件类型，使用默认图标
    return QFileIconProvider::icon(info);
}

void CustomIconProvider::onThumbnailGenerated(const QString &videoPath, const QImage &thumbnail)
{
    QFileInfo fileInfo(videoPath);
    QString ext = fileInfo.suffix().toLower();
    if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif")
        GenerateImageThumbnails(videoPath, thumbnail);
    else
        GenerateVideoThumbnails(videoPath, thumbnail);
    emit thumbnailAvailable(videoPath);
}

void CustomIconProvider::GenerateImageThumbnails(const QString &videoPath, const QImage &thumbnail)
{
    QPixmap pixmap = QPixmap::fromImage(thumbnail);
    pixmap = pixmap.scaled(QSize(canvasSize, canvasSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap canvas(canvasSize, canvasSize);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    int x = (canvasSize - pixmap.width()) / 2;
    int y = (canvasSize - pixmap.height()) / 2;
    painter.drawPixmap(x, y, pixmap);

    painter.end();

    m_Lock.lock();
    m_Cthumbnails.insert(videoPath, new QPixmap(canvas));
    m_Lock.unlock();
}

void CustomIconProvider::GenerateVideoThumbnails(const QString &videoPath, const QImage &thumbnail)
{
    QPixmap pixmap = QPixmap::fromImage(thumbnail);
    pixmap = pixmap.scaled(QSize(canvasSize, canvasSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap player(":/new/prefix1/icon/player.png");

    QPixmap canvas(canvasSize, canvasSize);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    int x = (canvasSize - pixmap.width()) / 2;
    int y = (canvasSize - pixmap.height()) / 2;
    painter.drawPixmap(x, y, pixmap);

    x = (canvasSize - player.width()) / 2;
    y = (canvasSize - player.height()) / 2;
    painter.drawPixmap(x, y, player);

    painter.end();

    m_Lock.lock();
    m_Cthumbnails.insert(videoPath, new QPixmap(canvas));
    m_Lock.unlock();
}
