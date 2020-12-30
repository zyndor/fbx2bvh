#pragma once

#include <string>
#include <vector>

struct Options
{
	enum ExitCode
	{
		NO_EXIT = -1,
		PRINT_USAGE,
		ERROR
	};

	static constexpr uint32_t DEFAULT_FPS = 24;

	std::string inPath;
	std::string outPath;
	std::vector<std::string> excludes;
	bool csv = false;
	bool dry = false;
	double frameDelaySeconds = 1. / DEFAULT_FPS;
	double targetFrameDelaySeconds = frameDelaySeconds;
	ExitCode exitCode = NO_EXIT;

	Options(int argc, char** argv);

private:
	enum Mode
	{
		INPUT,
		OUTPUT,
		EXCLUDE,
		FRAME_DELAY_MS,
		TARGET_FRAME_DELAY_MS,
		INVALID
	};

	Mode mode = INPUT;
};