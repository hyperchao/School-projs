#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <Windows.h>
#include "menu.h"
#include <cstdint>

using namespace std;

const char *ADMIN_PASSWD_FILE = "passwd";
const int KEY = 0x6a;
const bool PAUSE = true;
const bool NOPAUSE = false;

// �ӱ�׼�������һ��ֵ
template <class T>
static void readOneItem(T &input) {
    cin >> input;
    char ch;
    while ((ch = cin.get()) != '\n');
}

// ��ʾ��Ϣmessage, ����pause�����Ƿ���ͣ
static void message(const string &msg, bool pause) {
    cout << "\n\t\t\t     " << msg;
    if (pause) {
        cout << "\n\t\t\t     ";
        system("pause");
    }
}
// ������������
static string readPasswd() {
    char ch;
    string str;
    
    while ((ch = getch()) != '\r') {
        //���ǻس���¼��
        if(ch != '\b') {
            str += ch;
            putchar('*'); //���*��
        }
        else {
            // ��ʾ�س�Ч��
            if (!str.empty()) {
                putchar('\b');
                putchar(' ');
                putchar('\b');
                str.pop_back();
            }
        }
    }
    return str;
}
// ����readPasswd����ʾhint���������
static string readPasswd(const string &hint) {
    cout << "\n\t\t\t     " + hint;
    return readPasswd();
}

Menu::Menu() : shop(new Shop), customer(nullptr) { }

Menu::~Menu() {
    delete shop;
}

void Menu::Welcome() {
    
    while (true) {
        system("cls");
        cout << "\t\t\t     ------------------------------------------------------------------" << endl;
        cout << "\t\t\t     //                                                              //" << endl;
        cout << "\t\t\t     //                  ��ӭʹ�ñ����￨����ϵͳ��                  //" << endl;
        cout << "\t\t\t     //                       ��ѡ���½��ʽ:                        //" << endl;
        cout << "\t\t\t     //                         a.�̼ҵ�½                           //" << endl;
        cout << "\t\t\t     //                         b.�ͻ���½                           //" << endl;
        cout << "\t\t\t     //                         c.�˳�ϵͳ                           //" << endl;
        cout << "\t\t\t     //                                                              //" << endl;
        cout << "\t\t\t     ------------------------------------------------------------------" << endl;
        cout << "\t\t\t     ";
        
        //��ȡһ�����룬������Ӧ���������Ӧ�˵����˳�
        string input;
        readOneItem(input);
        if (input == "a" || input == "A") {
            sellerLogin();
        }
        else if (input == "b" || input == "B") {
            customerLogin();
        }
        else if (input == "c" || input == "C") {
            break;
        }
        else {
            message("��������, ����������!", PAUSE);
        }
	}
}

void Menu::sellerLogin() {
    // ���ļ��н��ܻ������
    string passwd;
    passwd = Decryption(ADMIN_PASSWD_FILE);
    
    system("cls");
    title1("�̻���½ϵͳ");
    splitline();
    if (readPasswd("����: ") == passwd) {
        // ������ȷ���빦�ܲ˵�
        sellerMenu();
    }
    else {
        message("�������", PAUSE);
    }
}

void Menu::customerLogin(){
    
    vector<string> hints{ "�ͻ���½ϵͳ", "�˻���", "����" }; // ��ʾ����ʾ
    vector<bool> echo{ true, false }; // ����ȡ�������Ƿ����
    vector<string> input; // input[0]����˻����� input[1]�������
    
    input = MenuWithInput(hints, echo);
    int index;
    
    // �û������ڲ���������ȷ������û����ܲ˵�
    if (shop->lookupByName(input[0].c_str(), index)) {
        Customer *c = shop->getAccount(index);
        if (c->verifyPasswd(input[1].c_str())) {
            customer = c;
            customerMenu();
            return;
        }
    } 
    
    message("��������", PAUSE);
}

void Menu::sellerMenu() {
    
    while (true) {
        // ��ʾ���ܲ˵�
        sellerTitle();
        vector<string> items{ "������",  "������", "���Ĳ���", "�˻���ֵ", "��������", "�ͻ���ѯ", "�����޸Ĳ��˳�" };
        functionMenu("���˵�", items);
        
        int input = -1;

        // ��ȡ���벢�����Ӧ�˵�
        readOneItem(input);
        if (input >= 1 && input <= 6) {
            sellerChoices(input);
        }
        else if (input == 7) {
            break;
        }
        else {
            message("����������", PAUSE);
        }
    }
}

