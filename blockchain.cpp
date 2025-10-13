#include "blockchain.h"
#include "utilities.h"
#include "splashkit.h"
#include <string>
#include <vector>
#include <cmath>

using std::string;
using std::to_string;
using std::vector;

void Blockchain::creategenesisBlock()
{
    Block genesis;
    genesis.index = 0;
    genesis.Username = "";
    genesis.amount = 0;
    genesis.previousHash = "k0";
    genesis.Hash = "k1";
    chain.push_back(genesis);
}

Block Blockchain::getLatestBlock() const
{
    return chain.back();
}

void Blockchain::addBlock(string publickey, int index, double amount, string prevHash)
{
    Block newBlock;
    newBlock.index = index;
    newBlock.Username = publickey;
    newBlock.amount = amount;
    newBlock.previousHash = prevHash;
    newBlock.Hash = getnewHash(prevHash, publickey, index, amount);
    chain.push_back(newBlock);
}

void Blockchain::displayChain() const
{
    write_line("----- BLOCKCHAIN -----");
    for (auto &block : chain)
    {
        write_line("Index: " + to_string(block.index));
        write_line("Username: " + block.Username);
        write_line("Amount: " + to_string(block.amount));
        write_line("PrevHash: " + block.previousHash);
        write_line("Hash: " + block.Hash);
        write_line("-----------------------");
    }
}

double Wallet::getBalance(Blockchain &B)
{
    double balance = 0;
    for (auto &block : B.chain)
    {
        if (block.Username == this->publicKey)
            balance += block.amount;
    }
    return balance;
}

void Wallet::mine_block(Blockchain &B, string publicKey, string prevHash, string currHash)
{
    prevHash = read_string("Enter previous hash: ");
    currHash = read_string("Enter current hash: ");

    for (int i = 0; i < B.chain.size(); i++)
    {
        if (prevHash == B.chain[i].previousHash && currHash == B.chain[i].Hash)
        {
            B.currIndex = B.chain.size();
            B.addBlock(publicKey, B.currIndex, 0.05, currHash);
            write_line("Mined block successfully!");
            return;
        }
    }

    write_line("Block not found!");
}

void Wallet::getPrivateKey(string publicKey)
{
    const int mod = 68472;
    const int base = 257;
    for (char c : publicKey)
    {
        this->privateKey += to_string((static_cast<int>(pow(c, 3)) * base) % mod);
    }
}

string getnewHash(const string &prevHash, const string &publickey, int index, double amount)
{
    const int mod = 100000000; // modulus to keep hash manageable
    string newHash;
    int base = 211;

    long long hashVal = 0;

    for (char c : prevHash)
    {
        for (char k : publickey)
        {
            hashVal += (static_cast<int>(pow(c, 2)) + static_cast<int>(pow(k, 2)) + index * base + static_cast<int>(amount * 100)) % mod;
            hashVal %= mod; // prevent overflow
        }
    }

    newHash = to_string(hashVal);
    return newHash;
}
