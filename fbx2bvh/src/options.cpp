#include "options.h"
#include <iostream>

namespace
{

bool ReadNumber(const char* string, const char* name, double& value)
{
	bool result = (sscanf(string, "%lf", &value) == 1);
	if (!result)
	{
		std::cerr << "Invalid " << name << " definition '" << string << "'" << std::endl;
	}
	return result;
}

}

Options::Options(int argc, char ** argv)
{
	bool targetFrameDelaySet = false;
	for (auto i = argv + 1, i1 = argv + argc; i < i1; ++i)
	{
		if (*i && **i == '-')	// mode switch
		{
			switch ((*i)[1])
			{
			case 'e':
				mode = EXCLUDE;
				continue;

			case 'i':
				mode = INPUT;
				continue;

			case 'o':
				mode = OUTPUT;
				continue;

			case 'f':
				mode = FRAME_DELAY_MS;
				continue;

			case 't':
				mode = TARGET_FRAME_DELAY_MS;
				continue;

			case 'c':
				csv = true;
				continue;

			case 'd':
				dry = true;
				continue;

			case 'h':
				exitCode = PRINT_USAGE;
				return;

			default:
				exitCode = ERROR;
				return;
			}
		}

		switch (mode)
		{
		case INVALID:
			exitCode = ERROR;
			return;

		case INPUT:
			inPath = *i;
			mode = outPath.empty() ? OUTPUT : INVALID;
			break;

		case OUTPUT:
			outPath = *i;
			mode = inPath.empty() ? INPUT : INVALID;
			break;

		case EXCLUDE:
			if (*i)
			{
				auto c = *i;
				do
				{
					auto cEnd = c + strcspn(c, ":");
					excludes.push_back(std::string(c, cEnd));
					c = cEnd;
					if (!*c)
					{
						break;
					}
					++c;
				} while (true);
			}
			mode = inPath.empty() ? INPUT : INVALID;
			break;

		case FRAME_DELAY_MS:
			if (ReadNumber(*i, "frame delay", frameDelaySeconds))
			{
				frameDelaySeconds /= 1000.;	// read milliseconds
				mode = INVALID;
				break;
			}
			else
			{
				exitCode = ERROR;
				return;
			}

		case TARGET_FRAME_DELAY_MS:
			if (ReadNumber(*i, "target frame delay", targetFrameDelaySeconds))
			{
				targetFrameDelaySeconds /= 1000.;	// read milliseconds
				targetFrameDelaySet = true;
				mode = INVALID;
				break;
			}
			else
			{
				exitCode = ERROR;
				return;
			}
		}
	}

	if (!targetFrameDelaySet)
	{
		targetFrameDelaySeconds = frameDelaySeconds;
	}
}
