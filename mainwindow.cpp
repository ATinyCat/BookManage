#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "adddialog.h"
#include "borrowinfodialog.h"
#include "searchdialog.h"
#include "drawdialog.h"
#include <QStandardItemModel>
#include <QString>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel;
    model->setColumnCount(6);
    model->setHeaderData(0,Qt::Horizontal,QString::fromUtf8("书号"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromUtf8("书名"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromUtf8("出版社"));
    model->setHeaderData(3,Qt::Horizontal,QString::fromUtf8("作者"));
    model->setHeaderData(4,Qt::Horizontal,QString::fromUtf8("库存"));
    model->setHeaderData(5,Qt::Horizontal,QString::fromUtf8("价格"));
    ui->bookView->setModel(model);
    ui->bookView->setColumnWidth(0,40);
    ui->bookView->setColumnWidth(1,120);
    ui->bookView->setColumnWidth(2,150);
    ui->bookView->setColumnWidth(4,50);
    ui->bookView->setColumnWidth(5,50);
    inputBook(6,"示例六","超高等教育出版社","韩梅梅",6,998);
    inputBook(1,"示例一","高等教育出版社","小明",2,55);
    inputBook(2,"示例二","中等教育出版社","小红",5,35);
    inputBook(3,"示例三","低等教育出版社","小刚",1,37.2);
    inputBook(4,"示例四","低低等教育出版公司","小华",2,99.8);
    inputBook(5,"示例五","超低等教育出版社","李雷",5,9.99);
    Book b1;
    b1.InputBook(25,"C++程序设计","清华大学出版社","谭浩强",10,0.25);
    b1.SetBorrow("20170415","2017.12.1");
    b1.SetBorrow("20161564","2018.2.1");
    BT.insertRecord(25,b1);
    ui->bookView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    displayTable();
    QModelIndex nofocus = model->index(-1,0,nofocus);
    ui->bookView->setCurrentIndex(nofocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_addButton_clicked()
{
    addDialog *addD = new addDialog(this);
    if (addD->exec()==QDialog::Accepted)
    {

        inputBook(addD->number,
                  addD->name,
                  addD->publisher,
                  addD->author,
                  addD->stock,
                  addD->price);
        displayTable();
    }

}

void MainWindow::on_exitButton_clicked()
{
    QMessageBox info(QMessageBox::Information,"Author","tiger");
    info.exec();
    QApplication::exit();
}

void MainWindow::on_delButton_clicked()
{
    int currentRow = ui->bookView->currentIndex().row();
    if (currentRow<0)
    {
        QMessageBox warn(QMessageBox::Warning,"注意","请选中要删除的图书");
        warn.exec();
        return;
    }

    QModelIndex index,index2;
    index = model->index(currentRow,0,index);
    QString num = index.data().toString();
    index2 = model->index(currentRow,1,index2);
    QString name = index2.data().toString();

    QString text = "是否要删除 "+num+" 号图书？";
    QMessageBox msgbox(QMessageBox::Question,"注意",text);
    msgbox.setStandardButtons(QMessageBox::Ok|QMessageBox::Cancel);
    msgbox.setButtonText(QMessageBox::Ok,"删 除");
    msgbox.setButtonText(QMessageBox::Cancel,"取 消");
    if (msgbox.exec()==QMessageBox::Ok)
    {
        int number = num.toInt();
        BT.remove(number);
        displayTable();
    }
}
void MainWindow::on_borrowInfoButton_clicked()
{
    int currentrow = ui->bookView->currentIndex().row();
    if (currentrow<0)
    {
        QMessageBox warn(QMessageBox::Warning,"注意","请选中要查看的图书");
        warn.exec();
        return;
    }

    QModelIndex index;
    index = model->index(currentrow,0,index);
    int num = index.data().toInt();

    if (!BT.search(num)) return;
    int x = BT.currrent->search(num);
    Book *b =& BT.currrent->record[x];
    borrowInfoDialog *bD = new borrowInfoDialog;
    bD->getBook(b);
    bD->displayTable();
    bD->exec();
}

bool MainWindow::inputBook(int number, QString name, QString publisher, QString author, int stock, float price)
{
    Book b;
    b.InputBook(number,name,publisher,author,stock,price);
    return(BT.insertRecord(number,b));
}

void MainWindow::displayTable()
{
    ct = 0;
    model->removeRows(0,model->rowCount());                 //清除列表已有数据
    displayTable(BT.root);
}

void MainWindow::displayTable(BTNodeP(Book) p)
{
    if (p == NULL) return;
    for (int i=0;i<p->record.size();i++)
    {
        model->setItem(ct,0,new QStandardItem(QString::number(p->record[i].number,10)));
        model->setItem(ct,1,new QStandardItem(p->record[i].name));
        model->setItem(ct,2,new QStandardItem(p->record[i].publisher));
        model->setItem(ct,3,new QStandardItem(p->record[i].author));
        model->setItem(ct,4,new QStandardItem(QString::number(p->record[i].stock,10)));
        model->setItem(ct,5,new QStandardItem(QString::number(p->record[i].price,10,2)));
        ct++;
    }
    for (int i=0;i<p->child.size();i++)
    {
        displayTable(p->child[i]);
    }

}

void MainWindow::on_searchButton_clicked()
{
    searchDialog * src = new searchDialog;
    BTree<Book> *p = &BT;
    src->gerTree(p);
    if(src->exec()==QDialog::Accepted)
    {
        displayTable();
    }
}

void MainWindow::on_BTreeButton_clicked()//失败的尝试。。。
{
    drawDialog *dD = new drawDialog;
    dD->resize(800,450);
    dD->setWindowTitle("");
    BTree<Book> *p = &BT;
    if (BT.root==NULL) return;
    dD->getTree(p);
    dD->exec();
}
