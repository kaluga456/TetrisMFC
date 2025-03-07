#include "stdafx.h"
#pragma hdrstop
#include "statistics.h"

CStatisticRecord& CStatisticRecord::operator=(const CStatisticRecord& record)
{
	if(&record == this)
		return *this;
	PlayerName = record.PlayerName;
	Score = record.Score;
	Lines = record.Lines;
	Bonus = record.Bonus;
	Time = record.Time;
	Date = record.Date;
	return *this;
}
CStatistic::CStatistic()
{
	try
	{
		CFile file;
		file.Open(_T("stat.dat"), CFile::modeRead);
		if (file.m_hFile == CFile::hFileNull)	//can`t open file
			return;

		CArchive archive(&file, CArchive::load);
		for(size_t i = 0; i < BEST_PLAYERS_MAX_COUNT; ++i)
		{
			archive >> Data[i].PlayerName;
			archive >> Data[i].Score;
			archive >> Data[i].Lines;
			archive >> Data[i].Bonus;
			archive >> Data[i].Time;
			archive >> Data[i].Date;
		}
	}
	catch(CArchiveException* pex)
	{
		pex->ReportError();
		::AfxMessageBox(_T("Can`t open file. New file will be created."), MB_OK|MB_ICONEXCLAMATION);
	}
}
void CStatistic::AddRecord(const CStatisticRecord& record)
{
	if(record.Score <= GetWorstScore())
		return;
	int insert_index = -1;
	for(size_t i = 0; i < BEST_PLAYERS_MAX_COUNT; ++i)
	{
		if(record.Score > Data[i].Score)
		{
			insert_index = i;
			break;
		}	
	}
	if(insert_index != -1)	//redundant?
	{
		for(int i=BEST_PLAYERS_MAX_COUNT-2;i>insert_index;i--)
			Data[i+1] = Data[i];	
		Data[insert_index] = record;
	}
}
void CStatistic::GetRecord(int index, CStatisticRecord& record) const
{
	ASSERT(index>=0 && index<BEST_PLAYERS_MAX_COUNT);
	record = Data[index];
}
CStatistic::~CStatistic()
{
	try
	{
		CFile file;
		file.Open(_T("stat.dat"), CFile::modeCreate | CFile::modeWrite);
		if (file.m_hFile == CFile::hFileNull)	//can`t create/open file
		{
			::AfxMessageBox(_T("Can`t access statistics file"), MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		CArchive archive(&file, CArchive::store);
		for(size_t i = 0; i < BEST_PLAYERS_MAX_COUNT; ++i)
		{
			archive << Data[i].PlayerName;
			archive << Data[i].Score;
			archive << Data[i].Lines;
			archive << Data[i].Bonus;
			archive << Data[i].Time;
			archive << Data[i].Date;
		}
	}
	catch(CArchiveException* pex)
	{
		pex->ReportError();
		::AfxMessageBox(_T("Can`t write to statistics file"), MB_OK|MB_ICONEXCLAMATION);
	}
}