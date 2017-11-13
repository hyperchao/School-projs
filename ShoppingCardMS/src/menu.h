#pragma once

#include <string>
#include <vector>
#include "customer.h"
#include "shop.h"

class Menu {

public:

    Menu();
    ~Menu();
    
    void Welcome(); // 欢迎界面
    void sellerLogin(); // 商家登录界面
    void customerLogin(); // 用户登录界面
    void sellerMenu(); // 商家功能菜单
    void customerMenu(); // 用户功能菜单
    void sellerChoices(int choice); // 执行商家选择的功能
    void customerChoices(int choice); // 执行用户选择的功能
    void Encryption(const std::string& passwd); // 加密
    std::string Decryption(const char* passwdFile); // 解密
    
private:

    // 标题显示
    void sellerTitle();
    void customerTitle();
    void title1(const std::string &title);
    
    // 菜单显示
    
    // 功能菜单， titile为标题, item是所有要显示的选项的vector
    void functionMenu(const std::string &title, std::vector<std::string> &item);
    
    // 输入框菜单， hints保存输入提示, echo控制每一个输入是否回显
    std::vector<std::string> MenuWithInput(std::vector<std::string> &hints, std::vector<bool> &echo);
    
    void showCustomerInfo(Customer *c); // 信息显示界面

    void splitline(); // 分隔符
    
    Customer *customer = nullptr;
    Shop *shop = nullptr;
};
