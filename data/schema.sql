CREATE TABLE IF NOT EXISTS Users (
    UserID VARCHAR(16) NOT NULL,
    Creds CHAR(256) NOT NULL,
    Refrs CHAR(256) NOT NULL,
    PRIMARY KEY(UserID)
);

CREATE TABLE IF NOT EXISTS ExchangeKeys (
    UserID VARCHAR(16) NOT NULL,
    ExchangeID int NOT NULL,
    PubKey VARCHAR(256),
    PvtKey VARCHAR(256), --@TODO: is this long enough @Aleks
    LastUpdatedYear int,
    LastUpdatedMonth int,
    LastUpdatedDay int,
    PRIMARY KEY(UserID, ExchangeID),
    FOREIGN KEY(UserID) REFERENCES Users(UserID)
);

CREATE TABLE IF NOT EXISTS Trades (
    UserID VARCHAR(16) NOT NULL,
    TradeYear int,
    TradeMonth int,
    TradeDay int,
    BoughtCurrency VARCHAR(16),
    SoldCurrency VARCHAR(16),
    BoughtAmount DECIMAL,
    SoldAmount DECIMAL,
    FOREIGN KEY(UserID) REFERENCES Users(UserID)
);
