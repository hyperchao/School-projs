#pragma once

#include <ctime>
#include <cstdint>

class Customer {

public:

	Customer(const char *name, uint64_t phone, int id, const char *passwd);//构造函数声明

    
	const char* getName();//获取姓名
	uint64_t getPhoneNum(); //获取号码
	void setPhoneNum(uint64_t phone_num);//设置号码
    double getBalance();//获取余额
    void setBalance(double amount);//改变余额
    int getPoint();//获取积分
    void addPoint(int val);//增加积分
	int getID();//获取编号
	time_t getRegisterTime();//获取办卡时间
    bool isLocked();//判断是否锁卡
    bool verifyPasswd(const char *passwd);//改密码
	bool setPasswd(const char *old_passwd, const char *new_passwd);//设置密码
	void setLocked(bool locked);//设置是否锁卡
	bool pointConvert();//积分兑换
	void deposit(double amount);//充值

private:

	char name[13];//姓名
	uint64_t phone_num;//号码
	int id;//编号
	char passwd[13];//密码
	time_t register_time;//办卡时间

	double balance = 0;//余额初始化为0
	int point = 0;//积分初始化为0
	bool locked = false;//锁卡标记为false
};