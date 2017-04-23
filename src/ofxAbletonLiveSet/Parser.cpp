#include "Parser.h"

OFX_ALS_BEGIN_NAMESPACE

bool Parser::open(const string& path){
	ifstream ifs;
	ifs.open(ofToDataPath(path).c_str());
	if (!ifs) return false;
	
	pugi::xml_document doc;
	
	// check extension ?
	string extension = path.substr(path.find_last_of(".") + 1);
	bool gzipped = false;
	
	if( extension == "als" || extension == "ALS") {
		gzipped = true;
		Poco::InflatingInputStream inflater(ifs, Poco::InflatingStreamBuf::STREAM_GZIP);
		if (!inflater) return false;
		
		if (!doc.load(inflater)) return false;
	}
	else {
		if(!doc.load(ifs)) return false;
	}
	
	// parse it!
	parseGeneralInfo(doc);
	parseTempo(doc);
	parseMidiTrack(doc);
	parseLocator(doc);
	parseAudioTrack(doc);
	
	LS.loadedFile=path;
	
	return true;
}

void Parser::parseGeneralInfo(const pugi::xml_document &doc){
	pugi::xpath_query q("//MasterTrack/Name");
	pugi::xpath_node_set nodes = q.evaluate_node_set(doc);
	
	if (!nodes.empty()) {
		LS.name = nodes[0].node().child("EffectiveName").attribute("Value").as_string();
		LS.userName = nodes[0].node().child("UserName").attribute("Value").as_string();
		LS.annotation = nodes[0].node().child("Annotation").attribute("Value").as_string();
	}
}

void Parser::parseTempo(const pugi::xml_document& doc){
	pugi::xpath_query q("//Tempo");
	pugi::xpath_node_set nodes = q.evaluate_node_set(doc);
	
	if (!nodes.empty()) {
		parse(LS.tempo, nodes[0].node());
	}
}

void Parser::parse(Tempo& tempo, const pugi::xml_node& node){
	map<BarTime, float> events;
	
	pugi::xpath_query q("ArrangerAutomation/Events/FloatEvent");
	pugi::xpath_node_set nodes = q.evaluate_node_set(node);
	
	for (int i = 0; i < nodes.size(); i++){
		const pugi::xml_node& n = nodes[i].node();
		
		BarTime time = n.attribute("Time").as_float();
		float value = n.attribute("Value").as_float();
		events[time] = value;
	}
	
	float default_tempo = node.child("Manual").attribute("Value").as_float();
	tempo.updateTempoMap(events, default_tempo);
}

//

void Parser::parseMidiTrack(const pugi::xml_document& doc){
	pugi::xpath_query q("//Tracks/MidiTrack");
	pugi::xpath_node_set nodes = q.evaluate_node_set(doc);
	
	LS.miditracks.clear();
	
	for (int i = 0; i < nodes.size(); i++){
		MidiTrack MT;
		parse(MT, nodes[i].node(), 0);
		LS.miditracks.push_back(MT);
	}
}

void Parser::parse(MidiTrack& MT, const pugi::xml_node &node, RealTime offset) {
	MT.name = node.child("Name").child("EffectiveName").attribute("Value").value();
	MT.color = node.child("ColorIndex").attribute("Value").as_int();
	
	{
		// time signatures are in clips and may change over time.
		// here we just get the 1st one from the 1st clip and apply it to the whole track
		// (to do)
		pugi::xpath_query q("DeviceChain/MainSequencer/ClipTimeable/ArrangerAutomation/Events/MidiClip/TimeSignature/TimeSignatures/RemoteableTimeSignature");
		pugi::xpath_node_set nodes = q.evaluate_node_set(node);
		
		if( nodes.size() > 0 ){
			const pugi::xml_node& n = nodes[0].node();
			
			MT.timeSignature.numerator = n.child("Numerator").attribute("Value").as_int(0);
			MT.timeSignature.denominator = n.child("Denominator").attribute("Value").as_int(0);
			//MT.timeSignature.time = n.child("Time").attribute("Value").as_int(0);
		}
		
	}
	
	{
		pugi::xpath_query q(".//MidiControllers/ControllerTargets.0");
		pugi::xpath_node_set nodes = q.evaluate_node_set(node);
		
		if (nodes.size()){
			controller_target_offset = nodes[0].node().attribute("Id").as_int(0);
		}
	}
	
	pugi::xpath_query q(".//ArrangerAutomation/Events/MidiClip");
	pugi::xpath_node_set nodes = q.evaluate_node_set(node);
	
	MT.clips.clear();
	
	for (int i = 0; i < nodes.size(); i++){
		MidiClip MC;
		parse(MC, nodes[i].node(), 0);
		MT.clips.push_back(MC);
	}
	
	std:sort(MT.clips.begin(), MT.clips.end(), sort_by_time<MidiClip>);
}

