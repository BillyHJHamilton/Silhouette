#include <cassert>
#include <iostream>
#include "Random.h"

namespace Random
{
	static std::mt19937 s_Generator;
	std::mt19937 &GetGenerator() { return s_Generator; }

	static float s_Epsilon = 0.0000001f;

	void SeedGenerator()
	{
		// Use the cryptographically secure generator to seed the faster Mersenne Twister generator.
		std::random_device r;
		s_Generator = std::mt19937(r());
	}

	void SeedGenerator(unsigned int seed)
	{
		s_Generator = std::mt19937(seed);
	}

	int IntInRange(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(s_Generator);
	}

	float FloatInRange(float min, float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(s_Generator);
	}

	int Index(int size)
	{
		std::uniform_int_distribution<int> dist(0, size-1);
		return dist(s_Generator);
	}

	int WeightedIndex(const std::vector<int> &weights)
	{
		assert(weights.size() > 0);
		int sum = 0;
		for (int w : weights)
		{
			sum += w;
		}
		int x = Random::IntInRange(0, sum-1);

		for (int i = 0; i < (int)weights.size(); i++)
		{
			assert(weights[i] > 0);
			x -= weights[i];
			if (x < 0)
				return i;
		}
		std::cerr << "RandomNumbers - weighted_index (int) failed to resolve." << std::endl;
		std::cerr << "Contents of weight vector: ";
		for (int w : weights)
		{
			std::cerr << w;
		}
		std::cerr << std::endl;
		return 0;
	}

	bool Coinflip()
	{
		static std::uniform_int_distribution<int> dist(0, 1);
		return dist(s_Generator) == 1;
	}

	bool OneIn(int x)
	{
		assert(x >= 1);
		std::uniform_int_distribution<int> dist(1, x);
		return dist(s_Generator) == 1;
	}
}