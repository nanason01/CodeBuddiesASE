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

    
    CROW_ROUTE(app, "/tryjson").methods("POST"_method)([](const crow::request& req){
        
        auto x = crow::json::load(req.body);
        if (!x)
            return crow::response(400);
        
        std::cout << x["a"] << std::endl;
        std::cout << x["b"] << std::endl;
        
        if (a.validate_credentials(req))
            return crow::response(200);
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/trade").methods("POST"_method)([](const crow::request& req){
        
        
        
        if (a.validate_credentials(req)){
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            //struct trade t = a.parse_trade_from_json(req);
            
            // TODO : store trade t in db
            
            return crow::response(200);
        }
	    else
		    return crow::response(401);
    });
    
    CROW_ROUTE(app, "/exchangekey").methods("POST"_method)([](const crow::request& req){
        
        
        
        if (a.validate_credentials(req)){
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            std::string exchange = a.convert_to_string(x["exchange"]);
            std::string readkey = a.convert_to_string(x["readkey"]);
            
            std::cout << exchange << std::endl;
            std::cout << readkey << std::endl;
            
            // TODO : store client_id, exchange and readkey into db
		
            return crow::response(200);
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/removekey").methods("POST"_method)([](const crow::request& req){
        
        
        
        if (a.validate_credentials(req)){
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            std::string exchange = a.convert_to_string(x["exchange"]);
            std::string readkey = a.convert_to_string(x["readkey"]);
            
            std::cout << exchange << std::endl;
            std::cout << readkey << std::endl;
            
            // TODO : delete exchange and readkey for clientid from db
            
            return crow::response(200);;
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/get_annotated_trades")([](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
            std::string client_id = a.get_client_id(req);
            /*
            * TODO :
            * get trades for client from db
            * all_matched_trades = get_matched_trades(trades)
            * format return value into response
            */
            return crow::response(200);
		
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/year_end_stats")([](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
		
            std::string client_id = a.get_client_id(req);
            /*
            * TODO :
            * get all trades for client from db
            * YearEndPNL y_pnl;
            * y_pnl = get_year_end_pnl(trades);
            * format return value into response
            */
            
            return crow::response(200);
		
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/trade_pnl").methods("POST"_method)([](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            //struct trade t = parse_trade_from_json(req);
            
            /*
            * TODO :
            * 
            * PNL pnl = get_trade_pnl(trade);
            * pnl is a double
            * std::string pnl = pnl.str();
            */
            
            return crow::response("2000.5");
		
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/portfolio_pnl")([](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
		
            std::string client_id = a.get_client_id(req);

            /*
            * TODO :
            * get required trades for client from db
            * get_pnl_snapshots(trade);
            * format return value into response
            */
            
            return crow::response(200);
		
        }
        else
            return crow::response(401);
    });

    



    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();


    std::cout << "Hello world\n";
}