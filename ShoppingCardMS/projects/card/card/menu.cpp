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

// 从标准输入读入一个值
template <class T>
static void readOneItem(T &input) {
    cin >> input;
    char ch;
    while ((ch = cin.get()) != '\n');
}

// 显示信息message, 参数pause控制是否暂停
static void message(const string &msg, bool pause) {
    cout << "\n\t\t\t     " << msg;
    if (pause) {
        cout << "\n\t\t\t     ";
        system("pause");
    }
}
// 读入密码输入
static string readPasswd() {
    char ch;
    string str;
    
    while ((ch = getch()) != '\r') {
        //不是回撤就录入
        if(ch != '\b') {
            str += ch;
            putchar('*'); //输出*号
        }
        else {
            // 显示回撤效果
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
// 重载readPasswd，显示hint后读入密码
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
        cout << "\t\t\t     //                  欢迎使用本购物卡管理系统！                  //" << endl;
        cout << "\t\t\t     //                       请选择登陆方式:                        //" << endl;
        cout << "\t\t\t     //                         a.商家登陆                           //" << endl;
        cout << "\t\t\t     //                         b.客户登陆                           //" << endl;
        cout << "\t\t\t     //                         c.退出系统                           //" << endl;
        cout << "\t\t\t     //                                                              //" << endl;
        cout << "\t\t\t     ------------------------------------------------------------------" << endl;
        cout << "\t\t\t     ";
        
        //读取一个输入，根据相应输入进入相应菜单或退出
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
            message("输入有误, 请重新输入!", PAUSE);
        }
	}
}

void Menu::sellerLogin() {
    // 从文件中解密获得密码
    string passwd;
    passwd = Decryption(ADMIN_PASSWD_FILE);
    
    system("cls");
    title1("商户登陆系统");
    splitline();
    if (readPasswd("密码: ") == passwd) {
        // 密码正确进入功能菜单
        sellerMenu();
    }
    else {
        message("密码错误", PAUSE);
    }
}

void Menu::customerLogin(){
    
    vector<string> hints{ "客户登陆系统", "账户名", "密码" }; // 显示的提示
    vector<bool> echo{ true, false }; // 待读取的输入是否回显
    vector<string> input; // input[0]存放账户名， input[1]存放密码
    
    input = MenuWithInput(hints, echo);
    int index;
    
    // 用户名存在并且密码正确则进入用户功能菜单
    if (shop->lookupByName(input[0].c_str(), index)) {
        Customer *c = shop->getAccount(index);
        if (c->verifyPasswd(input[1].c_str())) {
            customer = c;
            customerMenu();
            return;
        }
    } 
    
    message("输入有误", PAUSE);
}

void Menu::sellerMenu() {
    
    while (true) {
        // 显示功能菜单
        sellerTitle();
        vector<string> items{ "卡办理",  "卡作废", "更改参数", "账户充值", "发起消费", "客户查询", "保存修改并退出" };
        functionMenu("主菜单", items);
        
        int input = -1;

        // 读取输入并进入对应菜单
        readOneItem(input);
        if (input >= 1 && input <= 6) {
            sellerChoices(input);
        }
        else if (input == 7) {
            break;
        }
        else {
            message("请重新输入", PAUSE);
        }
    }
}

