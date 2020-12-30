#include "motion-buffer-sizer.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/animation/fbxanimcurve.h"
#include <algorithm>

MotionBufferSizer::MotionBufferSizer(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer)
:	SkeletalAnimProcessor(excludes, layer)
{}

bool MotionBufferSizer::OnEnterJoint(fbxsdk::FbxNode& node)
{
	uint32_t channelCount = 0;
	double maxTime = 0.;
	for (auto c : {
#define DECL_CHANNEL(source, axis, ignore) (node.source).GetCurve(&mAnimLayer, #axis),
#include "channels.inc"
#undef DECL_CHANNEL
	})
	{
		if (c)
		{
			++channelCount;
			auto keyCount = c->KeyGetCount();
			if (keyCount > 0)
			{
				maxTime = std::max(maxTime, c->KeyGet(keyCount - 1).GetTime().GetSecondDouble());
			}
		}
	}

	mChannelCount += channelCount;
	mDuration = std::max(maxTime, mDuration);

	return true;
}