#include "hierarchy-writer.h"
#include "fbxsdk/scene/geometry/fbxnode.h"
#include "fbxsdk/scene/geometry/fbxnodeattribute.h"
#include "fbxsdk/scene/animation/fbxanimlayer.h"

const char* const HierarchyWriter::kChannelNames[]{
#define DECL_CHANNEL(ignore, axis, target) #axis #target,
#include "channels.inc"
#undef DECL_CHANNEL
};

HierarchyWriter::HierarchyWriter(std::vector<std::string> const& excludes, fbxsdk::FbxAnimLayer& layer, std::ostream& os)
:	SkeletalAnimProcessor(excludes, layer),
	mOutput(os)
{
	os << "HIERARCHY" << std::endl;
}

bool HierarchyWriter::OnEnterJoint(fbxsdk::FbxNode& node)
{
	Indent();
	mOutput << (mDepth == 0 ? "ROOT" : "JOINT") << ' ' << node.GetName() << std::endl;

	Indent();
	mOutput << "{" << std::endl;

	++mDepth;

	Indent();
	// the offset is dictated by the position of the nodes in the scene; we're ignoring it here.
	mOutput << "OFFSET 0 0 0" << std::endl;

	Indent();
	mOutput << "CHANNELS ";

	PrintChannels(node);
	mOutput << std::endl;

	if (0 == node.GetChildCount())
	{
		PrintEndSite();
	}
	return true;
}

void HierarchyWriter::OnExit(fbxsdk::FbxNode& node)
{
	auto attribs = node.GetNodeAttribute();
	if (attribs && attribs->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
	{
		--mDepth;

		Indent();
		mOutput << "}" << std::endl;
	}
}

void HierarchyWriter::Indent()
{
	for (auto i = 0; i < mDepth; ++i)
	{
		mOutput << "  ";
	}
}

void HierarchyWriter::PrintChannels(fbxsdk::FbxNode& node)
{
	Channel channels[kNumChannels];
	Channel* writep = channels;

	void(*channelProcessors[])(fbxsdk::FbxAnimLayer& layer, fbxsdk::FbxNode& node, Channel*& writep) {
#define DECL_CHANNEL(source, axis, target) [](fbxsdk::FbxAnimLayer& layer, fbxsdk::FbxNode& node, Channel*& writep)\
	{\
		if ((node.source).GetCurve(&layer, #axis))\
		{\
			*writep = axis ## target;\
			++writep;\
		}\
	},
#include "channels.inc"
#undef DECL_CHANNEL
	};
	for (auto fn : channelProcessors)
	{
		fn(mAnimLayer, node, writep);
	}

	mOutput << writep - channels << ' ';
	Channel const* readp = channels;
	if (readp != writep)
	{
		mOutput << kChannelNames[*readp];
		++readp;
	}

	while (readp != writep)
	{
		mOutput << ' ' << kChannelNames[*readp];
		++readp;
	}
}

void HierarchyWriter::PrintEndSite()
{
	Indent();
	mOutput << "End Site" << std::endl;

	Indent();
	mOutput << "{" << std::endl;

	++mDepth;
	Indent();
	mOutput << "OFFSET 0 0 0" << std::endl;
	--mDepth;

	Indent();
	mOutput << "}" << std::endl;
}
