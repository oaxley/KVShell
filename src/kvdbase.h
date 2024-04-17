/*
 * @file    kvdbase.h
 * @brief   Header for the KVDbase class
 */

// ----- guards
#ifndef KVDBASE_H
#define KVDBASE_H

// ----- includes
#include <SQLiteCpp/SQLiteCpp.h>

#include <string>


// ----- class
class KVDbase
{
public:     //< public methods
    KVDbase(std::string dbname);
    ~KVDbase();

    SQLite::Database& get();

    // no copy
    KVDbase(const KVDbase&) = delete;
    KVDbase& operator=(const KVDbase&) = delete;

    // no move
    KVDbase(KVDbase&&) = delete;
    KVDbase& operator=(KVDbase&&) = delete;

private:    //< private methods
    void createTables();


private:
//< private members
    SQLite::Database* pSQLite_;
};

#endif // KVDBASE_H