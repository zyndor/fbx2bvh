#pragma once

#include "skeletal-anim-processor.h"

namespace fbxsdk
{
class FbxAnimCurve;
}

struct MotionWriter : SkeletalAnimProcessor
{
	MotionWriter(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer,
		uint32_t frameCount, uint32_t channelCount, double frameDelaySeconds);

	bool OnEnterJoint(fbxsdk::FbxNode& node) override;

	std::vector<float> const& GetBuffer() const
	{
		return mBuffer;
	}

private:
	using Interpolator = float(*)(float v0, float v1, float alpha);

	uint32_t mFrameCount;
	uint32_t mStride;
	double mFrameDelaySeconds;

	std::vector<float> mBuffer;
	float* mWritep;

	void WriteCurve(fbxsdk::FbxAnimCurve const& curve, Interpolator interpolator);
};