void Menu::sellerChoices(int choice) {
    
    switch (choice) {
        case 1: // 卡办理
        {
            vector<string> hints{ "", "请输入姓名", "请输入电话号码", "请输入密码", "请再次输入密码" };
            vector<bool> echo{ true, true, false, false };
            vector<string> input;
            uint64_t phone;
            
            input = MenuWithInput(hints, echo);
            if (input[2] == input[3] && input[2].size() < 13) {
                phone = (uint64_t)strtod(input[1].c_str(), 0);
                shop->addAccount(input[0].c_str(), phone, input[2].c_str());
                message("操作成功", PAUSE);
            }
            else {
                message("输入有误", PAUSE);
            }
            
            break;
        }
        case 2: // 卡作废
        {
            int id = -1;
            message("请输入卡号:", NOPAUSE);
            readOneItem(id);
            if (shop->delAccount(id)) {
                message("操作成功", PAUSE);
            }
            else {
                message("该卡号不存在", PAUSE);
            }
            break;
		}
        case 3: // 更改参数
		{
			vector<string> items{ "更改积分兑换下限", "更改积分兑换比例", "返回" };
			functionMenu("主菜单", items);

			int input = -1;
			readOneItem(input);
			if (input == 1) {
				int val;
				message("请输入数值：", NOPAUSE);
				readOneItem(val);
				if (val > 0) {
					shop->setMinPoint(val);
					message("操作成功", PAUSE);
				}
				else {
					message("输入有误", PAUSE);
				}
			}
			else if (input == 2) {
				double val;
				message("请输入数值：", NOPAUSE);
				readOneItem(val);
				if (val > 0 && val < 1) {
					shop->setRatio(val);
					message("操作成功", PAUSE);
				}
				else {
					message("输入有误", PAUSE);
				}
			}
			else if (input != 3) {
				message("输入有误", PAUSE);
			}
			break;
		}
		case 4: // 账户充值
		{
			vector<string> hints{ "", "请输入卡号", "请输入充值金额" };
			vector<bool> echo{ true, true };
			vector<string> input;
			int id;
			double amount;

			input = MenuWithInput(hints, echo);
			id = (int)strtod(input[0].c_str(), 0);
			amount = strtod(input[1].c_str(), 0);
			if (shop->deposit(id, amount)) {
				message("操作成功", PAUSE);
			}
			else {
				message("输入有误", PAUSE);
			}
			break;
		}
		case 5: // 发起消费
		{
			vector<string> hints{ "", "请输入卡号", "请输入消费金额" };
			vector<bool> echo{ true, true };
			vector<string> input;
			int index, id;
			double amount;

			input = MenuWithInput(hints, echo);
			id = (int)strtod(input[0].c_str(), 0);
			amount = strtod(input[1].c_str(), 0);
			if (shop->consume(id, amount)) {
				message("操作成功", PAUSE);
			}
			else if (shop->lookupById(id, index) && shop->getAccount(index)->isLocked()) {
				message("账号已冻结", PAUSE);
			}
			else {
				message("输入有误", PAUSE);
			}
			break;
		}
		case 6: // 查询客户
		{
			vector<string> items{ "按姓名查询", "按电话号码查询", "按卡号查询", "返回" };
			functionMenu("客户查询", items);

			int input = -1;
			readOneItem(input);
			Customer *curr_customer = nullptr;

			switch (input) {
				case 1: // 按姓名查询
				{
					string name;
					int index;
					vector<int> accounts;

					message("请输入姓名:", NOPAUSE);
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
                case 2: // 按电话号码查询
                {
                    uint64_t phone;
                    int index;
                    message("请输入电话号码：", NOPAUSE);
                    readOneItem(phone);
                    if (shop->lookupByPhone(phone, index)) {
                        curr_customer = shop->getAccount(index);
                    }
                    break;
                }
                case 3: // 按卡号查询
                {
                    int id, index;
                    message("请输入卡号:", NOPAUSE);
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
                    message("输入错误", PAUSE);
                    return ;
                }
            }
            if (curr_customer) {
                // 如果用户存在那么显示相应信息
                showCustomerInfo(curr_customer);
                message("", PAUSE);
            }
            else {
                message("用户不存在", PAUSE);
            }
        }
    }
}

void Menu::customerMenu() {

	while (true) {
        customerTitle();
        vector<string> items{ "账户充值", "卡片挂失",  "解除挂失", "积分兑换", "查看账户信息", "修改账户信息", "保存修改并退出" };
        functionMenu("主菜单", items);
        
        int input = -1;
        readOneItem(input);
		if (input >= 1 && input <= 6) {
			customerChoices(input);
		}
	    else if (input == 7) {
            break;
        }
        else {
            message("输入有误", PAUSE);
        }
    }
}

void Menu::customerChoices(int choice) {
    
	switch (choice) {
	    case 1: //账户充值
	    {
            double amount;
            message("请输入需要充值金额:", NOPAUSE);
            readOneItem(amount);
            customer->deposit(amount);
            message("充值成功，当前余额:", NOPAUSE);
            cout << customer->getBalance() << "元！" << endl;
            message("", PAUSE);
            break;
        }
	    case 2: // 卡片挂失
	    {
            if (customer->isLocked()) {
                message("该卡已经挂失!", PAUSE);
            }
            else {
                customer->setLocked(true);
                message("挂失成功！", PAUSE);
            }
            break;
        }
        case 3: // 解除挂失
        {
            if (!customer->isLocked()) {
                message("该卡并未挂失!", PAUSE);
            }
            else {
                customer->setLocked(false);
                message("解除挂失成功！", PAUSE);
            }
            break;
        }
	    case 4: // 积分兑换
	    {
            if (customer->pointConvert()) {
                message("积分兑换余额成功，当前余额:", NOPAUSE);
                cout << customer->getBalance() << "元！" << endl;
                message("", PAUSE);
            }
            else {
                message("积分不足，兑换失败！", PAUSE);
            }
            break;
	    }
	    case 5: // 查询账户信息
	    {
            showCustomerInfo(customer);
            message("", PAUSE);
            break;
        }
	    case 6: // 修改账户信息
	    {
            vector<string> items{ "修改账户密码","修改预留电话号码", "返回" };
            functionMenu("客户自助信息修改", items);

            int input;
            readOneItem(input);
            if (input == 1) {
                string passwd, passwdConfirm1, passwdConfirm2;

                passwd = readPasswd("请输入原密码: ");
                passwdConfirm1 = readPasswd("请输入修改后的密码: ");
                passwdConfirm2 = readPasswd("请再次输入密码: ");
                if (passwdConfirm1 == passwdConfirm2 && passwdConfirm1.size() < 12
                    && customer->setPasswd(passwd.c_str(), passwdConfirm1.c_str())) {
                        message("修改成功", PAUSE);
                }
                else {
                    message("密码输入错误", PAUSE);
                }
			}
			else if (input == 2) {
				uint64_t phoneNumber;
                message("请输入修改后的电话号码:", NOPAUSE);
                readOneItem(phoneNumber);
                if (phoneNumber == customer->getPhoneNum()) {
                    message("新的电话号码与您预存的电话号码相同", PAUSE);
                }
                else {
                    customer->setPhoneNum(phoneNumber);
                    message("修改成功", PAUSE);
                }
			}
			else if (input != 3) {
                message("输入有误，请重新输入!", PAUSE);
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
    // 功能菜单格式如下：
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
    
    vector<CONSOLE_SCREEN_BUFFER_INFO> points; // 保存每一次输入的位置
    CONSOLE_SCREEN_BUFFER_INFO point;
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);//获得当前输出窗口句柄
    vector<string> input; // 存储所有的输入, 函数结束时返回
    string item;
    
    system("cls");
    title1(hints[0]);
    splitline();
    
    for (unsigned i = 1; i < hints.size(); i++) {
        cout << "\t\t\t     " + hints[i] + ": ";
        GetConsoleScreenBufferInfo(hOutput, &point); // 记录每一个待读取的输入位置
        points.push_back(point);
        cout << endl;
    }
    splitline();
    
    GetConsoleScreenBufferInfo(hOutput, &point);// 记录最后的输入位置
    
    for (unsigned i = 0; i < points.size(); i++) {
        SetConsoleCursorPosition(hOutput, points[i].dwCursorPosition);//定光标位置到相应输入位置
        // 根据echo决定读取方式
        if (echo[i]) {
            readOneItem(item);
        }
        else {
            item = readPasswd();
        }
        input.push_back(item);
    }
    
    SetConsoleCursorPosition(hOutput, point.dwCursorPosition); // 定位到最后的输入位置
    
    return input;
}

void Menu::showCustomerInfo(Customer *c) {
    time_t register_time = c->getRegisterTime();
	char str[50];
    
	strftime(str, sizeof(str), "%F %X", localtime(&register_time));

    cout << endl;
    cout << "\t\t\t\t     您的账户信息如下:" << endl;
    cout << "\t\t\t\t     账户ID: " << c->getID() << endl;
    cout << "\t\t\t\t     户主姓名: " << c->getName() << endl;
    cout << "\t\t\t\t     预留电话: " << c->getPhoneNum() << endl;
    cout << "\t\t\t\t     账户余额: " << c->getBalance() << endl;
    cout << "\t\t\t\t     账户积分: " << c->getPoint() << endl;
	cout << "\t\t\t\t     开卡时间: " << str << endl;
    if (c->isLocked()) {
        cout << "\t\t\t\t     账户状态:冻结" << endl;
    }
    else {
        cout << "\t\t\t\t     账户状态:激活" << endl;
    }
}

void Menu::sellerTitle() {
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *               商家管理端                *" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t---------------------------------------------------------------------" << endl;
}

void Menu::customerTitle() {
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *             客户自助管理端              *" << endl;
    cout << "\t\t\t\t     *                                         *" << endl;
    cout << "\t\t\t\t     *******************************************" << endl;
    cout << "\t\t\t---------------------------------------------------------------------" << endl;
}

// 将passwd加密到文件
void Menu::Encryption(const string& passwd) {
	char temp;
	ofstream fout(ADMIN_PASSWD_FILE);
	for (unsigned i = 0; i < passwd.size(); i++) {
		temp = passwd[i] ^ KEY;
		fout << temp;
	}
	fout.close();
}

// 从passwdFile解密得到密码
string Menu::Decryption(const char* passwdFile) {
	string psd;
	char temp;
	ifstream fin(passwdFile);
	if (fin.fail()) {
		cout << "暂时无法登陆，请联系系统管理员！" << endl;
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
