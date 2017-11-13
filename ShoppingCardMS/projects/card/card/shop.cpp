#include <iostream>
#include <cstring>
#include <fstream>
#include "shop.h"
#include<vector>

using namespace std;

const char *DATA_FILE = "shop.dat";//����һ��ָ��洢�ͻ���Ϣ�ļ���ָ��

Record Shop::record;

Shop::Shop() {
    
    ifstream file(DATA_FILE, ios::binary);//�����������ļ������Զ����Ʒ�ʽ���ļ�
    
    if (file) {
        file.read((char*)&record, sizeof(Record));//���ļ��ж������ݵ�record
        Customer c("name", 0, 0, "passwd");
        int size;
        file.read((char*)&size, sizeof(int)); //���ļ��ж������ݵ�size
        for (int i = 0; i < size; i++) {
            file.read((char*)&c, sizeof(Customer));//���ļ��ж������ݵ�customer c
            group.push_back(c);//��groupβ������customer c
        }
    }
}

Shop::~Shop() {
    
    ofstream file(DATA_FILE, ios::binary);//����������ļ������Զ����Ʒ�ʽ��ȡ�ļ�
    
    if (file) {
        file.write((char*)&record, sizeof(Record));//д���ݵ�record��
        int size = group.size();
        file.write((char*)&size, sizeof(int));//дԪ�ظ�����size��
        file.write((char*)group.data(), sizeof(group[0])*size);
    }
}

int Shop::getMinConvertPoint() {
    return record.min_point;//���ػ��ֶһ�����
}

double Shop::getRatio() {
    return record.ratio;//���ػ��ֶһ�����
}

void Shop::addAccount(const char *name, uint64_t phone, const char *passwd) {
    
    int new_id;
    
    if(!group.empty()){//�û���Ϊ��
        Customer &c = group.back();
        new_id = c.getID() + 1;//���û����Ϊ���һ���û����+1
    }
    else{
        new_id = 1;//������1��ʼΪ�û����
    }
    
    Customer newcustomer(name, phone, new_id, passwd);
    group.push_back(newcustomer);//�����û�����Ϣ��������
}     

bool Shop::delAccount(int account_id) {//ɾ���û���Ϣ
    
    int index;
    
    if (lookupById(account_id, index)) {
        group.erase(group.begin() + index); //ɾ��������ָ���˻�
        return true;//ɾ���ɹ�
    }
    else {
        return false;//ɾ��ʧ��
    }
}

Customer *Shop::getAccount(int index) {//��ȡ�û���Ϣ
    return &group[index];
}

bool Shop::lookupByName(const char *name, int &index) {//ͨ�����������û�
    
    for(int i = 0; i < group.size(); i++) {
        if(strcmp(group[i].getName(), name) == 0) {//�ȶ�����
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

bool Shop::lookupByPhone(uint64_t phone, int &index) {//ͨ����������û�
    
    for(int i = 0; i < group.size(); i++) {
        if (group[i].getPhoneNum() == phone) {//�ȶԺ���
            index = i;
            return true;
        }
    }
    
    return false;
}

bool Shop::lookupById(int id, int &index)//ͨ����Ų����û�
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


bool Shop::deposit(int account_id, double amount) {//��ֵ
    
    int index;
    
    if (lookupById(account_id, index)) {//ͨ����Ų��ҳ�ֵ
        group[index].deposit(amount);//����customer�е�deposit����ʵ���˻���ֵ
        return true;//��ֵ�ɹ�
    }
    else {
        return false;//��ֵʧ��
    }
}

bool Shop::consume(int account_id, double amount) {//����
    
    int index;
    
    if (lookupById(account_id, index) && !group[index].isLocked()) {//�����ȷ�ҹ��￨û��
        double balance = group[index].getBalance() - amount;//���ı�
        if (balance >= 0) {
            group[index].setBalance(balance);
            group[index].addPoint(amount);
            return true; //�ۿ�ɹ�
        }
    }
    
    return false; //���㣬�ۿ�ʧ��
}

void Shop::setMinPoint(int val) {//���û��ֶһ�����
    record.min_point = val;
}

void Shop::setRatio(double val) {//���û��ֶһ�����
    record.ratio = val;
}
