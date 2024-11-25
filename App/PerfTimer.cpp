#include "PerfTimer.h"

#if PERF_TEST
#include <iomanip>
#include "GameApp.h"
#include "Util/MapUtil.h"
#include "Util/VectorUtil.h"

namespace PerfTimerPrivate
{
	struct Info
	{
		double m_TotalTime = 0.0;
		int32 m_NumHits = 0;
	};

	static std::unordered_map<const char*,Info> s_Map;
}

PerfTimer::PerfTimer(const char* timerName) :
	m_TimerName(timerName),
	m_StartTime(GameApp::Get().GetClockTime())
{
}

PerfTimer::~PerfTimer()
{
	double endTime = GameApp::Get().GetClockTime();
	PerfTimerPrivate::Info& info = Util::FindOrAdd(PerfTimerPrivate::s_Map, m_TimerName);
	info.m_TotalTime += (endTime - m_StartTime);
	++info.m_NumHits;
}

void PerfTimer::PrintAll()
{
	struct TimerResult
	{
		const char* name;
		double avg;

		bool operator>(const TimerResult& other) const
		{
			return avg > other.avg;
		}
	};

	// Sort the timers from longest to smallest.
	std::vector<TimerResult> list;
	for (const auto& pair : PerfTimerPrivate::s_Map)
	{
		const char* name = pair.first;
		const PerfTimerPrivate::Info& info = pair.second;
		const double avg = info.m_TotalTime / static_cast<double>(info.m_NumHits);
		list.push_back({name, avg});
	}
	Util::SortDescending(list);

	// Print them.
	if (list.size() > 0)
	{
		std::cout << "Average Performance Results: " << std::endl
			<< std::fixed << std::setprecision(3);
		for (const TimerResult& pair : list)
		{
			if (pair.avg < 0.001)
			{
				std::cout << " " << std::setw(40) << pair.name
					<< std::setw(10) << pair.avg*1'000'000.0 << " micro";// << std::endl;
			}
			else
			{
				std::cout << " " << std::setw(40) << pair.name
					<< std::setw(10) << pair.avg*1'000.0 << " ms   ";// << std::endl;
			}

			//const double perSecond = 1.0 / avg;
			//std::cout << " (" << perSecond << "/second)" << std::endl;

			const double pctBudget = 100.0 * (pair.avg/(1.0/60.0));
			std::cout << "   (" << pctBudget << "%)" << std::endl;
		}
		std::cout << std::endl;
	}
}

#else
PerfTimer::PerfTimer(const char* timerName)
{
}

PerfTimer::~PerfTimer()
{
}

void PerfTimer::PrintAll()
{
}
#endif