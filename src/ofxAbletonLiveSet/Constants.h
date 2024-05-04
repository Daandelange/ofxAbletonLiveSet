#pragma once

#include "ofMain.h"
#include "ofxAlsPugi.h"

#define OFX_ALS_BEGIN_NAMESPACE namespace ofx { namespace AbletonLiveSet {
#define OFX_ALS_END_NAMESPACE } }

// Uncomment below to disable POCO. Disables deflating.
// Or better: set is as a project (or addon) compilation flag !
//#define OFX_ALS_WITHOUT_POCO
OFX_ALS_BEGIN_NAMESPACE
OFX_ALS_END_NAMESPACE

namespace ofxAbletonLiveSet = ofx::AbletonLiveSet;
namespace ofxAls = ofxAbletonLiveSet;
