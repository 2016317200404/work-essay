#ifndef THUMBNAILTASK_H
#define THUMBNAILTASK_H

#include <QSize>
#include <QPixmap>
#include <QObject>
#include <QRunnable>

class ThumbnailTask : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit ThumbnailTask(const QString& filePath, QSize size = QSize(128, 128), QObject* parent = nullptr);

    void run() override;

signals:
    void thumbnailGenerated(const QString &videoPath, const QImage &thumbnail);

    void finished(QPixmap);

public:
    QString m_filePath;
    QSize m_size;
    QPixmap m_pixmap;
};

#endif // THUMBNAILTASK_H
