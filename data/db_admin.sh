#!/bin/bash

DB_PATH='/db'

if [ "$1" = "clear" ]; then
    if (sqlite3 $DB_PATH < clear.sql); then
        echo success
    else
        echo failure
    fi
elif [ "$1" = "create" ]; then
    if (sqlite3 $DB_PATH < schema.sql); then
        echo success
    else
        echo failure
    fi    
elif [ "$1" = "dump" ]; then
    echo "Users:"
    sqlite3 $DB_PATH "SELECT * FROM Users"
    echo "ExchangeKeys:"
    sqlite3 $DB_PATH "SELECT * FROM ExchangeKeys"
    echo "Trades:"
    sqlite3 $DB_PATH "SELECT * FROM Trades"
else
    echo "Command not recognized: $1"
    echo "Usage:
    ./db_admin.sh clear  -> clears database (not exists ok)
    ./db_admin.sh create -> creates tables (exists ok)
    ./db_admin.sh dump   -> dumps database to console"
    exit 1
fi
