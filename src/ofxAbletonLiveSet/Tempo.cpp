#include "Tempo.h"

OFX_ALS_BEGIN_NAMESPACE

Tempo::~Tempo(){
	if (timemap){
		delete timemap;
		timemap = NULL;
	}
}

RealTime Tempo::toRealTime(BarTime bar_time){
	return timemap->toRealTime(bar_time);
}

BarTime Tempo::toBarTime(RealTime real_time){
	return timemap->toBarTime(real_time);
}

float Tempo::getGlobalTempo() const {
	return globalTempo;
}
	
void Tempo::updateTempoMap(const map<BarTime, float>& data, float default_tempo){
	events.clear();
	
	map<BarTime, float>::const_iterator it = data.begin();
	while (it != data.end()){
		Event e;
		e.time = it->first;
		e.tempo = it->second;
		
		if (e.time < 0) e.time = 0;
		events.push_back(e);
		
		it++;
	}
	
	if (timemap){
		delete timemap;
		timemap = NULL;
	}
	
	globalTempo = default_tempo;
	
	if (events.size() <= 1){
		timemap = new SimpleTimemap(default_tempo);
	}
	else{
		// timemap = new AutometedTimemap(events);
		throw "tempo automation is not implemented yet";
	}
	
	for (int i = 0; i < events.size(); i++){
		events[i].time = toRealTime(events[i].time);
	}
}

OFX_ALS_END_NAMESPACE
