#ifndef MYFILESYSTEMMODEL_H
#define MYFILESYSTEMMODEL_H

#include <QFileSystemModel>

class MyFileSystemModel : public QFileSystemModel
{
public:
    explicit MyFileSystemModel(QObject *parent = nullptr);

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
};

#endif // MYFILESYSTEMMODEL_H
