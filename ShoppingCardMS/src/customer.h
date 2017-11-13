#pragma once

#include <ctime>
#include <cstdint>

class Customer {

public:

	Customer(const char *name, uint64_t phone, int id, const char *passwd);//���캯������

    
	const char* getName();//��ȡ����
	uint64_t getPhoneNum(); //��ȡ����
	void setPhoneNum(uint64_t phone_num);//���ú���
    double getBalance();//��ȡ���
    void setBalance(double amount);//�ı����
    int getPoint();//��ȡ����
    void addPoint(int val);//���ӻ���
	int getID();//��ȡ���
	time_t getRegisterTime();//��ȡ�쿨ʱ��
    bool isLocked();//�ж��Ƿ�����
    bool verifyPasswd(const char *passwd);//������
	bool setPasswd(const char *old_passwd, const char *new_passwd);//��������
	void setLocked(bool locked);//�����Ƿ�����
	bool pointConvert();//���ֶһ�
	void deposit(double amount);//��ֵ

private:

	char name[13];//����
	uint64_t phone_num;//����
	int id;//���
	char passwd[13];//����
	time_t register_time;//�쿨ʱ��

	double balance = 0;//����ʼ��Ϊ0
	int point = 0;//���ֳ�ʼ��Ϊ0
	bool locked = false;//�������Ϊfalse
};