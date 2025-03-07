#pragma once

constexpr UINT BEST_PLAYERS_MAX_COUNT = 10;

struct CStatisticRecord
{
	CString PlayerName;
	int Score{};
	int Lines{};
	int Bonus{};
	CTimeSpan Time;
	CTime Date;
	CStatisticRecord& operator=(const CStatisticRecord& record);
};

struct CStatistic
{
public:
	CStatistic();
	~CStatistic();

	int GetWorstScore() const {return Data[BEST_PLAYERS_MAX_COUNT - 1].Score;}
	void AddRecord(const CStatisticRecord& record);
	void GetRecord(int index, CStatisticRecord& record) const;

private:
	int RecordsCount{};
	int WorstScore{};
	CStatisticRecord Data[BEST_PLAYERS_MAX_COUNT];
};