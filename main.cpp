#include "crow.h"
//#include "crow_all.h"
#include <ctime>
#include <iostream>
#include <unistd.h>

#define CLIENTIDLEN 20
#define APIKEYLEN 255

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

bool validate_credentials(std::string api_key_whole){
    /*
     * Input : API key
     * Separate client_id and API key suffix
     * Hash api key suffix
     * check against value for client_id stored in DB
     * Return True or False
     * 
    */
   std::string client_id = api_key_whole.substr(0, CLIENTIDLEN);
   std::string api_key = api_key_whole.substr(CLIENTIDLEN, APIKEYLEN);
}

int main()
{
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    CROW_ROUTE(app, "/getcredentials")([](){
        srand((unsigned)time(NULL) * getpid());     
        // std::cout << gen_random(12) << "\n";
        std::string client_id = gen_random(CLIENTIDLEN);
        std::string api_key = gen_random(APIKEYLEN);
        std::string api_key_whole = client_id + api_key;

        /*
         * TODO
         * 1) Store client_id into Database
         * 2) Get hash of api_key_suffix
         * 3) Store hash in Database
        */
        return  client_id + " " + api_key_whole;
    });

    CROW_ROUTE(app, "/invalidate")([](){
        return "Not yet implemented";
    });



    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
}

