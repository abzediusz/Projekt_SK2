#pragma once
#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;
class NetworkManager;

class LoginWindow : public QWidget {
    Q_OBJECT
public:
    LoginWindow(QWidget *parent = nullptr);
    
private slots:
    void onConnectClicked();
    
private:
    QLineEdit *hostInput;
    QLineEdit *portInput;
    QLineEdit *nickInput;
    QPushButton *connectButton;
    QLabel *statusLabel;
    NetworkManager *network;
};
