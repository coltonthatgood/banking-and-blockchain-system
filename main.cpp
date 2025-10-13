#include "splashkit.h"
#include "utilities.h"
#include "blockchain.h"
#include <string>
#include <vector>
#include <cmath>

using std::string;
using std::to_string;
using std::vector;

int main()
{
    Blockchain B;
    B.creategenesisBlock();

    vector<Wallet> users;
    bool running = true;

    while (running)
    {
        write_line("\n===== USER MENU =====");
        write_line("1. Create new wallet");
        write_line("2. Select existing wallet");
        write_line("3. View blockchain");
        write_line("4. Exit");

        int uchoice = read_integer("Choice: ");

        if (uchoice == 1)
        {
            Wallet newUser;
            newUser.username = read_string("Enter new username: ");
            newUser.publicKey = "pub_" + newUser.username;
            newUser.Wallet::getPrivateKey(newUser.publicKey);
            write_line("Your public key:" + newUser.publicKey);
            write_line("Your private key:" + newUser.privateKey);
            users.push_back(newUser);
            write_line("Wallet created for " + newUser.username);
        }
        else if (uchoice == 2)
        {
            if (users.empty())
            {
                write_line("No users found! Create one first.");
                continue;
            }

            write_line("Existing users:");
            for (int i = 0; i < users.size(); i++)
                write_line(to_string(i) + ": " + users[i].username);

            int idx = read_integer("Select user index: ");
            if (idx < 0 || idx >= users.size())
            {
                write_line("Invalid user index.");
                continue;
            }

            string keyEntered = read_string("Please enter the private key that you have.");
            if (keyEntered != users[idx].privateKey)
            {
                write_line("Invalid log-in! Exiting");
                return 0;
            }

            Wallet &user = users[idx];

            bool walletRunning = true;
            while (walletRunning)
            {
                write_line("\n===== MENU for " + user.username + " =====");
                write_line("1. View Blockchain");
                write_line("2. Mine Block");
                write_line("3. Check Balance");
                write_line("4. Back to User Menu");

                int choice = read_integer("Enter your choice: ");

                switch (choice)
                {
                case 1:
                    B.displayChain();
                    break;
                case 2:
                    user.mine_block(B, user.publicKey, "", "");
                    break;
                case 3:
                    write_line("Balance: " + to_string(user.getBalance(B)));
                    break;
                case 4:
                    walletRunning = false;
                    break;
                default:
                    write_line("Invalid choice!");
                }
            }
        }
        else if (uchoice == 3)
        {
            B.displayChain();
        }
        else if (uchoice == 4)
        {
            running = false;
            write_line("Goodbye!");
        }
        else
        {
            write_line("Invalid choice!");
        }
    }

    return 0;
}