void Menu::sellerChoices(int choice) {
    
    switch (choice) {
        case 1: // ������
        {
            vector<string> hints{ "", "����������", "������绰����", "����������", "���ٴ���������" };
            vector<bool> echo{ true, true, false, false };
            vector<string> input;
            uint64_t phone;
            
            input = MenuWithInput(hints, echo);
            if (input[2] == input[3] && input[2].size() < 13) {
                phone = (uint64_t)strtod(input[1].c_str(), 0);
                shop->addAccount(input[0].c_str(), phone, input[2].c_str());
                message("�����ɹ�", PAUSE);
            }
            else {
                message("��������", PAUSE);
            }
            
            break;
        }
        case 2: // ������
        {
            int id = -1;
            message("�����뿨��:", NOPAUSE);
            readOneItem(id);
            if (shop->delAccount(id)) {
                message("�����ɹ�", PAUSE);
            }
            else {
                message("�ÿ��Ų�����", PAUSE);
            }
            break;
		}
        case 3: // ���Ĳ���
		{
			vector<string> items{ "���Ļ��ֶһ�����", "���Ļ��ֶһ�����", "����" };
			functionMenu("���˵�", items);

			int input = -1;
			readOneItem(input);
			if (input == 1) {
				int val;
				message("��������ֵ��", NOPAUSE);
				readOneItem(val);
				if (val > 0) {
					shop->setMinPoint(val);
					message("�����ɹ�", PAUSE);
				}
				else {
					message("��������", PAUSE);
				}
			}
			else if (input == 2) {
				double val;
				message("��������ֵ��", NOPAUSE);
				readOneItem(val);
				if (val > 0 && val < 1) {
					shop->setRatio(val);
					message("�����ɹ�", PAUSE);
				}
				else {
					message("��������", PAUSE);
				}
			}
			else if (input != 3) {
				message("��������", PAUSE);
			}
			break;
		}
		case 4: // �˻���ֵ
		{
			vector<string> hints{ "", "�����뿨��", "�������ֵ���" };
			vector<bool> echo{ true, true };
			vector<string> input;
			int id;
			double amount;

			input = MenuWithInput(hints, echo);
			id = (int)strtod(input[0].c_str(), 0);
			amount = strtod(input[1].c_str(), 0);
			if (shop->deposit(id, amount)) {
				message("�����ɹ�", PAUSE);
			}
			else {
				message("��������", PAUSE);
			}
			break;
		}
		case 5: // ��������
		{
			vector<string> hints{ "", "�����뿨��", "���������ѽ��" };
			vector<bool> echo{ true, true };
			vector<string> input;
			int index, id;
			double amount;

			input = MenuWithInput(hints, echo);
			id = (int)strtod(input[0].c_str(), 0);
			amount = strtod(input[1].c_str(), 0);
			if (shop->consume(id, amount)) {
				message("�����ɹ�", PAUSE);
			}
			else if (shop->lookupById(id, index) && shop->getAccount(index)->isLocked()) {
				message("�˺��Ѷ���", PAUSE);
			}
			else {
				message("��������", PAUSE);
			}
			break;
		}
		case 6: // ��ѯ�ͻ�
		{
			vector<string> items{ "��������ѯ", "���绰�����ѯ", "�����Ų�ѯ", "����" };
			functionMenu("�ͻ���ѯ", items);

			int input = -1;
			readOneItem(input);
			Customer *curr_customer = nullptr;

			switch (input) {
				case 1: // ��������ѯ
				{
					string name;
					int index;
					vector<int> accounts;

					message("����������:", NOPAUSE);
					readOneItem(name);
					accounts = shop->lookupByName(name.c_str());
					if (!accounts.empty()) {
						for (unsigned i = 0; i < accounts.size(); i++) {
                            curr_customer = shop->getAccount(accounts[i]);
                            showCustomerInfo(curr_customer);
						}
					}
                    break;
                }
                case 2: // ���绰�����ѯ
                {
                    uint64_t phone;
                    int index;
                    message("������绰���룺", NOPAUSE);
                    readOneItem(phone);
                    if (shop->lookupByPhone(phone, index)) {
                        curr_customer = shop->getAccount(index);
                    }
                    break;
                }
                case 3: // �����Ų�ѯ
                {
                    int id, index;
                    message("�����뿨��:", NOPAUSE);
                    readOneItem(id);
                    if (shop->lookupById(id, index)) {
                        curr_customer = shop->getAccount(index);
                    }
                    break;
                }
                case 4:
                {
                    return;
                }
                default:
                {
                    message("�������", PAUSE);
                    return ;
                }
            }
            if (curr_customer) {
                // ����û�������ô��ʾ��Ӧ��Ϣ
                showCustomerInfo(curr_customer);
                message("", PAUSE);
            }
            else {
                message("�û�������", PAUSE);
            }
        }
    }
}

