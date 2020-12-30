#include "motion-writer.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/animation/fbxanimcurve.h"

namespace
{

constexpr double ROUNDING_FACTOR = 1024. * 1024.;

template <typename T>
T Round(T value)
{
	return std::round(value * ROUNDING_FACTOR) / ROUNDING_FACTOR;
}

template <typename T>
T Lerp(T v0, T v1, T alpha)
{
	return v0 + (v1 - v0) * alpha;
}

template <typename T>
T InvLerp(T v0, T v1, T vAlpha)
{
	return (vAlpha - v0) / (v1 - v0);
}

}

MotionWriter::MotionWriter(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer,
	uint32_t frameCount, uint32_t channelCount, double frameDelaySeconds)
:	SkeletalAnimProcessor(excludes, layer),
	mFrameCount(frameCount),
	mStride(channelCount),
	mFrameDelaySeconds(frameDelaySeconds),
	mBuffer(frameCount * channelCount, 0.f),
	mWritep(mBuffer.data())
{}

bool MotionWriter::OnEnterJoint(fbxsdk::FbxNode& node)
{
	struct
	{
		fbxsdk::FbxPropertyT<fbxsdk::FbxDouble3>& prop;
		const char* const curveName;
	} channels[]{
#define DECL_CHANNEL(source, axis, ignore) { (node.source), #axis },
#include "channels.inc"
#undef DECL_CHANNEL
	};
	for (auto& c : channels)
	{
		if (auto curve = c.prop.GetCurve(&mAnimLayer, c.curveName))
		{
			WriteCurve(*curve, Lerp<float>);
		}
	}

	return true;
}

void MotionWriter::WriteCurve(fbxsdk::FbxAnimCurve const& curve, Interpolator interpolator)
{
	auto writep = mWritep;
	auto k0 = curve.KeyGet(0);
	double ts0 = Round(k0.GetTime().GetSecondDouble());
	float val0 = k0.GetValue();

	double t = 0.f;
	while (Round(t) < ts0)
	{
		*writep = val0;
		writep += mStride;
		t += mFrameDelaySeconds;
	}

	for (int i0 = 1, i1 = curve.KeyGetCount(); i0 < i1; ++i0)
	{
		auto k1 = curve.KeyGet(i0);
		double ts1 = Round(k1.GetTime().GetSecondDouble());
		float val1 = k1.GetValue();
		while (Round(t) < ts1)
		{
			double alpha = Round(InvLerp(ts0, ts1, t));
			*writep = interpolator(val0, val1, static_cast<float>(alpha));
			writep += mStride;
			t += mFrameDelaySeconds;
		}

		ts0 = ts1;
		val0 = val1;
	}

	double tEnd = Round(mFrameCount * mFrameDelaySeconds);
	while (Round(t) < tEnd)
	{
		*writep = *(writep - 1);
		writep += mStride;
		t += mFrameDelaySeconds;
	}

	++mWritep;
}
