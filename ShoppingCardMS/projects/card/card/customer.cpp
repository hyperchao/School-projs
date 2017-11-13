#include "customer.h"
#include "shop.h"
#include <cstring>

Customer::Customer(const char *name, uint64_t phone, int id, const char *passwd) {//为数据成员赋值
    
    this->phone_num = phone;
    this->id = id;
    strcpy(this->name, name);
    strcpy(this->passwd, passwd);
    register_time = time(0);
}

const char* Customer::getName() {
    return name;//返回name
}

uint64_t Customer::getPhoneNum() {
    return phone_num;//返回号码
}

int Customer::getID() {
    return id;//返回编号
}

double Customer::getBalance() {
    return balance;//返回余额
}

void Customer::setBalance(double amount) {
    balance = amount;//通过参数amount修改余额
}

time_t Customer::getRegisterTime() {
    return register_time;//返回办卡时间
}

int Customer::getPoint() {
	return point;//返回积分
}

void Customer::addPoint(int val) {
    point += val;//增加积分
}

bool Customer::isLocked() {
    return locked;//返回locked
}

bool Customer::verifyPasswd(const char *passwd) {
    return strcmp(this->passwd, passwd) == 0;//返回判断密码是否相等
}

bool Customer::setPasswd(const char *old_passwd, const char *new_passwd) {
    
    if (verifyPasswd(old_passwd)) {//判断当前密码是否等于原密码
        strcpy(passwd, new_passwd); //相等则改密码成功

        return true;//返回true
    }
    else {
        return false;//否则修改不成功，返回false
    }
}

void Customer::setPhoneNum(uint64_t phone_num) {
    this->phone_num = phone_num;//通过参数phone_num修改号码
}

void Customer::setLocked(bool locked) {
    this->locked = locked;//通过参数locked修改锁卡标记
}

bool Customer::pointConvert() {
    
    int limit = Shop::getMinConvertPoint();//调用shop类中的获取最小积分点函数为limit赋值
    
    if (point > limit) {//如果积分大于最小积分值点
        balance += point/limit*limit*Shop::getRatio();//兑换积分为现金到余额中
        point %= limit;//新积分等于原积分除最小积分点取余
        return true;//返回true
    }
    else {
        return false;//如果积分小于最小值积分值点，不兑换积分
    }
}

void Customer::deposit(double amount) {
    balance += amount;//余额充值
}