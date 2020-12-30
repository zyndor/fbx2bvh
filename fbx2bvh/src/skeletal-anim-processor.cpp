#include "skeletal-anim-processor.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/geometry/fbxnodeattribute.h"

SkeletalAnimProcessor::SkeletalAnimProcessor(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer)
:	ExclusionVisitor(excludes),
	mAnimLayer(layer)
{}

bool SkeletalAnimProcessor::OnEnterIncluded(fbxsdk::FbxNode& node)
{
	auto attribs = node.GetNodeAttribute();
	bool result = (attribs && attribs->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton) ?
		OnEnterJoint(node) : true;

	return result;
}

void SkeletalAnimProcessor::OnExit(fbxsdk::FbxNode& /*node*/)
{}
