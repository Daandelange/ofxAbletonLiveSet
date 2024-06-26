#pragma once

#include "Constants.h"

OFX_ALS_BEGIN_NAMESPACE

// fixme: Ableton seems to use double precision, using floats truncates the values.
typedef float BarTime;
typedef float RealTime;

class TempoException : public std::exception {
    public:
        TempoException(char* _msg) : msg(_msg){};
        virtual char * what () const noexcept override {
            return msg;
        }
    private:
        char* msg;
};

class Tempo {
public:
	
	Tempo() : timemap(NULL) {}
	~Tempo();
	
	RealTime toRealTime(BarTime bar_time);
	
	BarTime toBarTime(RealTime real_time);
	
	float getGlobalTempo() const;
	
	void updateTempoMap(const map<BarTime, float>& data, float default_tempo);
	
private:
	
	float globalTempo = 120.0f;
	
	struct Event {
		BarTime time;
		float tempo;
	};
	
	vector<Event> events;
	
	// ====
	
	struct Timemap {
		virtual ~Timemap() {}
		virtual RealTime toRealTime(BarTime bar_time) = 0;
		virtual BarTime toBarTime(RealTime real_time) = 0;
	} *timemap;
	
	struct SimpleTimemap : public Timemap {
		float to_realtime_factor;
		float to_bartime_factor;
		
		SimpleTimemap(float default_tempo) {
			to_realtime_factor = (60. / default_tempo);
			to_bartime_factor = 1. / to_realtime_factor;
		}
		
		RealTime toRealTime(BarTime bar_time) {
			return to_realtime_factor * bar_time;
		}
		
		BarTime toBarTime(RealTime real_time) {
			return to_bartime_factor * real_time;
		}
	};
	
#if 0
	struct AutometedTimemap : public Timemap {
		AutometedTimemap(vector<Event>& e) {
			RealTime curtime = 0;
			
			for (int i = 0; i < e.size() - 1; i++){
				Event &e0 = e[i];
				Event &e1 = e[i + 1];
				
				float e0_beattime = (60. / e0.tempo);
				float e1_beattime = (60. / e1.tempo);
				
				float bartime_delta = e1.time.getBarTime() - e0.time.getBarTime();
				
				float X = (e0_beattime + e1_beattime) * bartime_delta * 0.5;
				
				cout << e0.time.getBarTime() << " -> " << e1.time.getBarTime() << endl;
				cout << curtime << " " << curtime + X << endl;
				curtime += X;
			}
		}
		
		RealTime toRealTime(BarTime bar_time){
			return 0;
		}
		
		BarTime toBarTime(RealTime real_time){
			return 0;
		}
	};
#endif
};

OFX_ALS_END_NAMESPACE
