#include "stdafx.h"
#pragma hdrstop
#include "Rating.h"

CRating Rating;
//////////////////////////////////////////////////////////////////////////////
//CScore
bool CScore::operator>(const CScore& rop) const
{
	//old scores comes first
	if (Score == rop.Score)
		return Date < rop.Date;
	return Score > rop.Score;
}
//////////////////////////////////////////////////////////////////////////////
//CRating
bool CRating::add(int result) //uses current date
{
	time_t current_time{};
	::time(&current_time);
	return add(result, static_cast<CDate>(current_time));
}
bool CRating::add(int result, const CDate& date)
{
	if (result <= 0)
		return false;

	CScore score{ result , date};
	for (auto i = Scores.cbegin(); i != Scores.cend(); ++i)
	{
		const CScore& s = *i;
		if (score > s)
		{
			Scores.insert(i, score);
			if (Scores.size() > MAX_SCORES)
				Scores.resize(MAX_SCORES);
			return true;
		}
	}
	if (Scores.size() < MAX_SCORES)
	{
		Scores.push_back(score);
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////////