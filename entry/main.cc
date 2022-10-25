#include <iostream>
#include "crow.h"
//#include "crow_all.h"
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <openssl/sha.h>
#include "auth.h"
//#include "data/data.h"

//#include "engine/matching.h"

int main() {
    //get_pnl_snapshots({ "me", "mine" });
    
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

    
    CROW_ROUTE(app, "/tryjson").methods("POST"_method)([&a](const crow::request& req){
        
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
    
    CROW_ROUTE(app, "/trade").methods("POST"_method)([&a](const crow::request& req){
        
        
        
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
    
    CROW_ROUTE(app, "/exchangekey").methods("POST"_method)([&a](const crow::request& req){
        
        
        
        if (a.validate_credentials(req)){
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            std::string exchange = a.convert_to_string(x["exchange"]);
            std::string readkey = a.convert_to_string(x["readkey"]);
            std::string secretkey = a.convert_to_string(x["secretkey"]);
            
            std::cout << exchange << std::endl;
            std::cout << readkey << std::endl;
            
            // TODO : store client_id, exchange and readkey, and secretkey into db
		
            return crow::response(200);
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/removekey").methods("POST"_method)([&a](const crow::request& req){
        
        
        
        if (a.validate_credentials(req)){
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            std::string exchange = a.convert_to_string(x["exchange"]);
            std::string readkey = a.convert_to_string(x["readkey"]);
            std::string secretkey = a.convert_to_string(x["secretkey"]);
            
            std::cout << exchange << std::endl;
            std::cout << readkey << std::endl;
            
            // TODO : delete exchange and readkey for clientid from db
            
            return crow::response(200);;
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/get_annotated_trades")([&a](const crow::request& req){
        
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
    
    CROW_ROUTE(app, "/year_end_stats")([&a](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
		
            std::string client_id = a.get_client_id(req);
            /*
            * TODO :
            * get all trades for client from db
            * 
            * Code below this should be working and should be uncommented when db gets done
            * 
            * YearEndPNL y_pnl;
            * y_pnl = get_year_end_pnl(trades);
            * 
            * crow::json::wvalue stats;
            * stats["lt_realized_pnl"] = y_pnl.lt_realized.str();
            * stats["st_realized_pnl"] = y_pnl.st_realized.str();
            * stats["actual_pnl"] = y_pnl.actual_pnl.str();
            * return stats;
            */
            
            return crow::response(200);
		
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/trade_pnl").methods("POST"_method)([&a](const crow::request& req){
        
        if (a.validate_credentials(req)){
		
		
            auto x = crow::json::load(req.body);
            if (!x)
                return crow::response(400);
            
            std::string client_id = a.get_client_id(req);
            //struct trade t = a.parse_trade_from_json(req);
            
            /*
            * Below code should be uncommented when parse_trade_from_json is implemented
            * 
            * PNL pnl = get_trade_pnl(trade);
            * std::string pnl = pnl.str();
            * 
            * return crow::response(pnl);
            */
            
            return crow::response("2000.5");
		
        }
        else
            return crow::response(401);
    });
    
    CROW_ROUTE(app, "/portfolio_pnl")([&a](const crow::request& req){
        
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