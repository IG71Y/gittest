#ifndef SELECTWIN_H
#define SELECTWIN_H

#include <QWidget>

#include <QSqlTableModel>

namespace Ui {
class SelectWin;
}

/**
 * @brief SelectWin类 实现服务器查询功能的界面。
 */
class SelectWin : public QWidget
{
    Q_OBJECT

public:
    explicit SelectWin(QWidget *parent = nullptr);
    ~SelectWin();

private slots:

    // 选择按钮点击槽函数。
    void on_selectBt_clicked();

private:
    Ui::SelectWin *ui;

    QSqlTableModel *model;

};

#endif // SELECTWIN_H
