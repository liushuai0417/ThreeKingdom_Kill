#include "joinroomdialog.h"
#include "ui_joinroomdialog.h"
#include"mypushbutton.h"
#include<QPainter>
JoinRoomDialog::JoinRoomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinRoomDialog)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/res/icon/icon.png"));
    this->setWindowTitle("三国Kill");
    ui->widget->setStyleSheet("QWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
    ui->le_content->setStyleSheet("QLineEdit{background-color: rgba(96, 96, 96, 0.4)}"
                                         "QLineEdit{border-width:2;border-style:outset;border : 1px solid gray;}");

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::white);
    QBrush myBrush;
    QPalette palette;
    myBrush = QBrush(Qt::white,Qt::DiagCrossPattern);
    palette.setBrush( QPalette::Text,  myBrush);
    ui->le_content->setPalette(palette);
    ui->le_content->setFont(QFont("华文行楷",20,QFont::Bold));

    m_roomLayout = new QVBoxLayout;
    m_roomLayout->setContentsMargins(0,0,0,0);//设置外边距
    m_roomLayout->setSpacing(2);
    ui->wdg_roomitem->setLayout(m_roomLayout);

    //name查找房间按钮
    MyPushButton *searchByName = new MyPushButton(":/res/icon/click_searchbyname.png",":/res/icon/click_searchbyname_1.png");
    searchByName->setParent(this);
    searchByName->move(60,this->height()*0.8-80);
    //connect(searchByName)
    //id找房间按钮
    MyPushButton *searchById = new MyPushButton(":/res/icon/click_searchbyid.png",":/res/icon/click_searchbyid_1.png");
    searchById->setParent(this);
    searchById->move(this->width()-searchById->width()-60,this->height()*0.8-80);
    connect(searchByName,&MyPushButton::clicked,this,&JoinRoomDialog::SLOT_JoinRoomByName);
    connect(searchById,&MyPushButton::clicked,this,&JoinRoomDialog::SLOT_JoinRoomById);
}

JoinRoomDialog::~JoinRoomDialog()
{
    delete ui;
}

void JoinRoomDialog::paintEvent(QPaintEvent *event){
    //设置背景
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/res/BJ/task_level_map.png");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);

    ui->scrollArea->setStyleSheet("background-color:transparent;");
    ui->scrollArea->viewport()->setStyleSheet("background:transparent;");
    ui->wdg_roomitem->setStyleSheet("QWidget#wdg_frienditem{ border:1px solid white;background:  transparent; }");

    ui->widget->setStyleSheet("QWidget:pane {border-top:0px solid #e8f3f9;background:  transparent; }"
                                "QTabBar::tab::selected{background: transparent;color:white;border:2px groove gray;border-radius:10px;padding:2px 4px;border-style:outset;font:12pt '华文行楷'}"
                                "QTabBar::tab{background: white;color:black;border-style:outset;border:2px groove gray;border-radius:10px;padding:2px 4px;font:12pt '华文行楷'}");
}

//name查找房间槽函数
void JoinRoomDialog::SLOT_JoinRoomByName(){
    QString str = ui->le_content->text();
    Q_EMIT SIG_JoinRoomByNameCommit(str);
}

//id查找房间槽函数
void JoinRoomDialog::SLOT_JoinRoomById(){
    QString str = ui->le_content->text();
    Q_EMIT SIG_JoinRoomByIdCommit(str);
}

void JoinRoomDialog::Slot_AddRoomItem(QWidget *item){
    m_roomLayout->addWidget(item);
}

void JoinRoomDialog::Slot_RemoveRoomItem(QWidget *item){
    m_roomLayout->removeWidget(item);
}

void JoinRoomDialog::closeEvent(QCloseEvent *event){
    auto ite = vec.begin();
    while(ite!=vec.end()){
        this->Slot_RemoveRoomItem(*ite);
        delete *ite;
        *ite = NULL;
        ite++;
    }
}
