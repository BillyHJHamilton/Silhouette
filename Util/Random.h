#pragma once

#include<random>
#include<vector>

namespace Random
{
	void SeedGenerator();
	void SeedGenerator(unsigned int seed);
	std::mt19937 &GetGenerator();

	int IntInRange(int min, int max);
	float FloatInRange(float min, float max);
	int Index(int size);
	int WeightedIndex(const std::vector<int> &weights);
	bool Coinflip();
	bool OneIn(int x);

	template<class T>
	int Index(std::vector<T> v)
	{
		return Random::Index(v.size());
	}

	template<class T>
	void ShuffleVector(std::vector<T> v)
	{
		std::shuffle(v.begin(), v.end(), GetGenerator());
	}
}