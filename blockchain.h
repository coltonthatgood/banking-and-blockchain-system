#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "splashkit.h"
#include "utilities.h"
#include <string>
#include <cstdint>
#include <vector>

using std::string;
using std::vector;

//block struct
struct Block
{
    int index;
    uint64_t Username;
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
    void addBlock(uint64_t publickey, int index, double amount, string prevHash);
    void displayChain() const;
};

//wallet struct
struct Wallet
{
    string username;
    uint64_t publicKey;
    uint64_t privateKey;
    uint64_t modulus;

    double getBalance(Blockchain &B);
    void mine_block(Blockchain &B, uint64_t publicKey, string prevHash, string currHash);
    void transfer(Blockchain &B, uint64_t senderPublickey, uint64_t receiverPublickey, double amount);
};


inline vector<Wallet> users;

string getnewHash(const string &prevHash, const uint64_t &publickey, int index, double amount);
void saveChainToDB(const Blockchain &B);
void readChainFromDB(Blockchain &B);


#endif
