#pragma once

#include "customer.h"
#include <vector>

struct Record {
    int min_point = 100; // ��Ͷһ�����
    double ratio = 0.1;   // �һ�����
};

class Shop {

public:

    Shop();//���캯��
    ~Shop();//��������
    
    void addAccount(const char *name, uint64_t phone, const char *passwd);//����˻�
    bool delAccount(int account_id); //ɾ���˻�
    Customer *getAccount(int index);//��ȡ�˻���Ϣ

    bool lookupByName(const char *name, int &index); //ͨ�����ֱȶ��û�
	std::vector<int> lookupByName(const char *name);
    bool lookupByPhone(uint64_t phone, int &index);//ͨ������ȶ��û�
    bool lookupById(int id, int &index);//ͨ����űȶ��û�

    bool deposit(int account_id, double amount);//���
    bool consume(int account_id, double amount); //����
    
    void setMinPoint(int val);//���û��ֶһ�����
    void setRatio(double val);//���û��ֶһ�����
    static int getMinConvertPoint();//��ȡ��Ͷһ�����
    static double getRatio(); //��ȡ���ֱ���

private:

    std::vector<Customer> group; //����ȫ������group
    static Record record;//���徲̬�ṹ�����record
};
