/*
 * @file    kvdbase.h
 * @brief   Source for the KVDbase class
 */

// ----- includes
#include "kvdbase.h"

#include <filesystem>
#include <iostream>

// ----- class

// constructor
KVDbase::KVDbase(std::string dbname) :
    pSQLite_{nullptr}
{
    // check if the database already exists
    if (std::filesystem::exists(std::filesystem::path{dbname})) {
        // open the DB without creating it
        try {
            std::cout << "Using database [" << dbname << "]\n";
            pSQLite_ = new SQLite::Database(dbname, SQLite::OPEN_READWRITE);
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            std::exit(EXIT_FAILURE);
        }
    }
    else {
        // open the DB and create it
        try {
            std::cout << "Creating database [" << dbname << "]\n";
            pSQLite_ = new SQLite::Database(dbname, SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);
            createTables();
        } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

// destructor
KVDbase::~KVDbase()
{
    if (pSQLite_) {
        delete pSQLite_;
        pSQLite_ = nullptr;
    }
}

// return a reference to the underlying SQLite handle
SQLite::Database& KVDbase::get()
{
    return *pSQLite_;
}

// create the initial tables
void KVDbase::createTables()
{
    // ensure we have an object before moving forward
    if (!pSQLite_)
        return;

    try {
        pSQLite_->exec("DROP TABLE IF EXISTS KVEntry");
        pSQLite_->exec("CREATE TABLE KVEntry ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "user INTEGER NOT NULL,"
            "key BLOB NOT NULL,"
            "value BLOB NOT NULL,"
            "expiry INTEGER,"
            "timestamp INTEGER"
            ")"
            );
    } catch (std::exception& e) {
        std::cerr << "Error: unable to create the table in the database\n";
        std::cerr << e.what() << "\n";
        std::exit(EXIT_FAILURE);
    }
}

// retrieve a single row from the database
DBResult* KVDbase::fetchRow(std::uint8_t* key, int size,  int uid)
{
    try
    {
        // prepare the query
        SQLite::Statement query(*pSQLite_, "SELECT value FROM KVEntry WHERE user = :uid AND key = :key");
        query.bind(":uid", uid);
        query.bind(":key", key, size);

        // execute the query
        bool result = query.executeStep();
        if (!result) {
            return nullptr;
        }

        SQLite::Column blob = query.getColumn(0);
        int size = blob.getBytes();

        // create a new result structure
        DBResult* db_result = new DBResult{
            size: size,
            pData: new std::uint8_t[size]
        };

        // copy the blob
        memcpy(db_result->pData, blob.getBlob(), size);

        return db_result;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return nullptr;
}

// add a key/value in the database
int KVDbase::insert(std::uint8_t* key, int ksize, std::uint8_t* value, int vsize, int uid)
{
    int rows{0};

    try
    {
        // check if the row does not exist already
        SQLite::Statement squery(*pSQLite_, "SELECT * FROM KVEntry WHERE user = :uid AND key = :key");
        squery.bind(":uid", uid);
        squery.bind(":key", key, ksize);

        bool result = squery.executeStep();
        if (!result)
        {
            // the record does not exist, create one
            SQLite::Statement iquery(*pSQLite_, "INSERT INTO KVEntry (user, key, value) VALUES (:uid, :key, :value)");

            iquery.bind(":uid", uid);
            iquery.bind(":key", key, ksize);
            iquery.bind(":value", value, vsize);

            rows = iquery.exec();
        }
        else
        {
            // update the current record
            SQLite::Statement uquery(*pSQLite_, "UPDATE KVEntry SET value = :value WHERE user = :uid AND key = :key");

            uquery.bind(":uid", uid);
            uquery.bind(":key", key, ksize);
            uquery.bind(":value", value, vsize);

            rows = uquery.exec();
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return rows;
}

// check if a key exists in the database
bool KVDbase::exists(std::uint8_t* key, int ksize, int uid)
{
    try
    {
        // check if the row does not exist already
        SQLite::Statement squery(*pSQLite_, "SELECT * FROM KVEntry WHERE user = :uid AND key = :key");
        squery.bind(":uid", uid);
        squery.bind(":key", key, ksize);

        return squery.executeStep();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return false;
}


bool KVDbase::remove(std::uint8_t* key, int ksize, int uid)
{
    int rows{0};

    try
    {
        // check if the row does not exist already
        SQLite::Statement query(*pSQLite_, "DELETE FROM KVEntry WHERE user = :uid AND key = :key");
        query.bind(":uid", uid);
        query.bind(":key", key, ksize);

        rows = query.exec();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return (rows != 0);
}
