CREATE TABLE IF NOT EXISTS Users (
    UserID VARCHAR(16) NOT NULL,
    Creds CHAR(256) NOT NULL,
    PRIMARY KEY(UserID)
);

CREATE TABLE IF NOT EXISTS ExchangeKeys (
    UserID VARCHAR(16) NOT NULL,
    ExchangeID int NOT NULL,
    APIKey VARCHAR(256),
    LastUpdated DATE,
    PRIMARY KEY(UserID, ExchangeID),
    FOREIGN KEY(UserID) REFERENCES Users(UserID)
);

CREATE TABLE IF NOT EXISTS Trades (
    UserID VARCHAR(16) NOT NULL,
    TradeDate DATE NOT NULL,
    BoughtCurrency VARCHAR(16),
    SoldCurrency VARCHAR(16),
    BoughtAmount DECIMAL,
    SoldAmount DECIMAL,
    FOREIGN KEY(UserID) REFERENCES Users(UserID)
);