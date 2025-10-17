#pragma once
#include <vector>
#include <string>

using std::string;

double interest;
struct transfer
{
    unsigned int id = 0;
    string date;
    string passkey;
    string sender;
    string receiver;
    double amount;
};

void print(string passkey, std::vector<transfer> &transactions);
void add_transaction(string date, string passkey, string sender, string receiver, double amount, std::vector<transfer> &transactions);


class SavingsAccount
{
    private:
    string username;
    string password;
    double balance;
    std::vector<transfer> user_transactions;
    public:
    void withdraw();
    void transaction();
    void deposit();
    void print_account();
    string& get_username() { return username; }
    string& get_password() { return password; }
    double& get_balance() { return balance;}
    std::vector<transfer>& get_transactions() {return user_transactions;}
    friend void add_account(SavingsAccount &s);
    void kill_switch();
};

void add_account(SavingsAccount &s);
std::vector<SavingsAccount> list_accounts;
void login();
bool login_data(string username, string password);
void interface();
void logout(SavingsAccount *sa);
