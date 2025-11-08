
#include "customcombobox.h"

CustomComboBox::CustomComboBox(QWidget *parent) : QComboBox(parent) {}

void CustomComboBox::paintEvent(QPaintEvent *e) {
    QStyleOptionComboBox opt;
    initStyleOption(&opt);

    opt.subControls =/* QStyle::SC_ComboBoxFrame | */QStyle::SC_ComboBoxEditField| QStyle::SC_ComboBoxArrow;;

    QPainter p(this);
    style()->drawComplexControl(QStyle::CC_ComboBox, &opt, &p, this);

    QPixmap pixmap(":/image/folder.jpg");
    if (!pixmap.isNull()) {
        // 调整图片大小以适应ComboBox高度（保留宽高比）
        int imgHeight = opt.rect.height() - 4; // 减去边距
        QPixmap scaledPixmap = pixmap.scaledToHeight(imgHeight, Qt::SmoothTransformation);

        // 绘制图片（左侧）
        int x = opt.rect.left() + 20; // 左边距5像素
        int y = opt.rect.top() + (opt.rect.height() - scaledPixmap.height()) / 2;
        p.drawPixmap(x, y, scaledPixmap);
        // 调整文本绘制区域，避免与图片重叠
        opt.rect.adjust(scaledPixmap.width() + 10, 0, 0, 0);
    }

    // 自定义文本绘制
    p.setPen(palette().color(QPalette::Text));
    p.drawText(opt.rect.adjusted(5, 0, -5, 0),
               Qt::AlignRight | Qt::AlignVCenter,
               m_CbxText);
}

void CustomComboBox::setCustomCbxTitle(QString str)
{
    m_CbxText = str;
}