void Menu::customerMenu() {

	while (true) {
        customerTitle();
        vector<string> items{ "�˻���ֵ", "��Ƭ��ʧ",  "�����ʧ", "���ֶһ�", "�鿴�˻���Ϣ", "�޸��˻���Ϣ", "�����޸Ĳ��˳�" };
        functionMenu("���˵�", items);
        
        int input = -1;
        readOneItem(input);
		if (input >= 1 && input <= 6) {
			customerChoices(input);
		}
	    else if (input == 7) {
            break;
        }
        else {
            message("��������", PAUSE);
        }
    }
}

void Menu::customerChoices(int choice) {
    
	switch (choice) {
	    case 1: //�˻���ֵ
	    {
            double amount;
            message("��������Ҫ��ֵ���:", NOPAUSE);
            readOneItem(amount);
            customer->deposit(amount);
            message("��ֵ�ɹ�����ǰ���:", NOPAUSE);
            cout << customer->getBalance() << "Ԫ��" << endl;
            message("", PAUSE);
            break;
        }
	    case 2: // ��Ƭ��ʧ
	    {
            if (customer->isLocked()) {
                message("�ÿ��Ѿ���ʧ!", PAUSE);
            }
            else {
                customer->setLocked(true);
                message("��ʧ�ɹ���", PAUSE);
            }
            break;
        }
        case 3: // �����ʧ
        {
            if (!customer->isLocked()) {
                message("�ÿ���δ��ʧ!", PAUSE);
            }
            else {
                customer->setLocked(false);
                message("�����ʧ�ɹ���", PAUSE);
            }
            break;
        }
	    case 4: // ���ֶһ�
	    {
            if (customer->pointConvert()) {
                message("���ֶһ����ɹ�����ǰ���:", NOPAUSE);
                cout << customer->getBalance() << "Ԫ��" << endl;
                message("", PAUSE);
            }
            else {
                message("���ֲ��㣬�һ�ʧ�ܣ�", PAUSE);
            }
            break;
	    }
	    case 5: // ��ѯ�˻���Ϣ
	    {
            showCustomerInfo(customer);
            message("", PAUSE);
            break;
        }
	    case 6: // �޸��˻���Ϣ
	    {
            vector<string> items{ "�޸��˻�����","�޸�Ԥ���绰����", "����" };
            functionMenu("�ͻ�������Ϣ�޸�", items);

            int input;
            readOneItem(input);
            if (input == 1) {
                string passwd, passwdConfirm1, passwdConfirm2;

                passwd = readPasswd("������ԭ����: ");
                passwdConfirm1 = readPasswd("�������޸ĺ������: ");
                passwdConfirm2 = readPasswd("���ٴ���������: ");
                if (passwdConfirm1 == passwdConfirm2 && passwdConfirm1.size() < 12
                    && customer->setPasswd(passwd.c_str(), passwdConfirm1.c_str())) {
                        message("�޸ĳɹ�", PAUSE);
                }
                else {
                    message("�����������", PAUSE);
                }
			}
			else if (input == 2) {
				uint64_t phoneNumber;
                message("�������޸ĺ�ĵ绰����:", NOPAUSE);
                readOneItem(phoneNumber);
                if (phoneNumber == customer->getPhoneNum()) {
                    message("�µĵ绰��������Ԥ��ĵ绰������ͬ", PAUSE);
                }
                else {
                    customer->setPhoneNum(phoneNumber);
                    message("�޸ĳɹ�", PAUSE);
                }
			}
			else if (input != 3) {
                message("������������������!", PAUSE);
            }
            break;
		}
	}
}

void Menu::title1(const string &title) {
    cout << "\t\t\t                                 " + title << endl;
}

