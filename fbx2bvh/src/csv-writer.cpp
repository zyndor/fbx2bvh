#include "csv-writer.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/geometry/fbxnodeattribute.h"

CsvWriter::CsvWriter(std::vector<std::string> const & excludes, fbxsdk::FbxAnimLayer & layer, std::vector<float> const & data, uint32_t stride, std::ostream & output)
:	SkeletalAnimProcessor(excludes, layer),
	mData(data.data()),
	mDataSize(data.size()),
	mStride(stride),
	mOutput(output)
{}

inline bool CsvWriter::OnEnterJoint(fbxsdk::FbxNode & node)
{
	struct Channel
	{
		fbxsdk::FbxAnimCurve* curve;
		const char* targetName;
	} channels[]{
#define DECL_CHANNEL(source, axis, target) { (node.source).GetCurve(&mAnimLayer, #axis), #axis #target },
#include "channels.inc"
#undef DECL_CHANNEL
	};
	for (auto c : channels)
	{
		if (c.curve)
		{
			mOutput << node.GetName() << ", " << c.targetName;
			auto readp = mData;
			auto endp = readp + mDataSize;
			while (readp != endp)
			{
				mOutput << ", " << *readp;
				readp += mStride;
			}
			mOutput << std::endl;
			++mData;
		}
	}
	return true;
}
