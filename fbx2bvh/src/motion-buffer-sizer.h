#pragma once

#include "skeletal-anim-processor.h"

struct MotionBufferSizer : SkeletalAnimProcessor
{
	MotionBufferSizer(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer);

	bool OnEnterJoint(fbxsdk::FbxNode& node) override;

	uint32_t GetChannelCount() const
	{
		return mChannelCount;
	}

	uint32_t GetFrameCount(double frameDelaySeconds) const
	{
		return static_cast<uint32_t>(std::ceil(mDuration / frameDelaySeconds));
	}

private:
	uint32_t mChannelCount = 0;
	double mDuration = 0.;
};
