#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include <QMainWindow>
#include<QPaintEvent>
namespace Ui {
class GameDialog;
}

class GameDialog : public QMainWindow
{
    Q_OBJECT
signals:
    void SIG_LeaveRoom(int roomid);
public:
    explicit GameDialog(QWidget *parent = 0);
    ~GameDialog();
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    int roomid;
private:
    Ui::GameDialog *ui;
};

#endif // GAMEDIALOG_H
