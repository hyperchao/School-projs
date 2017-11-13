#include <iostream>
#include <cstring>
#include <fstream>
#include "shop.h"
#include<vector>

using namespace std;

const char *DATA_FILE = "shop.dat";//定义一个指向存储客户信息文件的指针

Record Shop::record;

Shop::Shop() {
    
    ifstream file(DATA_FILE, ios::binary);//定义输入流文件对象，以二进制方式打开文件
    
    if (file) {
        file.read((char*)&record, sizeof(Record));//从文件中读出数据到record
        Customer c("name", 0, 0, "passwd");
        int size;
        file.read((char*)&size, sizeof(int)); //从文件中读出数据到size
        for (int i = 0; i < size; i++) {
            file.read((char*)&c, sizeof(Customer));//从文件中读出数据到customer c
            group.push_back(c);//在group尾部新增customer c
        }
    }
}

Shop::~Shop() {
    
    ofstream file(DATA_FILE, ios::binary);//定义输出流文件对象，以二进制方式读取文件
    
    if (file) {
        file.write((char*)&record, sizeof(Record));//写数据到record中
        int size = group.size();
        file.write((char*)&size, sizeof(int));//写元素个数到size中
        file.write((char*)group.data(), sizeof(group[0])*size);
    }
}

int Shop::getMinConvertPoint() {
    return record.min_point;//返回积分兑换下限
}

double Shop::getRatio() {
    return record.ratio;//返回积分兑换比率
}

void Shop::addAccount(const char *name, uint64_t phone, const char *passwd) {
    
    int new_id;
    
    if(!group.empty()){//用户不为空
        Customer &c = group.back();
        new_id = c.getID() + 1;//新用户编号为最后一个用户编号+1
    }
    else{
        new_id = 1;//否则以1开始为用户编号
    }
    
    Customer newcustomer(name, phone, new_id, passwd);
    group.push_back(newcustomer);//将新用户的信息储存起来
}     

bool Shop::delAccount(int account_id) {//删除用户信息
    
    int index;
    
    if (lookupById(account_id, index)) {
        group.erase(group.begin() + index); //删除容器内指定账户
        return true;//删除成功
    }
    else {
        return false;//删除失败
    }
}

Customer *Shop::getAccount(int index) {//获取用户信息
    return &group[index];
}

bool Shop::lookupByName(const char *name, int &index) {//通过姓名查找用户
    
    for(int i = 0; i < group.size(); i++) {
        if(strcmp(group[i].getName(), name) == 0) {//比对姓名
            index = i;
            return true;
        }
    }
    
    return false;
}
std::vector<int> Shop::lookupByName(const char *name)
{
	vector<int> temp;
	for (int i = 0; i < group.size(); i++)
	{
		if (strcmp(group[i].getName(), name) == 0)
		{
			temp.push_back(i);

		}
	}
	
	return temp;

}

bool Shop::lookupByPhone(uint64_t phone, int &index) {//通过号码查找用户
    
    for(int i = 0; i < group.size(); i++) {
        if (group[i].getPhoneNum() == phone) {//比对号码
            index = i;
            return true;
        }
    }
    
    return false;
}

bool Shop::lookupById(int id, int &index)//通过编号查找用户
{
	if (group.empty()) {
		return false;
	}

	int low = 0, high = group.size() - 1;

	while (low <= high) {
		int mid = ((high - low) / 2) + low;
		if (group[mid].getID() > id)
			high = mid - 1;
		else if (group[mid].getID() < id)
			low = mid + 1;
		else {
			index = mid;
			return true;
		}
	}

	return false;
}


bool Shop::deposit(int account_id, double amount) {//充值
    
    int index;
    
    if (lookupById(account_id, index)) {//通过编号查找充值
        group[index].deposit(amount);//调用customer中的deposit函数实现账户充值
        return true;//充值成功
    }
    else {
        return false;//充值失败
    }
}

bool Shop::consume(int account_id, double amount) {//消费
    
    int index;
    
    if (lookupById(account_id, index) && !group[index].isLocked()) {//编号正确且购物卡没锁
        double balance = group[index].getBalance() - amount;//余额改变
        if (balance >= 0) {
            group[index].setBalance(balance);
            group[index].addPoint(amount);
            return true; //扣款成功
        }
    }
    
    return false; //余额不足，扣款失败
}

void Shop::setMinPoint(int val) {//设置积分兑换下限
    record.min_point = val;
}

void Shop::setRatio(double val) {//设置积分兑换比率
    record.ratio = val;
}
