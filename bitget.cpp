#include <iostream>
#include <curl/curl.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <openssl/buffer.h>

using namespace std;

string base64_encode(const unsigned char* input, int length) {
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    string result(bptr->data, bptr->length - 1);
    BIO_free_all(b64);

    return result;
}

string generate_signature(const string& timestamp, const string& method,
                           const string& request_path, const string& body,
                           const string& secret_key) {
    string message = timestamp + method + request_path + body;
    unsigned char* digest = HMAC(EVP_sha256(), secret_key.c_str(), 
                                 (int)secret_key.length(), (unsigned char*)message.c_str(), message.length(), NULL, NULL);
    return base64_encode(digest, 32);
}

void make_request(const string& method, const string& request_path, const string& body = "") {
    string api_key = "*****************"; // Replace with your actual API key
    string secret_key = "****************"; // Replace with your actual secret key
    string passphrase = "****************"; // Replace with your actual passphrase

    time_t t = time(nullptr);
    string timestamp = to_string(t * 1000);
    string signature = generate_signature(timestamp, method, request_path, body, secret_key);

    CURL* curl = curl_easy_init();
    if (curl) {
        string url = "https://api.bitget.com" + request_path;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("ACCESS-KEY: " + api_key).c_str());
        headers = curl_slist_append(headers, ("ACCESS-SIGN: " + signature).c_str());
        headers = curl_slist_append(headers, ("ACCESS-TIMESTAMP: " + timestamp).c_str());
        headers = curl_slist_append(headers, ("ACCESS-PASSPHRASE: " + passphrase).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");

        if (method == "POST") {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
}

int main() {
    int flag=1;
    while(flag)
    {
        int choice;
        string coin, chain, address, amount, startTime, endTime;

        cout << "Select endpoint: " << endl;
        cout << "1) Account Asset for all coins" << endl;
        cout << "2) Account Assets" << endl;
        cout << "3) Deposit Address" << endl;
        cout << "4) Withdrawal List" << endl;
        cout << "5) Deposit List" << endl;
        cout << "6) Withdraw Funds" << endl;
        cout << "7) Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        string request_path, body;
        switch (choice) {
            case 1:
                request_path = "/api/spot/v1/account/assets";
                make_request("GET", request_path);
                break;
            case 2:
                cout << "Enter coin: ";
                cin >> coin;
                request_path = "/api/spot/v1/account/assets?coin=" + coin;
                make_request("GET", request_path);
                break;
            case 3:
                cout << "Enter coin: ";
                cin >> coin;
                cout << "Enter chain: ";
                cin >> chain;
                request_path = "/api/spot/v1/wallet/deposit-address?coin=" + coin + "&chain=" + chain;
                make_request("GET", request_path);
                break;
            case 4:
                cout << "Enter coin: ";
                cin >> coin;
                cout << "Enter startTime: ";
                cin >> startTime;
                cout << "Enter endTime: ";
                cin >> endTime;
                request_path = "/api/spot/v1/wallet/withdrawal-list?coin=" + coin + "&startTime=" + startTime + "&endTime=" + endTime;
                make_request("GET", request_path);
                break;
            case 5:
                cout << "Enter coin: ";
                cin >> coin;    
                cout << "Enter startTime: ";
                cin >> startTime;
                cout << "Enter endTime: ";
                cin >> endTime;
                request_path = "/api/spot/v1/wallet/deposit-list?coin=" + coin + "&startTime=" + startTime + "&endTime=" + endTime;
                make_request("GET", request_path);
                break;
            case 6:
                cout << "Enter coin: ";
                cin >> coin;
                cout << "Enter address: ";
                cin >> address;
                cout << "Enter chain: ";
                cin >> chain;
                cout << "Enter amount: ";
                cin >> amount;

                request_path = "/api/spot/v1/wallet/withdrawal";
                body = "{\"coin\":\"" + coin + "\",\"address\":\"" + address + "\",\"chain\":\"" + chain + "\",\"amount\":\"" + amount + "\"}";
                
                make_request("POST", request_path, body);
                break;
            case 7:
                flag=0;
                break;
            default:
                cout << "Invalid choice" << endl;
                return 1;
        }
    }

    return 0;
}
