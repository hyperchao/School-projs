#include "customer.h"
#include "shop.h"
#include <cstring>

Customer::Customer(const char *name, uint64_t phone, int id, const char *passwd) {//Ϊ���ݳ�Ա��ֵ
    
    this->phone_num = phone;
    this->id = id;
    strcpy(this->name, name);
    strcpy(this->passwd, passwd);
    register_time = time(0);
}

const char* Customer::getName() {
    return name;//����name
}

uint64_t Customer::getPhoneNum() {
    return phone_num;//���غ���
}

int Customer::getID() {
    return id;//���ر��
}

double Customer::getBalance() {
    return balance;//�������
}

void Customer::setBalance(double amount) {
    balance = amount;//ͨ������amount�޸����
}

time_t Customer::getRegisterTime() {
    return register_time;//���ذ쿨ʱ��
}

int Customer::getPoint() {
	return point;//���ػ���
}

void Customer::addPoint(int val) {
    point += val;//���ӻ���
}

bool Customer::isLocked() {
    return locked;//����locked
}

bool Customer::verifyPasswd(const char *passwd) {
    return strcmp(this->passwd, passwd) == 0;//�����ж������Ƿ����
}

bool Customer::setPasswd(const char *old_passwd, const char *new_passwd) {
    
    if (verifyPasswd(old_passwd)) {//�жϵ�ǰ�����Ƿ����ԭ����
        strcpy(passwd, new_passwd); //����������ɹ�

        return true;//����true
    }
    else {
        return false;//�����޸Ĳ��ɹ�������false
    }
}

void Customer::setPhoneNum(uint64_t phone_num) {
    this->phone_num = phone_num;//ͨ������phone_num�޸ĺ���
}

void Customer::setLocked(bool locked) {
    this->locked = locked;//ͨ������locked�޸��������
}

bool Customer::pointConvert() {
    
    int limit = Shop::getMinConvertPoint();//����shop���еĻ�ȡ��С���ֵ㺯��Ϊlimit��ֵ
    
    if (point > limit) {//������ִ�����С����ֵ��
        balance += point/limit*limit*Shop::getRatio();//�һ�����Ϊ�ֽ������
        point %= limit;//�»��ֵ���ԭ���ֳ���С���ֵ�ȡ��
        return true;//����true
    }
    else {
        return false;//�������С����Сֵ����ֵ�㣬���һ�����
    }
}

void Customer::deposit(double amount) {
    balance += amount;//����ֵ
}