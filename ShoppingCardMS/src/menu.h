#pragma once

#include <string>
#include <vector>
#include "customer.h"
#include "shop.h"

class Menu {

public:

    Menu();
    ~Menu();
    
    void Welcome(); // ��ӭ����
    void sellerLogin(); // �̼ҵ�¼����
    void customerLogin(); // �û���¼����
    void sellerMenu(); // �̼ҹ��ܲ˵�
    void customerMenu(); // �û����ܲ˵�
    void sellerChoices(int choice); // ִ���̼�ѡ��Ĺ���
    void customerChoices(int choice); // ִ���û�ѡ��Ĺ���
    void Encryption(const std::string& passwd); // ����
    std::string Decryption(const char* passwdFile); // ����
    
private:

    // ������ʾ
    void sellerTitle();
    void customerTitle();
    void title1(const std::string &title);
    
    // �˵���ʾ
    
    // ���ܲ˵��� titileΪ����, item������Ҫ��ʾ��ѡ���vector
    void functionMenu(const std::string &title, std::vector<std::string> &item);
    
    // �����˵��� hints����������ʾ, echo����ÿһ�������Ƿ����
    std::vector<std::string> MenuWithInput(std::vector<std::string> &hints, std::vector<bool> &echo);
    
    void showCustomerInfo(Customer *c); // ��Ϣ��ʾ����

    void splitline(); // �ָ���
    
    Customer *customer = nullptr;
    Shop *shop = nullptr;
};
