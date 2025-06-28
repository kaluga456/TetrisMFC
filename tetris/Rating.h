#pragma once

using CDate = __int64;
//////////////////////////////////////////////////////////////////////////////
//score record
class CScore
{
public:
	CScore() : Score{}, Date{} {}
	CScore(int score, CDate date) : Score(score), Date(date) {}

	bool operator>(const CScore& rop) const;

	int Score{};
	CDate Date{};
};
//////////////////////////////////////////////////////////////////////////////
class CRating
{
public:
	static constexpr size_t MAX_SCORES = 20;
	using const_iterator = std::vector<CScore>::const_iterator;

	CRating() { Scores.reserve(MAX_SCORES + 1); }

	const_iterator begin() const { return Scores.cbegin(); }
	const_iterator end() const { return Scores.cend(); }

	bool add(int result); //uses current date
	bool add(int result, const CDate& date);
	void clear() { Scores.clear(); }

private:
	std::vector<CScore> Scores;
};
//////////////////////////////////////////////////////////////////////////////
extern CRating Rating;