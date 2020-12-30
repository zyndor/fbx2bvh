#pragma once

#include "skeletal-anim-processor.h"

struct HierarchyWriter : SkeletalAnimProcessor
{
	HierarchyWriter(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer, std::ostream& os);

private:
	enum Channel
	{
#define DECL_CHANNEL(ignore, axis, target) axis ## target,
#include "channels.inc"
		kNumChannels
#undef DECL_CHANNEL
	};

	static const char* const kChannelNames[kNumChannels];

	std::ostream& mOutput;
	int mDepth = 0;

	bool OnEnterJoint(fbxsdk::FbxNode& node) override;
	void OnExit(fbxsdk::FbxNode& node) override;

	void Indent();
	void PrintChannels(fbxsdk::FbxNode& node);
	void PrintEndSite();
};

