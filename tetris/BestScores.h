#pragma once

using CDate = __int64;

class CBestScores
{
public:
	static constexpr size_t MAX_RESULTS = 20;

	//using reverse iteration here, so best score comes first
	using const_iterator = std::map<int, CDate>::const_reverse_iterator;
	const_iterator begin() const { return Results.crbegin(); }
	const_iterator end() const { return Results.crend(); }

	bool add(int result); //uses current date
	bool add(int result, const CDate& date);
	void clear() { Results.clear(); }

private:
	std::map<int, CDate> Results;
};

extern CBestScores BestScores;