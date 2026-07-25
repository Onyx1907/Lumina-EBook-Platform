#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(Server *server, QWidget *parent)
    : QMainWindow(parent), m_server(server) // مقداردهی m_server با سرور اصلی
{
    setupUI();
    setWindowTitle("Server Management Dashboard");
    resize(950, 650);
}
MainWindow::~MainWindow()
{
}
