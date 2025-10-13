#include <string>
#include <vector>
#include "splashkit.h"
#include "utilities.h"
#include "bank.h"

using std::string;
using std::to_string;

SavingsAccount temp;
SavingsAccount *sa = &temp;

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
        if (list_accounts[i].SavingsAccount::get_username() == username && list_accounts[i].SavingsAccount::get_password() == password)
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
    string receiver;
    for (int i = 0; i < list_accounts.size(); i++)
    {
        if (list_accounts[i].username == receiver)
        {
            t.receiver = receiver;
            list_accounts[i].balance += t.amount;
        }
    }
    add_transaction(t.date, t.passkey, t.sender, t.receiver, t.amount, get_transactions());
    this->balance -= t.amount;
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
                continue;
            }
        }
        s.password = read_string("please enter password");
        s.balance = 500;
        list_accounts.push_back(s);
        sa = &temp; // force user to login
        created = true;
    }
}

void SavingsAccount::kill_switch()
{
    sa->balance = 0;
}

int main()
{
    add_account(*sa);
    while (true)
    {
        int login_choice = read_integer("Please choose one option:");
        if (login_choice == 1)
            login();
        else if (login_choice == 2)
            add_account(*sa);
        else
            return 0;

        enum choices
        {
            DEPOSIT = 1,
            WITHDRAW = 2,
            TRANSFER = 3,
            KILLSWITCH = 4,
            PRINT = 5
        };
        int choice;
        while (true)
        {
            choice = read_integer("Please choose one option:");
            switch (choice)
            {
            case DEPOSIT:
                sa->deposit();
            case WITHDRAW:
                sa->withdraw();
            case TRANSFER:
                sa->transaction();
            case KILLSWITCH:
                sa->kill_switch();
            case PRINT:
                sa->print_account();
            default:
                break;
            }
        }
    }
}
