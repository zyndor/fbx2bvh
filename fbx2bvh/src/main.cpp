#include "fbx-sdk.h"
#include "options.h"
#include "hierarchy-hasher.h"
#include "hierarchy-writer.h"
#include "motion-buffer-sizer.h"
#include "motion-writer.h"
#include "csv-writer.h"

#include "fbxsdk/scene/fbxscene.h"
#include "fbxsdk/scene/animation/fbxanimlayer.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

namespace
{

const float DEFAULT_FPS = 24.f;

void ReplaceInvalidFilenameChars(std::string& filename)
{
	const char invalid[] = "\\/:?*\"<>|";
	size_t start = 0;
	while (true)
	{
		start = filename.find_first_of(invalid, start);
		if (start == std::string::npos)
		{
			break;
		}
		filename[start] = '$';
		++start;
	};
}

std::string UpDir(const std::string& path)
{
	size_t offs = std::min(path.rfind('/'), path.rfind('\\'));
	return path.substr(0, offs + (offs == std::string::npos || path[offs] == '/' || path[offs] == '\\'));
}

void Usage()
{
	std::cout << std::endl;
	std::cout << "USAGE: fbx2bvh (options) [fbx path] (optional [bvh output path])" << std::endl;
	std::cout << std::endl;
	std::cout << "OPTIONS:" << std::endl;
	std::cout << "\t-e [pattern]: names of nodes to exclude from processing." << std::endl;
	std::cout << "\t-i [fbx path]: path to the .fbx file to load." << std::endl;
	std::cout << "\t-o [bvh output path]: path to use to output the BVH files." << std::endl;
	std::cout << "\t-f [milliseconds]: the frame delay of the animation." << std::endl;
	std::cout << "\t-t [milliseconds]: the target frame delay of the animation." << std::endl;
	std::cout << "\t-c: write hierarchy and motion as a single CSV." << std::endl;
	std::cout << "\t-d: dry run; hierarchies and motion data are output to stdout." << std::endl;
	std::cout << "\t-h: print this information and exit." << std::endl;
	std::cout << std::endl;
	std::cout << "\tExcludes: Nodes with names matching the patterns will not be descended into. Supports '*' wildcard at beginning and end. Multiple exclusions may be supplied, separated by ':' - if either is matched, the node is excluded." << std::endl;
	std::cout << "\tInput: Compulsory. Only if the switch was not used, and there is a spare argument supplied at the end, an attempt will be made to interpret that as input." << std::endl;
	std::cout << "\tOutput: Only if the switch was not used, and there is another spare argument supplied at the end, an attempt will be made to interpret that as output path." <<
		"The default is the directory of the input file. Hierarchies are saved as ${name_of_animation_layer}.txt. Motions are saved as ${name_of_hierarchy}_${name_of_animation_layer}.bin" << std::endl;
	std::cout << "\tFrame delay: The default is 1 / " << DEFAULT_FPS << " seconds. This is used as the sampling rate of the animations." << std::endl;
	std::cout << "\tTarget frame delay: This may be used to change the final speed of the animations. Defaults to the value of the frame delay." << std::endl;
	std::cout << std::endl;
}

struct MotionHeader
{
	uint32_t numFrames;
	float frameDelaySeconds;
	uint32_t numChannels;
};

void PerformProcessing(Options const& opts, FbxSdk const& sdk)
{
	std::map<uint64_t, std::string> hierarchies;
	for (auto i0 = 0, i1 = sdk.mScene->GetSrcObjectCount<fbxsdk::FbxAnimStack>(); i0 < i1; ++i0)
	{
		auto animStack = sdk.mScene->GetSrcObject<fbxsdk::FbxAnimStack>(i0);
		for (auto j0 = 0, j1 = animStack->GetMemberCount<fbxsdk::FbxAnimLayer>(); j0 < j1; ++j0)
		{
			auto animLayer = animStack->GetMember<fbxsdk::FbxAnimLayer>(j0);

			HierarchyHasher hh(opts.excludes, *animLayer);
			hh.Visit(*sdk.mScene->GetRootNode());

			auto iFind = hierarchies.find(hh.GetHash());
			// If the hierarchy has not been written yet, and we didn't want CSV, write it now.
			if (iFind == hierarchies.end())
			{
				std::string name(animLayer->GetName());
				ReplaceInvalidFilenameChars(name);

				std::ostream* output = &std::cout;
				if (opts.dry)
				{
					std::cout << "Hierarchy: " << name << std::endl << std::endl;
				}
				else if (!opts.csv)
				{
					auto filename = opts.outPath + name + ".txt";

					auto file = new std::ofstream(filename);
					if (!file->good())
					{
						delete file;
						throw std::runtime_error("Failed to open '" + filename + "' for writing.");
					}

					output = file;
				}

				HierarchyWriter hw(opts.excludes, *animLayer, *output);
				hw.Visit(*sdk.mScene->GetRootNode());

				if (!(opts.dry || opts.csv))
				{
					delete output;
				}

				iFind = hierarchies.insert({ hh.GetHash(), name }).first;
			}

			// now write the motion data.
			MotionBufferSizer mbs(opts.excludes, *animLayer);
			mbs.Visit(*sdk.mScene->GetRootNode());

			const uint32_t frameCount = mbs.GetFrameCount(opts.frameDelaySeconds);

			MotionWriter mw(opts.excludes, *animLayer, frameCount, mbs.GetChannelCount(),
				opts.frameDelaySeconds);
			mw.Visit(*sdk.mScene->GetRootNode());

			std::string motionName(animLayer->GetName());
			ReplaceInvalidFilenameChars(motionName);
			if (opts.dry)
			{
				std::cout << std::endl << "Motion name: " << motionName << std::endl;
				std::cout << "\tHierarchy name: " << iFind->second << std::endl;
				std::cout << "\tNumber of channels: " << mbs.GetChannelCount() << std::endl;
				std::cout << "\tNumber of frames @ " << 1.f / opts.frameDelaySeconds << " FPS: " << frameCount << std::endl;
				std::cout << "\tTarget frame delay: " << opts.targetFrameDelaySeconds << std::endl;

				auto i = mw.GetBuffer().begin();
				auto iEnd = mw.GetBuffer().end();
				auto frame = 0;
				while (i != iEnd)
				{
					std::cout << "FRAME " << frame << ":";
					auto iRowEnd = i + mbs.GetChannelCount();
					while (i != iRowEnd)
					{
						float val = *i;
						std::cout << " " << (val * val > 1e-10f ? val : 0.f);
						++i;
					}
					std::cout << std::endl;
					++frame;
				}
			}
			else
			{
				auto motionFilename = opts.outPath + iFind->second + "_" + motionName + (opts.csv ? ".csv" : ".bin");
				std::ofstream motionFile(motionFilename, std::ios::binary);
				if (!motionFile.good())
				{
					throw std::runtime_error("Failed to open '" + motionFilename + "' for writing.");
				}

				auto& buffer = mw.GetBuffer();
				if (opts.csv)
				{
					CsvWriter	mcw(opts.excludes, *animLayer, buffer, mbs.GetChannelCount(), motionFile);
					mcw.Visit(*sdk.mScene->GetRootNode());
				}
				else
				{
					MotionHeader mh{ frameCount, static_cast<float>(opts.targetFrameDelaySeconds), mbs.GetChannelCount() };
					if (!motionFile.write(reinterpret_cast<char*>(&mh), sizeof(mh)))
					{
						throw std::runtime_error("Failed to write header to '" + motionFilename + "'.");
					}

					if (!motionFile.write(reinterpret_cast<char const*>(buffer.data()), buffer.size() * sizeof(float)))
					{
						throw std::runtime_error("Failed to write data to '" + motionFilename + "'.");
					}
				}
			}
		}
	}
}
}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		Usage();
		std::exit(1);
	}

	Options opts(argc, argv);
	if (opts.exitCode != Options::NO_EXIT)
	{
		Usage();
		std::exit(opts.exitCode);
	}

	FbxSdk sdk;
	sdk.LoadScene(opts.inPath.c_str());
	if (!sdk.mScene->GetRootDocument())
	{
		std::exit(1);
	}

	if (opts.outPath.empty())
	{
		opts.outPath = UpDir(opts.inPath);
	}

	PerformProcessing(opts, sdk);

	return 0;
}
