#include "blockchain.h"
#include "utilities.h"
#include "splashkit.h"
#include "rsa.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <sqlite3.h>

using std::rand;
using std::string;
using std::to_string;
using std::vector;

using namespace std;

void saveChainToDB(const Blockchain &B)
{
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open("blockchain.db", &db);

    if (rc)
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Create table if not exists
    string sql = "CREATE TABLE IF NOT EXISTS BLOCKS ("
                 "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "index_num INTEGER, "
                 "publicKey INTEGER, "
                 "amount REAL, "
                 "previousHash TEXT, "
                 "currentHash TEXT);";
    rc = sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error creating table: " << errMsg << endl;
        sqlite3_free(errMsg);
    }

    // Optional: clear old entries if rewriting
    sql = "DELETE FROM BLOCKS;";
    sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);

    // Insert blocks
    for (const auto &block : B.chain)
    {
        string insert = "INSERT INTO BLOCKS (index_num, publicKey, amount, previousHash, currentHash) VALUES (" + to_string(block.index) + ", " + to_string(block.Username) + ", " + to_string(block.amount) + ", '" + block.previousHash + "', '" + block.Hash + "');";

        rc = sqlite3_exec(db, insert.c_str(), 0, 0, &errMsg);
        if (rc != SQLITE_OK)
        {
            cerr << "SQL insert error: " << errMsg << endl;
            sqlite3_free(errMsg);
        }
    }

    sqlite3_close(db);
    cout << "✅ Blockchain successfully saved to blockchain.db (" << B.chain.size() << " blocks)" << endl;
}

void readChainFromDB(Blockchain &B)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    int rc = sqlite3_open("blockchain.db", &db);
    if (rc)
    {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string sql = "SELECT index_num, username, amount, previousHash, currentHash FROM BLOCKS ORDER BY index_num;";

    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error (prepare): " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    B.chain.clear();

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Block block;
        block.index = sqlite3_column_int(stmt, 0);
        block.Username = reinterpret_cast<uint64_t>(sqlite3_column_text(stmt, 1));
        block.amount = sqlite3_column_double(stmt, 2);
        block.previousHash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        block.Hash = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));

        B.chain.push_back(block);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    cout << "Blockchain successfully loaded from blockchain.db" << endl;
}

void Blockchain::creategenesisBlock()
{
    Block genesis;
    genesis.index = 0;
    genesis.Username = 0;
    genesis.amount = 0;
    genesis.previousHash = "k0";
    genesis.Hash = "k1";
    chain.push_back(genesis);
}

Block Blockchain::getLatestBlock() const
{
    return chain.back();
}

void Blockchain::addBlock(uint64_t publickey, int index, double amount, string prevHash)
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
        write_line("Username: " + to_string(block.Username));
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

void Wallet::mine_block(Blockchain &B, uint64_t publicKey, string prevHash, string currHash)
{
    for (int i = 0; i < B.chain.size(); i++)
    {
        if (prevHash == B.chain[i].previousHash && currHash == B.chain[i].Hash)
        {
            B.currIndex = B.chain.size();
            B.addBlock(publicKey, B.currIndex, 0.05, currHash);
            write_line("Mined block successfully!");
            saveChainToDB(B);
            return;
        }
    }

    write_line("Block not found!");
}

void Wallet::transfer(Blockchain &B, uint64_t senderPublickey, uint64_t receiverPublickey, double amount)
{
    bool UserFound = false;
    for (int i = 0; i < users.size(); i++)
    {
        if (receiverPublickey == users[i].publicKey)
        {
            B.addBlock(senderPublickey, B.currIndex, -amount, B.chain[B.currIndex].Hash);
            B.currIndex++;
            B.addBlock(receiverPublickey, B.currIndex, amount, B.chain[B.currIndex].Hash);
            B.currIndex++;
            write_line("Successfully transferred!");
            UserFound = true;
            saveChainToDB(B);
            return;
        }
    }
    if (!UserFound)
    {
        write_line("No user found. Please try again.");
    }
}

string getnewHash(const string &prevHash, const uint64_t &publickey, int index, double amount)
{
    const int mod = 100000000; // modulus to keep hash manageable
    string newHash;
    int base = 211;

    long long hashVal = 0;

    for (char c : prevHash)
    {
        for (char k : to_string(publickey))
        {
            hashVal += (static_cast<int>(pow(c, 2)) + static_cast<int>(pow(k, 2)) + index * base + static_cast<int>(amount * 100)) % mod;
            hashVal %= mod; // prevent overflow
        }
    }

    newHash = to_string(hashVal);
    return newHash;
}

