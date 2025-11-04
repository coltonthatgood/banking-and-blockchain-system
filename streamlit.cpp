#define _WIN32_WINNT 0x0A00
#define WINVER 0x0A00

// run g++ blockchain.cpp utilities.cpp streamlit.cpp -o frontend -l SplashKit -l ws2_32 -l sqlite3 -pthread
// Ensure winsock2.h is loaded first (before windows.h)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <sqlite3.h>
#undef ERROR
#undef HELP_KEY

#include "splashkit.h"
#include "httplib.h"
#include "blockchain.h"
#include "utilities.h"
#include "nlohmann/json.hpp"
#include "rsa.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <mutex>
#include <cstdint>
using nlohmann_json = nlohmann::json;
using namespace std;
using namespace rsa;

Blockchain B;
// --- Thread locks for concurrency safety ---
std::mutex blockchain_mutex;
std::mutex user_mutex;

int main()
{
    readChainFromDB(B);

    if (B.chain.empty())
    {
        std::cout << "⚠️ No blockchain data found in DB. Creating genesis block..." << std::endl;
        B.creategenesisBlock();
        saveChainToDB(B);
    }
    httplib::Server svr;

    svr.Post("/create_wallet", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            auto body = nlohmann_json::parse(req.body);
            string username = body["username"];
 
            Wallet user;
            user.username = username;
            user.privateKey = 0;
            user.publicKey = 0;
            user.modulus = 0;
            generate_keys(user);

            {
                std::lock_guard<std::mutex> lock(user_mutex);
                users.push_back(user);
            }

            nlohmann_json reply = {
                {"publicKey", to_string(user.publicKey)},
                {"privateKey", to_string(user.privateKey)}
            };

            res.set_header("Connection", "keep-alive");
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(reply.dump(), "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content(string("{\"error\":\"") + e.what() + "\"}", "application/json");
        } });

    // --- Mine block endpoint ---
    svr.Post("/mine", [](const httplib::Request &req, httplib::Response &res)
             {
    try {
        auto body = nlohmann_json::parse(req.body);

        uint64_t publicKey = 0;
        try {
            publicKey = body["publicKey"].get<uint64_t>();
        } catch (...) {
            publicKey = std::stoull(body["publicKey"].get<std::string>());
        }

        string prevHash = body["prevHash"];
        string currHash = body["currHash"];

        bool found = false;

        {
            std::lock_guard<std::mutex> lock_user(user_mutex);
            std::lock_guard<std::mutex> lock_block(blockchain_mutex);

            for (auto &u : users) {
                if (u.publicKey == publicKey) {
                    u.mine_block(B, publicKey, prevHash, currHash);
                    found = true;
                    break;
                }
            }
        }

        nlohmann_json reply;
        if (found) {
            reply = {{"status", "mined"}};
            cout << "Mined block added to chain, new length: " << B.chain.size() << endl;

        } else {
            reply = {{"status", "failed"}, {"reason", "User not found"}};
        }

        res.set_header("Connection", "keep-alive");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(reply.dump(), "application/json");
    } 
    catch (const std::exception &e) {
        res.status = 400;
        res.set_content(
            nlohmann_json({{"error", e.what()}}).dump(),
            "application/json"
        );
    } });

    svr.Post("/automine", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            auto body = nlohmann_json::parse(req.body);
            uint64_t publicKey = body["publicKey"];

            lock_guard<mutex> lock_user(user_mutex);
            lock_guard<mutex> lock_block(blockchain_mutex);

            string prevHash = B.getLatestBlock().previousHash;
            string currHash = B.getLatestBlock().Hash;
            
                
                bool userFound = false;
                for (auto &u : users) {
                    if (u.publicKey == publicKey) {
                     u.mine_block(B, publicKey, prevHash, currHash);
                    userFound = true;
                    break;
                }
            }
        

            if (!userFound) {
                res.status = 404;
                res.set_content("{\"error\":\"User not found\"}", "application/json");
                return;
            }

            nlohmann_json reply = {
                {"status", "auto mined"},
                {"prevHash", prevHash},
                {"currHash", currHash}
            };

            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(reply.dump(), "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content(string("{\"error\":\"") + e.what() + "\"}", "application/json");
        } });

    svr.Post("/transfer", [](const httplib::Request &req, httplib::Response &res)
             {
        try {
            auto body = nlohmann_json::parse(req.body);
            uint64_t senderPublicKey = body["senderPublicKey"];
            uint64_t receiverPublicKey = body["receiverPublicKey"];
            double Amount = body["Amount"];

            {
                std::lock_guard<std::mutex> lock_user(user_mutex);
                std::lock_guard<std::mutex> lock_block(blockchain_mutex);

                for (auto &u : users) {
                    if (u.publicKey == senderPublicKey) {
                        u.transfer(B, senderPublicKey, receiverPublicKey, Amount);
                        break;
                    }
                }
            }

            nlohmann_json reply = {{"status", "transferred"}};
            res.set_header("Connection", "keep-alive");
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(reply.dump(), "application/json");
        } catch (const std::exception &e) {
            res.status = 400;
            res.set_content(string("{\"error\":\"") + e.what() + "\"}", "application/json");
        } });

    // --- Get blockchain endpoint ---
    svr.Get("/chain", [](const httplib::Request &, httplib::Response &res)
            {
        nlohmann_json chain_json = nlohmann_json::array();

        {
            std::lock_guard<std::mutex> lock(blockchain_mutex);
            for (auto &block : B.chain) {
                chain_json.push_back({
                    {"index", block.index},
                    {"username", to_string(block.Username)},
                    {"amount", block.amount},
                    {"hash", block.Hash},
                    {"prevHash", block.previousHash}
                });
            }
        }

        res.set_header("Connection", "keep-alive");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(chain_json.dump(4), "application/json"); });

    // check balance
    svr.Get("/checkbalance", [](const httplib::Request &req, httplib::Response &res)
            {
    try {
        if (!req.has_param("publicKey")) {
            res.status = 400;
            res.set_content("{\"error\":\"Missing publicKey parameter\"}", "application/json");
            return;
        }

        string publicKey = req.get_param_value("publicKey");
        double balance = 0.0;

        {
            std::lock_guard<std::mutex> lock_user(user_mutex);
            std::lock_guard<std::mutex> lock_block(blockchain_mutex);

            for (auto &u : users) {
                if (to_string(u.publicKey) == publicKey) {
                    balance = u.getBalance(B);
                    break;
                }
            }
        }

        nlohmann_json reply = {
            {"publicKey", publicKey},
            {"balance", balance}
        };

        res.set_header("Connection", "keep-alive");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(reply.dump(4), "application/json");
    } 
    catch (const std::exception &e) {
        res.status = 400;
        res.set_content(
            nlohmann_json({{"error", e.what()}}).dump(),
            "application/json"
        );
    } });

    cout << "🚀 Blockchain server running at http://localhost:8080" << endl;

    // Start the HTTP server
    if (!svr.listen("0.0.0.0", 8080))
    {
        cerr << "❌ Failed to start server (port may be in use)" << endl;
        return -1;
    }

    return 0;
}