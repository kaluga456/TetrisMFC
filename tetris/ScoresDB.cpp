#include "stdafx.h"
#pragma hdrstop
#include "Rating.h"
#include "ScoresDB.h"
//////////////////////////////////////////////////////////////////////////////
//CSQLStatement
CSQLStatement::operator sqlite3_stmt** ()
{
    if (Handle)
        throw CDBException("Unexpected usage of CSQLStatement");
    return &Handle;
}
//////////////////////////////////////////////////////////////////////////////
//CSQLite
CSQLite::operator sqlite3** ()
{
    if (Handle)
        throw CDBException("Unexpected usage of CSQLite");
    return &Handle;
}
//////////////////////////////////////////////////////////////////////////////
//CScoresDB
CScoresDB::CScoresDB()
{
    //init db
    VerifyResult(sqlite3_open(BEST_SCORES_FILE_NAME, SQLite));

    //create table
    const char* query_text = "CREATE TABLE IF NOT EXISTS BEST_SCORES ("
        "RANK INTEGER PRIMARY KEY AUTOINCREMENT,"
        "SCORE INTEGER NOT NULL,"
        "DATE INTEGER NOT NULL);";
    VerifyResult(sqlite3_exec(SQLite, query_text, nullptr, 0, nullptr));
}
void CScoresDB::VerifyResult(int db_result)
{
    if (SQLITE_OK == db_result)
        return;
    throw CDBException(static_cast<sqlite3*>(SQLite) ? sqlite3_errmsg(SQLite) : "Unknown sqlite3 error");
}
void CScoresDB::Clear()
{
    const char* query_text = "DELETE FROM BEST_SCORES";
    VerifyResult(sqlite3_exec(SQLite, query_text, nullptr, 0, nullptr));
}
void CScoresDB::Save(const CRating& rating)
{
    int rank = 1;
    const char* query_text = "INSERT OR REPLACE INTO BEST_SCORES (RANK, SCORE, DATE) VALUES (?, ?, ?);";
    for (const auto& i : rating)
    {
        //prepare
        CSQLStatement stmt;
        VerifyResult(sqlite3_prepare_v2(SQLite, query_text, -1, stmt, nullptr));

        //bind
        VerifyResult(sqlite3_bind_int(stmt, 1, rank));
        VerifyResult(sqlite3_bind_int(stmt, 2, i.Score));
        VerifyResult(sqlite3_bind_int64(stmt, 3, i.Date));

        //exec
        const int db_result = sqlite3_step(stmt);
        if (SQLITE_DONE != db_result)
            VerifyResult(db_result);

        ++rank;
        if (rank == CRating::MAX_SCORES + 1)
            break;
    }
}
void CScoresDB::Load(CRating& rating)
{
    //prepare
    CSQLStatement stmt;
    const char* query_text = "SELECT RANK, SCORE, DATE FROM BEST_SCORES;";
    VerifyResult(sqlite3_prepare_v2(SQLite, query_text, -1, stmt, nullptr));

    //exec
    for (int count = 0; count < CRating::MAX_SCORES; ++count)
    {
        const int db_result = sqlite3_step(stmt);
        if (SQLITE_DONE == db_result)
            break;

        //unexpected result
        if (SQLITE_ROW != db_result)
        {
            VerifyResult(db_result);
            break;
        }

        //results
        const int rank = sqlite3_column_int(stmt, 0); //not used here
        const int score = sqlite3_column_int(stmt, 1);
        const CDate date = sqlite3_column_int64(stmt, 2);

        rating.add(score, date);
    }
}
//////////////////////////////////////////////////////////////////////////////
