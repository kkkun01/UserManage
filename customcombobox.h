#ifndef CUSTOMCOMBOBOX_H
#define CUSTOMCOMBOBOX_H

#include <QComboBox>
#include <QPainter>
#include <QStyleOptionComboBox>

class CustomComboBox : public QComboBox {
    Q_OBJECT
public:
    explicit CustomComboBox(QWidget *parent = nullptr);
    void setCustomCbxTitle(QString str);

protected:
    void paintEvent(QPaintEvent *e) override;

private:


    QString m_CbxText = "";
};

#endif // CUSTOMCOMBOBOX_H
