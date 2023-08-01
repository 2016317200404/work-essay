#include "tinyxml2.h"
#include <iostream>
#include <QCoreApplication>
#include <QtXlsx>

using namespace tinyxml2;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QXlsx::Document xlsx("Test.xlsx");
    QXlsx::Worksheet *worksheet = xlsx.currentWorksheet();

    int column = 1; // 列索引，从1开始
    int row = 1;    // 起始行索引，从1开始
    int maxCol = 1;

    while (!worksheet->read(row, column).isNull()) {
        column++; // 移动到下一行
    }
    maxCol = column;

    for (int j = 2; j <= maxCol; ++j) {
        int i = 2;
        XMLDocument doc;
        tinyxml2::XMLElement* root = doc.NewElement("TS");
        doc.InsertEndChild(root);

        tinyxml2::XMLElement* context = doc.NewElement("context");
        root->InsertEndChild(context);

        tinyxml2::XMLElement* name = doc.NewElement("name");
        name->SetText("default");
        context->InsertEndChild(name);

        while (! worksheet->read(i, j).isNull()) {
            QVariant translateString = worksheet->read(i, j);
            QVariant souceString = worksheet->read(i, 1);

            tinyxml2::XMLElement* message = doc.NewElement("message");
            context->InsertEndChild(message);

            tinyxml2::XMLElement* source = doc.NewElement("source");
            source->SetText(souceString.toString().toUtf8().constData());
            message->InsertEndChild(source);

            tinyxml2::XMLElement* translation = doc.NewElement("translation");
            translation->SetText(translateString.toString().toUtf8().constData());
            message->InsertEndChild(translation);
            //qDebug() << "Cell[" << i << "][" << j << "]: " << translateString.toString() << "Cell[" << i << "][" << 1 << "]: " << souceString.toString();
            i++;
        }
        QString str = worksheet->read(1, j).toString();
        QString fileName = str.left(str.indexOf("(")) + ".ts";
        doc.SaveFile(fileName.toStdString().c_str());
    }

    return a.exec();
}