void Menu::splitline() {
    cout << "\t\t\t     ------------------------------------------------------------------" << endl;
}

void Menu::functionMenu(const string &title, vector<string> &item) {
    // ���ܲ˵���ʽ���£�
    //     ******************title*******************
    //     >>>           1.item[0]                <<<
    //     >>>           2.item[1]                <<<
    //     >>>              ...                   <<<
    //     ******************************************
    
    int left, right;
    left = (43 - title.size())/2;
    right = 43 - title.size() - left;
    
    system("cls");
    cout << "\t\t\t\t     " + string(left, '*') + title + string(right, '*') << endl;
    for (unsigned i = 0; i < item.size(); i++) {
        cout << "\t\t\t\t     >>>           " << i + 1 << ".";
        cout << item[i];
        cout << string(24 - item[i].size(), ' ') << "<<<" << endl;
    }
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t     ";
}

vector<string> Menu::MenuWithInput(vector<string> &hints, vector<bool> &echo) {
    
    vector<CONSOLE_SCREEN_BUFFER_INFO> points; // ����ÿһ�������λ��
    CONSOLE_SCREEN_BUFFER_INFO point;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);//��õ�ǰ������ھ��
    vector<string> input; // �洢���е�����, ��������ʱ����
    string item;
    
    system("cls");
    title1(hints[0]);
    splitline();
    
    for (unsigned i = 1; i < hints.size(); i++) {
        cout << "\t\t\t     " + hints[i] + ": ";
        GetConsoleScreenBufferInfo(hOutput, &point); // ��¼ÿһ������ȡ������λ��
        points.push_back(point);
        cout << endl;
    }
    splitline();
    
    GetConsoleScreenBufferInfo(hOutput, &point);// ��¼��������λ��
    
    for (unsigned i = 0; i < points.size(); i++) {
        SetConsoleCursorPosition(hOutput, points[i].dwCursorPosition);//�����λ�õ���Ӧ����λ��
        // ����echo������ȡ��ʽ
        if (echo[i]) {
            readOneItem(item);
        }
        else {
            item = readPasswd();
        }
        input.push_back(item);
    }
    
    SetConsoleCursorPosition(hOutput, point.dwCursorPosition); // ��λ����������λ��
    
    return input;
}

void Menu::showCustomerInfo(Customer *c) {
    time_t register_time = c->getRegisterTime();
	char str[50];
    
	strftime(str, sizeof(str), "%F %X", localtime(&register_time));

    cout << endl;
    cout << "\t\t\t\t     �����˻���Ϣ����:" << endl;
    cout << "\t\t\t\t     �˻�ID: " << c->getID() << endl;
    cout << "\t\t\t\t     ��������: " << c->getName() << endl;
    cout << "\t\t\t\t     Ԥ���绰: " << c->getPhoneNum() << endl;
    cout << "\t\t\t\t     �˻����: " << c->getBalance() << endl;
    cout << "\t\t\t\t     �˻�����: " << c->getPoint() << endl;
	cout << "\t\t\t\t     ����ʱ��: " << str << endl;
    if (c->isLocked()) {
        cout << "\t\t\t\t     �˻�״̬:����" << endl;
    }
    else {
        cout << "\t\t\t\t     �˻�״̬:����" << endl;
    }
}

void Menu::sellerTitle() {
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *               �̼ҹ����                *" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t---------------------------------------------------------------------" << endl;
}

void Menu::customerTitle() {
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *             �ͻ����������              *" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t---------------------------------------------------------------------" << endl;
}

// ��passwd���ܵ��ļ�
void Menu::Encryption(const string& passwd) {
	char temp;
	ofstream fout(ADMIN_PASSWD_FILE);
	for (unsigned i = 0; i < passwd.size(); i++) {
		temp = passwd[i] ^ KEY;
		fout << temp;
	}
	fout.close();
}

// ��passwdFile���ܵõ�����
string Menu::Decryption(const char* passwdFile) {
	string psd;
	char temp;
	ifstream fin(passwdFile);
	if (fin.fail()) {
		cout << "��ʱ�޷���½������ϵϵͳ����Ա��" << endl;
		system("pause");
		exit(0);
	}
	fin >> temp;
	for (int i = 0; !fin.eof();i++) {
		psd += temp^KEY;
		fin >> temp;
	}
	fin.close();
	return psd;
}
