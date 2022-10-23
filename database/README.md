# calldb createTable
For calling this header file connector classyou can set up a calldb class, use createTable function to create a sqlite database with two variables 'client_identifier' and 'hash_of_api'. 'client_identifier' is the PRIMARY KEY.

# calldb insertDB
You can use this function to insert items into the database, such as:
'''
sql.insertDB(xxx, xxx);
'''

# calldb deleteDB
You can use this function to delete items from the database by deleting the primaryb key "client_identifier", such as:
'''
sql.deleteDB(xxx);
'''

# Example

Here goes an example code
'''
'#include "calldb.h"
int main() {
	//This is an example of how to create a calldb class in order to create a table, insert, delete, and showTable, and close.
    calldb sql;
    sql.createTable();
    sql.insertDB("client1", "12d");
    sql.insertDB("client2", "dfds");
    sql.insertDB("client3", "rrwer");
    sql.showTable();
    sql.deleteDB("client2");
    sql.showTable();
    sql.closeDB();
    
    return 0;
}
'''