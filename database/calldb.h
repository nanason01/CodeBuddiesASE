#ifndef CALLDB_H
#define CALLDB_H

#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

class calldb {

	private:
		// Pointer to SQLite connection
    	sqlite3 *db;

    	// Save any error messages
    	char *zErrMsg;

    	// Save the result of opening the file
    	int rc;

    	// Saved SQL
    	char* sql;

    	// Compiled SQLite Statement
    	sqlite3_stmt *stmt;

    	// Create a callback function  
		static int callback(void *NotUsed, int argc, char **argv, char **azColName) {

		    // int argc: holds the number of results
		    // (array) azColName: holds each column returned
		    // (array) argv: holds each value

		    for(int i = 0; i < argc; i++) {
		        
		        // Show column name, value, and newline
		        cout << azColName[i] << ": " << argv[i] << endl;
		    
		    }

		    // Insert a newline
		    cout << endl;

		    // Return successful
		    return 0;
		}


	public:

		calldb() {
			// Save the result of opening the crypto.db
    		rc = sqlite3_open("crypto.db", &db);

    		    if (rc){
		fprintf(stderr, "There goes an error: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		}else{
		fprintf(stderr, "Opened database successfully\n");
		}

    
    		
		}

		void createTable() {

		    // Save SQL to create a table
		    sql = "CREATE TABLE crypto_db (client_identifier TEXT PRIMARY KEY NOT NULL, hash_of_api TEXT NOT NULL);";
		    
		    // Run the SQL
		    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		}

		void insertDB(char* client, char* api) {

			char *query = NULL;

			// Build a string using asprintf()
			asprintf(&query, "INSERT INTO crypto_db ('client_identifier', 'hash_of_api') VALUES  ('%s', '%s');", client, api);   

			// Prepare the query
		    sqlite3_prepare(db, query, strlen(query), &stmt, NULL);

		    // Test it
			rc = sqlite3_step(stmt);

			// Finialize the usage
			sqlite3_finalize(stmt);

			// Free up the query space
			free(query);     

		}

		void showTable() {

			// Save SQL insert data
    		sql = "SELECT * FROM 'crypto_db';";
    
   			// Run the SQL
    		rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

		}

		void deleteDB(char* client) {

			char *query = NULL;

			// Build a string using asprintf()
			asprintf(&query, "DELETE FROM 'crypto_db' WHERE client_identifier = '%s';", client);   

			// Prepare the query
		    sqlite3_prepare(db, query, strlen(query), &stmt, NULL);

		    // Test it
			rc = sqlite3_step(stmt);

			// Finialize the usage
			sqlite3_finalize(stmt);

			// Free up the query space
			free(query);     

		}

		void closeDB() {

			// Close the SQL connection
    		sqlite3_close(db);

		}
	


};
#endif