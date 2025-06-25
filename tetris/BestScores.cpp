#include "stdafx.h"
#pragma hdrstop
#include "BestScores.h"

CBestScores BestScores;

bool CBestScores::CBestScores::add(int result)
{
	time_t current_time{};
	::time(&current_time);
	return add(result, static_cast<CDate>(current_time));
}
bool CBestScores::CBestScores::add(int result, const CDate& date)
{
	//too bad score
	if (result <= 0)
		return false;

	//add score
	if (Results.size() < MAX_RESULTS)
	{
		Results[result] = date;
		return true;
	}

	//replace score
	std::map<int, CDate>::reverse_iterator i = Results.rbegin();
	if (i->first < result)
	{
		Results[result] = date;
		return true;
	}

	return false;
}