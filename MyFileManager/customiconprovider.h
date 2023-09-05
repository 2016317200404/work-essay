#ifndef CUSTOMICONPROVIDER_H
#define CUSTOMICONPROVIDER_H

#include <QMap>
#include <QMutex>
#include <QCache>
#include <QObject>
#include <QFileIconProvider>

class CustomIconProvider : public QObject, public QFileIconProvider
{
    Q_OBJECT
public:
    CustomIconProvider();

    QIcon icon(const QFileInfo& info) const override;

public slots:
    void onThumbnailGenerated(const QString &videoPath, const QImage &thumbnail);

signals:
    void thumbnailAvailable(const QString &videoPath);

    void requireThumbnail(const QString &videoPath) const;

private:
    void GenerateImageThumbnails(const QString &videoPath, const QImage &thumbnail);

    void GenerateVideoThumbnails(const QString &videoPath, const QImage &thumbnail);

private:
    int canvasSize = 128;
    mutable QMutex m_Lock;
    QCache<QString, QPixmap> m_Cthumbnails;
};

#endif // CUSTOMICONPROVIDER_H