void Parser::parse(Note& note, const pugi::xml_node &node, RealTime offset){
	const pugi::xml_node& e = node;
	note.time = LS.tempo.toRealTime(e.attribute("Time").as_float(0)) + offset;
	note.duration = LS.tempo.toRealTime(e.attribute("Duration").as_float(0));
	note.velocity = e.attribute("Velocity").as_float(0);
}

void Parser::parse(MidiClipLoop &loop, const pugi::xml_node &node, RealTime offset){
	loop.enabled = node.child("LoopOn").attribute("Value").as_bool(false);
	loop.relativeStart = LS.tempo.toRealTime( node.child("LoopStart").attribute("Value").as_float(-1) );
	loop.start = loop.relativeStart + offset;
	loop.end = LS.tempo.toRealTime( node.child("LoopEnd").attribute("Value").as_float(-1) ) + offset;
	loop.duration = loop.end - loop.start;
	LS.tempo.toRealTime( node.child("StartRelative").attribute("Value").as_float(-1) );
	loop.outMarker = LS.tempo.toRealTime( node.child("OutMarker").attribute("Value").as_float(-1) );
}

void Parser::parse(MidiClip& MC, const pugi::xml_node &node, RealTime offset){
	float start = node.child("CurrentStart").attribute("Value").as_float();
	float end = node.child("CurrentEnd").attribute("Value").as_float();
	
	MC.time = LS.tempo.toRealTime(start) + offset;
	MC.endtime = LS.tempo.toRealTime(end) + offset;
	MC.duration = MC.endtime - MC.time;
	
	MC.color = node.child("ColorIndex").attribute("Value").as_int();
	MC.name = node.child("Name").attribute("Value").value();
	MC.annotation = node.child("Annotation").attribute("Value").value();
	
	// extract loop setting
	parse(MC.loop, node.child("Loop"), MC.time);
	
	{ // extract midi notes
		pugi::xpath_query q("Notes//KeyTracks//KeyTrack");
		pugi::xpath_node_set nodes = q.evaluate_node_set(node);
		
		MC.notes.clear();
		
		for (int i = 0; i < nodes.size(); i++){
			const pugi::xml_node& key_tracks = nodes[i].node();
			int midi_key = key_tracks.child("MidiKey").attribute("Value").as_int();
			
			pugi::xml_object_range<pugi::xml_node_iterator> notes = key_tracks.child("Notes").children();
			
			// grab notes from clip
			pugi::xml_node_iterator it = notes.begin();
			while (it != notes.end()){
				Note note;
				note.key = midi_key;
				
				parse(note, *it, MC.time);
				MC.notes.push_back(note);
				
				it++;
			}
		}
		
		// duplicate notes according to loop setting
		if(MC.loop.enabled){
			
			// grab notes contained in loop section
			vector<Note> loopNotes;
			loopNotes.clear();
			for(int i=0; i<MC.notes.size(); i++){
				if(MC.notes[i].time >= MC.loop.start && MC.notes[i].time < MC.loop.end){
					loopNotes.push_back(MC.notes[i]);
				}
			}
			
			for(Time t=MC.loop.duration; t < MC.duration; t+=MC.loop.duration){
				for(int i=0; i<loopNotes.size(); i++){
					if(loopNotes[i].time < MC.endtime) MC.notes.push_back(loopNotes[i] + t);
				}
			}
			
		}
		
		std:sort(MC.notes.begin(), MC.notes.end(), sort_by_time<Note>);
	}

	{ // extract envelopes
		pugi::xpath_query q("Envelopes//ClipEnvelope");
		pugi::xpath_node_set nodes = q.evaluate_node_set(node);
		
		for (int i = 0; i < nodes.size(); i++){
			const pugi::xml_node& clip_envelope = nodes[i].node();
			
			int id = clip_envelope.child("EnvelopeTarget").child("PointeeId").attribute("Value").as_int() - controller_target_offset;
			
			MC.envelopes.push_back(Automation());
			Automation &automation = MC.envelopes.back();
			
			automation.id = id;
			
			pugi::xml_object_range<pugi::xml_node_iterator> events = clip_envelope.child("Automation").child("Events").children();
			pugi::xml_node_iterator it = events.begin();
			
			while (it != events.end()){
				float time = LS.tempo.toRealTime(it->attribute("Time").as_float()) + MC.time;
				float value = it->attribute("Value").as_float();
				if (time < 0) time = 0;
				
				automation.events[time] = value;
				it++;
			}
		}
	}
	
	{ // todo: extract warpmarkers
		// /Tracks/MidiTrack/DeviceChain/ClipTimeable/ArrangerAutomation/Events/MidiClip/WarpMarkers/
		// <WarpMarker SecTime="0.015625" BeatTime="0.03125"/>
	}
	
	{ // todo: extract time signature
		// /Tracks/MidiTrack/DeviceChain/ClipTimeable/ArrangerAutomation/Events/MidiClip/TimeSignature/TimeSignatures/RemoteableTimeSignature/
	}
	
	{ // todo: extract floatEvents boolEvents from midiTrack plugins ?
		// /Tracks/MidiTrack/DeviceChain/DeviceChain/Devices/PluginDevice/ParameterList
	}
}

