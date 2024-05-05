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


// ----- structures
struct DBResult
{
    int size;
    std::uint8_t* pData;

    ~DBResult() {
        delete [] pData;
    }
};

// ----- class
class KVDbase
{
public:     //< public methods
    KVDbase(std::string dbname);
    ~KVDbase();

    SQLite::Database& get();

    // operations
    DBResult* fetchRow(std::uint8_t* key, int size, int uid);
    int insert(std::uint8_t* key, int ksize, std::uint8_t* value, int vsize, int uid);


    // no copy
    KVDbase(const KVDbase&) = delete;
    KVDbase& operator=(const KVDbase&) = delete;

    // no move
    KVDbase(KVDbase&&) = delete;
    KVDbase& operator=(KVDbase&&) = delete;

private:    //< private methods
    void createTables();


private:    //< private members
    SQLite::Database* pSQLite_;
};

#endif // KVDBASE_H
