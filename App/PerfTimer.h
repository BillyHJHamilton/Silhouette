#pragma once

#include <unordered_map>
#include "Core/BasicTypes.h"
#include "Core/Core.h"

// Note: Multiple timers with the same name are not currently supported.
class PerfTimer
{
public:
	PerfTimer(const char* timerName);
	~PerfTimer();

	static void PrintAll();

private:
#if PERF_TEST
	const char* m_TimerName;
	double m_StartTime;
#endif
};