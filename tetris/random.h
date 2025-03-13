#pragma once

namespace app
{
	void random_seed(std::default_random_engine& dre);

	template <typename T>
	class random_value
	{
	public:
		explicit random_value(const T& min_val = std::numeric_limits<T>::min(), const T& max_val = std::numeric_limits<T>::max()) : Uid(min_val, max_val)
		{
			random_seed(Dre);
		}

		T get() { return Uid(Dre); }

	private:
		std::default_random_engine Dre;
		std::uniform_int_distribution<T> Uid;
	};
}