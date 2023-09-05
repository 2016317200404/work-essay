#include "customiconprovider.h"
#include <QThread>
#include <QPainter>

CustomIconProvider::CustomIconProvider()
    : QObject(), QFileIconProvider()
{
}

QIcon CustomIconProvider::icon(const QFileInfo &info) const
{
    if (info.isFile()) {
        QString ext = info.suffix().toLower();
        if (ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "gif") {
            // 使用自定义图像文件图标
            int canvasSize = 128;
            QPixmap pixmap = QPixmap(info.absoluteFilePath());
            pixmap = pixmap.scaled(QSize(canvasSize, canvasSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);

            QPixmap canvas(128, 128);
            canvas.fill(Qt::transparent);

            QPainter painter(&canvas);
            int x = (canvasSize - pixmap.width()) / 2;
            int y = (canvasSize - pixmap.height()) / 2;
            painter.drawPixmap(x, y, pixmap);

            painter.end();

            return QIcon(canvas);
        } else if (ext == "mp4" || ext == "avi" || ext == "mov" || ext == "wmv") {
            if (thumbnails.contains(info.absoluteFilePath())) {
                int canvasSize = 128;
                QPixmap pixmap = QPixmap::fromImage(thumbnails[info.absoluteFilePath()]);
                pixmap = pixmap.scaled(QSize(canvasSize, canvasSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);

                QPixmap player(":/new/prefix1/icon/player.png");

                QPixmap canvas(128, 128);
                canvas.fill(Qt::transparent);

                QPainter painter(&canvas);
                int x = (canvasSize - pixmap.width()) / 2;
                int y = (canvasSize - pixmap.height()) / 2;
                painter.drawPixmap(x, y, pixmap);

                x = (canvasSize - player.width()) / 2;
                y = (canvasSize - player.height()) / 2;
                painter.drawPixmap(x, y, player);

                painter.end();

                return QIcon(canvas);
//                return QIcon(QPixmap::fromImage(thumbnails[info.absoluteFilePath()]));
            } else {
                emit requireThumbnail(info.absoluteFilePath());
            }
        }
    }

    // 对于其他文件类型，使用默认图标
    return QFileIconProvider::icon(info);
}

void CustomIconProvider::onThumbnailGenerated(const QString &videoPath, const QImage &thumbnail)
{
    thumbnails[videoPath] = thumbnail;
    emit thumbnailAvailable(videoPath);
}