//

void Parser::parseAudioTrack(const pugi::xml_document& doc){
	pugi::xpath_query q("//Tracks/AudioTrack");
	pugi::xpath_node_set nodes = q.evaluate_node_set(doc);
	
	LS.audiotracks.clear();
	
	for (int i = 0; i < nodes.size(); i++){
		AudioTrack AT;
		parse(AT, nodes[i].node(), 0);
		LS.audiotracks.push_back(AT);
	}
}

void Parser::parse(AudioTrack& AT, const pugi::xml_node &node, RealTime offset) {
	AT.name = node.child("Name").child("EffectiveName").attribute("Value").value();
	AT.color = node.child("ColorIndex").attribute("Value").as_int();
	
	pugi::xpath_query q(".//ArrangerAutomation/Events/AudioClip");
	pugi::xpath_node_set nodes = q.evaluate_node_set(node);
	
	// AudioTrack/DeviceChain/MainSequencer/Sample/ArrangerAutomation/Events/AudioClip
	
	AT.clips.clear();
	
	for (int i = 0; i < nodes.size(); i++){
		AudioClip AC;
		parse(AC, nodes[i].node(), 0);
		AT.clips.push_back(AC);
	}
	
	std:sort(AT.clips.begin(), AT.clips.end(), sort_by_time<AudioClip>);
}

void Parser::parse(AudioClip& AC, const pugi::xml_node &node, RealTime offset){
	
	// /CurrentStart
	// /CurrentEnd
	// /TimeSignature
	// /Name
	// /ColorIndex
	
	float start = node.child("CurrentStart").attribute("Value").as_float();
	float end = node.child("CurrentEnd").attribute("Value").as_float();
	
	
	AC.time = LS.tempo.toRealTime(start) + offset;
	AC.endtime = LS.tempo.toRealTime(end) + offset;
	AC.duration = AC.endtime - AC.time;
	
	AC.color = node.child("ColorIndex").attribute("Value").as_int();
	AC.name = node.child("Name").attribute("Value").value();
	//MC.annotation = node.child("Annotation").attribute("Value").value();
	
	// extract loop setting
	//parse(MC.loop, node.child("Loop"), AC.time);
	
}

//

void Parser::parseLocator(const pugi::xml_document& doc) {
	pugi::xpath_query q("//Locators/Locators/Locator");
	pugi::xpath_node_set nodes = q.evaluate_node_set(doc);
	
	LS.locators.clear();
	
	for (int i = 0; i < nodes.size(); i++){
		Locator L;
		parse(L, nodes[i].node(), 0);
		LS.locators.push_back(L);
	}
	
	std:sort(LS.locators.begin(), LS.locators.end(), sort_by_time<Locator>);
}

void Parser::parse(Locator& L, const pugi::xml_node& node, RealTime offset){
	L.name = node.child("Name").attribute("Value").as_string();
	L.annotation = node.child("Annotation").attribute("Value").as_string();
	L.time = LS.tempo.toRealTime(node.child("Time").attribute("Value").as_float()) + offset;
}

OFX_ALS_END_NAMESPACE
