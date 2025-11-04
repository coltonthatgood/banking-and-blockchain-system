#include <string>
#include <vector>
#include <cstdint>
#include "splashkit.h"
#include "utilities.h"
#include "bank.h"
#include <iomanip>
#include <openssl/sha.h>

using std::string;
using std::to_string;

SavingsAccount temp;
SavingsAccount *sa = &temp;

__uint128_t hashing(const string &password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)password.c_str(), password.size(), hash);

    // Take first 16 bytes (128 bits)
    __uint128_t result = 0;
    for (int i = 0; i < 16; ++i) {
        result = (result << 8) | hash[i];
    }
    return result;
}

void add_transaction(string date, string passkey, string sender, string receiver, double amount, std::vector<transfer> &transactions)
{
    transfer w;
    w.date = date;
    w.passkey = passkey;
    w.sender = sender;
    w.receiver = receiver;
    w.amount = amount;
    transactions.push_back(w);
    w.id++;
}

void print(string passkey, std::vector<transfer> transactions)
{
    for (unsigned int i = 0; i < transactions.size(); i++)
    {
        if (passkey == transactions[i].passkey)
        {
            write_line(transactions[i].date + transactions[i].sender + transactions[i].receiver + to_string(transactions[i].amount));
        }
    }
}

bool login_data(string username, string password)
{
    for (int i = 0; i < list_accounts.size(); i++)
    {
        if (list_accounts[i].SavingsAccount::get_username() == username && list_accounts[i].SavingsAccount::get_password() == hashing(password))
        {
            sa = &list_accounts[i];
            return true;
        }
    }
    return false;
}
void login()
{
    bool logged = false;
    while (!logged)
    {
        string username = read_string("enter username");
        string password = read_string("enter password");
        if (login_data(username, password))
        {
            logged = true;
        }
    }
}

void SavingsAccount::withdraw()
{
    transfer t;
    t.date = read_string("please enter date");
    t.passkey = read_string("please enter passkey");
    t.sender = "bank";
    t.receiver = SavingsAccount::get_username();
    t.amount = read_double("enter amount");
    add_transaction(t.date, t.passkey, t.sender, t.receiver, t.amount, get_transactions());
    this->balance -= t.amount;
}

void SavingsAccount::deposit()
{
    transfer t;
    t.date = read_string("please enter date");
    t.passkey = read_string("please enter passkey");
    t.sender = SavingsAccount::get_username();
    t.receiver = "bank";
    t.amount = read_double("enter amount");
    add_transaction(t.date, t.passkey, t.sender, t.receiver, t.amount, get_transactions());
    this->balance += t.amount;
}

void SavingsAccount::transaction()
{
    transfer t;
    t.date = read_string("please enter date");
    t.passkey = read_string("please enter passkey");
    t.sender = SavingsAccount::get_username();
    t.amount = read_double("enter amount");
    t.receiver = read_string("please enter receiver:");
    bool sent = false;
    for (int i = 0; i < list_accounts.size(); i++)
    {
        if (list_accounts[i].username == t.receiver)
        {
            list_accounts[i].balance += t.amount;
            add_transaction(t.date, t.passkey, t.sender, t.receiver, t.amount, get_transactions());
            this->balance -= t.amount;
            write_line("transaction successful!");
            sent = true;
            break;
        }
    }
    if(!sent)
    {
        write_line("transaction failed");
    }
}


void SavingsAccount::print_account()
{
    write_line(get_balance());
    write_line(get_username());
}

void add_account(SavingsAccount &s)
{
    bool created = false;
    while (!created)
    {
        s.username = read_string("please enter username");
        for (int i = 0; i < list_accounts.size(); i++)
        {
            if (list_accounts[i].username == s.username)
            {
                write_line("username used, please try again");
                created = true;
            }
        }
        s.password_hash = hashing(read_string("please enter password"));
        s.balance = 500;
        list_accounts.push_back(s);
        sa = &temp; // force user to login
        created = true;
    }
}

void SavingsAccount::kill_switch()
{
    sa->balance = 0;
    sa = &temp;
}

void interface()
{
    bool logged = false;
    while(!logged){
    int login_choice = read_integer("Please choose one option(1 to login, 2 to add account):");
        if (login_choice == 1){
            login();
            logged = true;
        }
        else if (login_choice == 2)
        {
            add_account(*sa);
        }
        else
        {
            continue;
        }
    }
}

void logout(SavingsAccount *sa)
{
    sa = &temp;
}

int main()
{
    add_account(*sa);
    while (true)
    {
        interface();

        enum choices
        {
            DEPOSIT = 1,
            WITHDRAW = 2,
            TRANSFER = 3,
            KILLSWITCH = 4,
            PRINT = 5,
            LOGOUT = 6
        };
        int choice;
        bool loggedin = true;
        while (loggedin)
        {
            choice = read_integer("Please choose one option(1-deposit, 2-withdraw, 3-transfer, 4-killswitch, 5-print, 6-logout):");
            switch (choice)
            {
            case DEPOSIT:
                sa->deposit();
                break;
            case WITHDRAW:
                sa->withdraw();
                break;
            case TRANSFER:
                sa->transaction();
                break;
            case KILLSWITCH:
                sa->kill_switch();
                break;
            case PRINT:
                sa->print_account();
                break;
            case LOGOUT:
                logout(sa);
                loggedin = false;
            default:
                break;
            }
        }
    }
}
