#include "myfilesystemmodel.h"
#include <QFileIconProvider>

MyFileSystemModel::MyFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{

}

bool MyFileSystemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::DecorationRole && value.canConvert<QIcon>()) {
        emit dataChanged(index, index, {role});
        return true;
    }
    return QFileSystemModel::setData(index, value, role);
}

QVariant MyFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DecorationRole) {
        return this->iconProvider()->icon(this->fileInfo(index));
    }
    return QFileSystemModel::data(index, role);
}
