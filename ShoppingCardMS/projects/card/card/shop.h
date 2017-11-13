#pragma once

#include "customer.h"
#include <vector>

struct Record {
    int min_point = 100; // 最低兑换点数
    double ratio = 0.1;   // 兑换比例
};

class Shop {

public:

    Shop();//构造函数
    ~Shop();//析构函数
    
    void addAccount(const char *name, uint64_t phone, const char *passwd);//添加账户
    bool delAccount(int account_id); //删除账户
    Customer *getAccount(int index);//获取账户信息

    bool lookupByName(const char *name, int &index); //通过名字比对用户
	std::vector<int> lookupByName(const char *name);
    bool lookupByPhone(uint64_t phone, int &index);//通过号码比对用户
    bool lookupById(int id, int &index);//通过编号比对用户

    bool deposit(int account_id, double amount);//存款
    bool consume(int account_id, double amount); //消费
    
    void setMinPoint(int val);//设置积分兑换下限
    void setRatio(double val);//设置积分兑换比率
    static int getMinConvertPoint();//获取最低兑换点数
    static double getRatio(); //获取积分比率

private:

    std::vector<Customer> group; //定义全局向量group
    static Record record;//定义静态结构体变量record
};
