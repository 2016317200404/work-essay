#ifndef CUSTOMICONPROVIDER_H
#define CUSTOMICONPROVIDER_H

#include <QMap>
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
    QMap<QString, QImage> thumbnails;
};

#endif // CUSTOMICONPROVIDER_H