#pragma once

//SQLite error
class CDBException : public std::runtime_error
{
public:
    explicit CDBException(const char* error_msg) : std::runtime_error(error_msg) {}
};

//sqlite3_stmt wrapper
class CSQLStatement
{
public:
    explicit CSQLStatement(sqlite3_stmt* handle = nullptr) : Handle{ handle } {}
    ~CSQLStatement() { sqlite3_finalize(Handle); }

    operator sqlite3_stmt* () { return Handle; }
    operator sqlite3_stmt** ();

private:
    sqlite3_stmt* Handle{ nullptr };
};

//scores db
class CScoresDB
{
public:
    static constexpr const char* BEST_SCORES_FILE_NAME = "bscore.db";

    CScoresDB();
    ~CScoresDB();

    //operations
    void Clear();
    void Save(const CBestScores& best_scores);
    void Load(CBestScores& best_scores);

private:
    sqlite3* Handle{ nullptr };

    //throws exception if db_result != SQLITE_OK
    void VerifyResult(int db_result);
};
