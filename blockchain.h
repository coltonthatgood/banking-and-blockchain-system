#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "splashkit.h"
#include "utilities.h"
#include <string>
#include <vector>

using std::string;
using std::vector;

//block struct
struct Block
{
    int index;
    string Username;
    double amount;
    string previousHash;
    string Hash;
};

//blockchain struct
struct Blockchain
{
    int currIndex = 0;
    vector<Block> chain;

    void creategenesisBlock();
    Block getLatestBlock() const;
    void addBlock(string username, int index, double amount, string prevHash);
    void displayChain() const;
};

//wallet struct
struct Wallet
{
    string username;
    string publicKey;
    string privateKey;

    double getBalance(Blockchain &B);
    void mine_block(Blockchain &B, string username, string prevHash, string currHash);
    void getPrivateKey(string publicKey);
};

string getnewHash(const string &prevHash, const string &publickey, int index, double amount);

#endif
