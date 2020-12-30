#pragma once

#include "skeletal-anim-processor.h"


struct CsvWriter : SkeletalAnimProcessor
{
	CsvWriter(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer, std::vector<float> const& data, uint32_t stride, std::ostream& output);

	bool OnEnterJoint(fbxsdk::FbxNode& node) override;

	float const* mData;
	const uint32_t mDataSize;
	const uint32_t mStride;
	std::ostream& mOutput;
};
