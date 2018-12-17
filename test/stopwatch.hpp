
#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

#include <iostream>
#include <time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

class Stopwatch {
public:
  Stopwatch() { start(); }
  Stopwatch(const timespec &startTime, const timespec &stopTime)
      : mStartTime(startTime), mStopTime(stopTime) {}

  Stopwatch(const timespec &startTime) : mStartTime(startTime) {}

  Stopwatch(const Stopwatch &other)
      : mStartTime(other.mStartTime), mStopTime(other.mStopTime) {}

  inline void start() {
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    mStartTime.tv_sec = mts.tv_sec;
    mStartTime.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &mStartTime);
#endif
  }

  inline void stop() {
#ifdef __MACH__ // OS X does not have clock_gettime, use clock_get_time
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    mStopTime.tv_sec = mts.tv_sec;
    mStopTime.tv_nsec = mts.tv_nsec;
#else
    clock_gettime(CLOCK_REALTIME, &mStopTime);
#endif
  }

  Stopwatch operator-(const Stopwatch &other) const {
    return Stopwatch(mStartTime, other.getStartTime());
  }

  inline double getTimeDiff() const {
    return ((mStopTime.tv_sec - mStartTime.tv_sec) +
            (double(mStopTime.tv_nsec - mStartTime.tv_nsec) / BILLION)) *
           1000;
  }

  inline double getTimeDiffInSecondsWithHighresolution() const {
    return (mStopTime.tv_sec - mStartTime.tv_sec) +
           double(mStopTime.tv_nsec - mStartTime.tv_nsec) / BILLION;
  }

  inline size_t getTimeDiffInSeconds() const {
    return (mStopTime.tv_sec - mStartTime.tv_sec);
  }

  inline bool getIsTimerExpired(const size_t nanoSeconds) {
    return ((mStopTime.tv_sec - mStartTime.tv_sec) * BILLION +
                (mStopTime.tv_nsec - mStartTime.tv_nsec) >=
            nanoSeconds);
  }

  const timespec &getStartTime() const { return mStartTime; }

  double getStartTimeInSeconds() const {
    return double(mStartTime.tv_sec) + double(mStartTime.tv_nsec) / BILLION;
  }

  const timespec &getStopTime() const { return mStopTime; }

  double getStopTimeInSeconds() const {
    return double(mStopTime.tv_sec) + double(mStopTime.tv_nsec) / BILLION;
  }

friend std::ostream &operator<<(std::ostream &os, const Stopwatch &timer) {
    using namespace std;
    os << timer.mStartTime.tv_sec << ":" << timer.mStartTime.tv_nsec << " ,"
       << timer.mStopTime.tv_sec << ": " << timer.mStopTime.tv_nsec << endl;
    return os;
  }

private:
  /// Constants for better reading
  static const long BILLION = 1000000000L;
  timespec mStartTime;
  /// The stopping time stamp
  timespec mStopTime;
};

#endif
