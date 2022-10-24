#include <iostream>
#include "crow.h"
//#include "crow_all.h"
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <openssl/sha.h>
#include "auth.h"

#include "engine/matching.h"

int main() {
    get_pnl_snapshots({ "me", "mine" });
    
    crow::SimpleApp app; //define your crow application
    auth a;

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([&a](const crow::request& req){
        
        if (a.validate_credentials(req))
            return crow::response(200);
        else
            return crow::response(401);
    });

    CROW_ROUTE(app, "/getcredentials")([&a](){
        return a.generate_credentials();
    });

    CROW_ROUTE(app, "/refreshcredentials")([&a](const crow::request& req){
        
        if (a.validate_refresh_token(req)){
		    return crow::response(a.refresh_credentials(req));
        }
        else{
        	return crow::response(401);
        }
        
    });



    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();


    std::cout << "Hello world\n";
}