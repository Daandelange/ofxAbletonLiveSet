#include "Model.h"

OFX_ALS_BEGIN_NAMESPACE

float Automation::getValueAt(float time) const {
	if (events.size() < 2) return -1;
	
	map<float, float>::const_iterator start = events.upper_bound(time);
	map<float, float>::const_iterator end = start;
	start--;
	
	if (start == events.begin()) return start->second;
	if (end == events.end()) return start->second;
	
	const float t = time - start->first;
	const float td = end->first - start->first;
	const float vd = end->second - start->second;

	return start->second + vd * (t / td);
}

// Data from & thanks to : https://github.com/elixirbeats/abletoolz/blob/8e3dab304ca1fe4344d7b662fc76e57ac4cd63ee/abletoolz/color_tools.py#L13-L19
const int AbletonColorSwatch::Colors[70] = {
	0xFF94A6, 0xFFA428, 0xCD9827, 0xF6F57C, 0xBEFA00, 0x21FF41, 0x25FEA9, 0x5DFFE9, 0x8AC5FE, 0x5480E4, 0x93A6FF, 0xD86CE4, 0xE552A1, 0xFFFEFE,
	0xFE3637, 0xF66D02, 0x99734A, 0xFEF134, 0x87FF67, 0x3DC201, 0x01BEAF, 0x18E9FE, 0x10A4EE, 0x007DC0, 0x886CE4, 0xB776C6, 0xFE38D4, 0xD1D0D1,
	0xE3665A, 0xFEA274, 0xD2AD70, 0xEDFFAE, 0xD3E499, 0xBAD175, 0x9AC58D, 0xD4FCE0, 0xCCF0F8, 0xB8C1E2, 0xCDBBE4, 0xAF98E4, 0xE5DDE0, 0xA9A8A8,
	0xC6938A, 0xB68257, 0x98826A, 0xBEBB69, 0xA6BE00, 0x7CB04C, 0x89C3BA, 0x9BB3C4, 0x84A5C3, 0x8392CD, 0xA494B5, 0xBF9FBE, 0xBD7096, 0x7B7A7A,
	0xAF3232, 0xA95131, 0x734E41, 0xDAC200, 0x84971F, 0x529E31, 0x0A9C8E, 0x236285, 0x1A2F96, 0x2E52A3, 0x624BAD, 0xA24AAD, 0xCD2E6F, 0xFFFEFE,
};

OFX_ALS_END_NAMESPACE
