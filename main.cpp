#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <stdexcept>
#if defined(_WIN32)
#include <Windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

struct color_t {
	uint8_t r,g,b;
	color_t(const uint32_t rgb)
	: r((rgb >> 16) & 0xff)
	, g((rgb >> 8) & 0xff)
	, b(rgb & 0xff)
	{}
	color_t(uint8_t r, uint8_t g, uint8_t b)
	: r(r), g(g), b(b) {}
};

enum class StretchRuleVertical : uint8_t {
	Disallowed,  // no stretching allowed
	Allowed, // stretch vertically
	PreserveTop,  // repeat only the lase row so the details in the top of the flag are preserved
	PreserveCenter, // last and first row are repeated to preserve the details in the center of the flag
	PreserveBottom, // repeat only the first row so the details on the bottom side are preserved
};

enum class StretchRuleHorizontal : uint8_t {
	Disallowed,  // no stretching allowed
	Allowed, // stretch horizontally
	PreserveLeft,  // repeat only the last column so the details on the left side are preserved
	PreserveCenter, // last and first column are repeated to preserve the details in the center of the flag
	PreserveRight, // repeat only the first column so the details on the right side are preserved
};

struct flag_t {
	std::variant<std::vector<color_t>, std::vector<std::vector<color_t>>> colors;
	std::string description;
	StretchRuleVertical stretchVertical = StretchRuleVertical::Allowed;
	StretchRuleHorizontal stretchHorizontal = StretchRuleHorizontal::Allowed;
};

enum class colorAdjust : uint8_t {
	none,
	lighten,
	darken
};

std::map<std::string, flag_t const> allFlags = {
	{ "lgbt", {
		// info: https://en.wikipedia.org/wiki/Rainbow_flag_(LGBT)
		// colors: https://en.wikipedia.org/wiki/File:Gay_Pride_Flag.svg
		std::vector<color_t> { 0xE40303, 0xFF8C00, 0xFFED00, 0x008026, 0x004Dff, 0x750787 },
		"Classic 6-color rainbow flag popular since 1979"
	} },

	{ "lgbt-1978", {
		// info: https://en.wikipedia.org/wiki/Rainbow_flag_(LGBT)
		// colors: https://en.wikipedia.org/wiki/File:Gay_flag_8.svg
		std::vector<color_t> { 0xFF69B4, 0xFF0000, 0xFF8E00, 0xFFFF00, 0x008E00, 0x00C0C0, 0x400098, 0x8E008E },
		"Original 8-color rainbow flag designed by Gilbert Baker in 1978"
	} },

	{ "lgbtpoc", {
		// info: https://en.wikipedia.org/wiki/Rainbow_flag_(LGBT)
		// colors: https://en.wikipedia.org/wiki/File:Philadelphia_Pride_Flag.svg
		std::vector<color_t> { 0x000000, 0x784F17, 0xE40303, 0xFF8C00, 0xFFED00, 0x008026, 0x004DFF, 0x750787 },
		"POC-inclusive rainbow flag designed by Philadelphia City Council in 2017"
	} },

	{ "transgender", {
		// info: https://en.wikipedia.org/wiki/Transgender_flags
		// colors: https://en.wikipedia.org/wiki/File:Transgender_Pride_flag.svg
		std::vector<color_t> { 0x5BCEFA, 0xF5A9B8, 0xFFFFFF, 0xF5A9B8, 0x5BCEFA },
		"Transgender pride flag designed by Monica Helms in 1999"
	} },

	{ "bisexual", {
		// info: https://en.wikipedia.org/wiki/Bisexual_pride_flag
		// colors: https://en.wikipedia.org/wiki/File:Bisexual_Pride_Flag.svg
		std::vector<color_t> { 0xD60270, 0xD60270, 0x9B4F96, 0x0038A8, 0x0038A8 },
		"Bisexual pride flag designed by Michael Page in 1998"
	} },

	{ "asexual", {
		// info: https://en.wikipedia.org/wiki/LGBT_symbols#Asexuality
		// colors: https://en.wikipedia.org/wiki/File:Asexual_Pride_Flag.svg
		std::vector<color_t> { 0x000000, 0xA3A3A3, 0xFFFFFF, 0x800080 },
		"Asexual pride flag designed by AVEN user 'standup' in 2010"
	} },

	{ "aromantic", {
		// info/colors: https://cameronwhimsy.tumblr.com/post/75868343112/ive-been-reading-up-on-a-lot-of-the-discussion
		std::vector<color_t> { 0x3DA642, 0xA8D379, 0xFFFFFF, 0xA9A9A9, 0x000000 },
		"Aromantic pride flag designed by Tumblr user 'cameronwhimsy' in 2014"
	} },

	{ "aromantic-asexual", {
		// info: https://www.lgbtqia.wiki/wiki/Aroace
		// colors: https://en.wikipedia.org/wiki/File:Aroace_flag.svg (also available from lgbtqia.wiki, but this is higher quality)
		std::vector<color_t> { 0xE28C00, 0xECCD00, 0xFFFFFF, 0x62AEDC, 0x203856 },
		"Aromantic-asexual pride flag designed by Tumblr user 'aroaesflags' in 2018"
	} },

	{ "pansexual", {
		// info: https://majesticmess.com/2018/12/01/interview-creator-of-the-pan-flag/
		// colors: https://web.archive.org/web/20111103184455/http://pansexualflag.tumblr.com/post/1265215452/hex-color-codes-you-dont-have-to-use-these-exact
		std::vector<color_t> { 0xFF218C, 0xFF218C, 0xFFD800, 0xFFD800, 0x21B1FF, 0x21B1FF },
		"Pansexual pride flag designed by Evie Varney in 2010"
	} },

	{ "nonbinary", {
		// info: https://en.wikipedia.org/wiki/LGBT_symbols#Non-binary
		// colors: https://en.wikipedia.org/wiki/File:Nonbinary_flag.svg
		std::vector<color_t> { 0xFFF430, 0xFFFFFF, 0x9C59D1, 0x000000 },
		"Non-binary pride flag designed by Kye Rowan in 2014"
	} },

	{ "lipstick-lesbian", {
		// info/colors: https://en.wikipedia.org/wiki/File:Lipstick_Lesbian_flag_without_lips.svg
		std::vector<color_t> { 0xA40061, 0xB75592, 0xD063A6, 0xEDEDEB, 0xE4ACCF, 0xC54E54, 0x8A1E04 },
		"Lipstick lesbian pride flag designed by Natalie McCray in 2010"
	} },

	{ "new-lesbian", {
		// info: https://en.wikipedia.org/wiki/LGBT_symbols#Lesbian
		// colors: https://en.wikipedia.org/wiki/File:Lesbian_pride_flag_2018.svg
		// second-last color changed from 0xB55690 to 0xB55590 to ensure distinct colors on non-truecolor displays
		std::vector<color_t> { 0xD52D00, 0xEF7627, 0xFF9A56, 0xFFFFFF, 0xD162A4, 0xB55590, 0xA30262 },
		"New lesbian pride flag designed by Emily Gwen in 2018"
	} },

	{ "community-lesbian", {
		// info/colors: https://majesticmess.com/encyclopedia/lesbian-flag-sadlesbeandisaster/
		// more info: https://twitter.com/lesflagisracist/status/1107301651403157505
		std::vector<color_t> { 0xD52D00, 0xFF9A56, 0xFFFFFF, 0xD362A4, 0xA30262 },
		"5-color 'Community' variant designed by Tumblr user 'taqwomen' in 2018"
	} },

	{ "genderqueer", {
		// info/colors: https://genderqueerid.com/about-flag
		std::vector<color_t> { 0xB57EDC, 0xB57EDC, 0xFFFFFF, 0xFFFFFF, 0x4A8123, 0x4A8123 },
		"Genderqueer pride flag designed by Marilyn Roxie in 2011"
	} },

	{ "progress-pride", {
	    // info/colors: https://de.wikipedia.org/wiki/Datei:LGBTQ+_rainbow_flag_Quasar_%22Progress%22_variant.svg
		std::vector<std::vector<color_t>> {
			{ 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0xee3124, 0xee3124, 0xee3124, 0xee3124, 0xee3124, 0xee3124, 0xee3124 },
			{ 0xffffff, 0xffffff, 0xffffff, 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0xf57e29, 0xf57e29, 0xf57e29, 0xf57e29 },
			{ 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0xffee00 },
			{ 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xffffff, 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0x58b947 },
			{ 0xffffff, 0xffffff, 0xffffff, 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0x0053a6, 0x0053a6, 0x0053a6, 0x0053a6 },
			{ 0xf5a9b8, 0xf5a9b8, 0xf5a9b8, 0x5bcefa, 0x5bcefa, 0x5bcefa, 0x603917, 0x603917, 0x603917, 0x000000, 0x000000, 0x000000, 0x9f248f, 0x9f248f, 0x9f248f, 0x9f248f, 0x9f248f, 0x9f248f, 0x9f248f },
		},
		"Progress pride flag designed by Daniel Quasar in 2018",
		StretchRuleVertical::Allowed,
		StretchRuleHorizontal::PreserveLeft,  // preserves the details on the left side
	}}
};

std::map<std::string, std::string> aliases = {
	{ "trans", "transgender" },
	{ "bi", "bisexual" },
	{ "ace", "asexual" },
	{ "aro", "aromantic" },
	{ "aroace", "aromantic-asexual"},
	{ "pan", "pansexual" },
	{ "nb", "nonbinary" },
	{ "enby", "nonbinary" },
	{ "pink-lesbian", "lipstick-lesbian" },
	{ "lesbian", "community-lesbian" },
	{ "progress", "progress-pride" },
};

flag_t stretch1dFlagTo(const flag_t& flag, const int height) {
	if (std::holds_alternative<std::vector<std::vector<color_t>>>(flag.colors)) {
		printf("WARNING: Method for stretching 1D flags called on 2D flag\n");
		return flag;
	}
	auto stretchedColors = std::get<std::vector<color_t>>(flag.colors);
	switch (flag.stretchVertical) {
		case StretchRuleVertical::Allowed: {
			const int currentHeight = static_cast<int>(stretchedColors.size());
			const std::vector<color_t> unstretchedColors = stretchedColors;
			if (currentHeight >= height) {
				break;
			}
			stretchedColors.clear();
			const double stretchFactor = static_cast<double>(height) / currentHeight;
			for (int i = 0; i < height; ++i) {
				int originalIndex = static_cast<int>(i / stretchFactor);
				if (originalIndex >= currentHeight) {
					originalIndex = currentHeight-1;
				}
				stretchedColors.push_back(unstretchedColors[originalIndex]);
			}
			break;
		}
		case StretchRuleVertical::PreserveTop: {
			while (static_cast<int>(stretchedColors.size()) < height) {
				stretchedColors.push_back(stretchedColors.back());
			}
			break;
		}
		case StretchRuleVertical::PreserveCenter: {
			const int delta_height = height - static_cast<int>(stretchedColors.size());
			for (int i = 0; i < delta_height / 2; ++i) {
				stretchedColors.insert(stretchedColors.begin(), stretchedColors.front());
				stretchedColors.push_back(stretchedColors.back());
			}
			break;
		}
		case StretchRuleVertical::PreserveBottom: {
			while (static_cast<int>(stretchedColors.size()) < height) {
				stretchedColors.insert(stretchedColors.begin(), stretchedColors.front());
			}
			break;
		}
		default:
			break;
	}
	return flag_t {
		.colors = stretchedColors,
		.description = flag.description,
		.stretchVertical = flag.stretchVertical,
		.stretchHorizontal = flag.stretchHorizontal
	};
}

flag_t stretch2dFlagTo(const flag_t& flag, const int width, const int height) {
	if (std::holds_alternative<std::vector<color_t>>(flag.colors)) {
		printf("WARNING: Method for stretching 2D flags called on 1D flag\n");
		return flag;
	}
	auto stretchedColors = std::get<std::vector<std::vector<color_t>>>(flag.colors);
	switch (flag.stretchHorizontal) {
		case StretchRuleHorizontal::Allowed: {
			const int currentWidth = static_cast<int>(stretchedColors[0].size());
			const std::vector<std::vector<color_t>> unstretchedColors = stretchedColors;
			if (currentWidth >= width) {
				break;
			}
			const double stretchFactor = static_cast<double>(width) / currentWidth;
			for (int row = 0; row < static_cast<int>(unstretchedColors.size()); ++row) {
				stretchedColors[row].clear();
				double error = 0.0;
				for (int i = 0; i < width; ++i) {
					const double exactOriginalIndex = i / stretchFactor;
					int originalIndex = static_cast<int>(exactOriginalIndex);
					error += exactOriginalIndex - originalIndex;
					if (error > 1.0) {
						++originalIndex;
						error = 0.0;
					} else if (error < -1.0) {
						--originalIndex;
						error = 0.0;
					}
					if (originalIndex < 0) {
						originalIndex = 0;
					} else if (originalIndex >= currentWidth) {
						originalIndex = currentWidth-1;
					}
					stretchedColors[row].push_back(unstretchedColors[row][originalIndex]);
				}
			}
			break;
		}
		case StretchRuleHorizontal::PreserveLeft: {
			for (auto& row : stretchedColors) {
				while (static_cast<int>(row.size()) < width) {
					row.push_back(row.back());
				}
			}
			break;
		}
		case StretchRuleHorizontal::PreserveCenter: {
			const int delta_width = width - static_cast<int>(stretchedColors[0].size());
			for (auto& row : stretchedColors) {
				for (int i = 0; i < delta_width / 2; ++i) {
					row.insert(row.begin(), row.front());
					row.push_back(row.back());
				}
			}
			break;
		}
		case StretchRuleHorizontal::PreserveRight: {
			for (auto& row : stretchedColors) {
				while (static_cast<int>(row.size()) < width) {
					row.insert(row.begin(), row.front());
				}
			}
			break;
		}
		default:
			break;
	}
	switch (flag.stretchVertical) {
		case StretchRuleVertical::Allowed: {
			const int currentHeight = static_cast<int>(stretchedColors.size());
			const std::vector<std::vector<color_t>> unstretchedColors = stretchedColors;
			if (currentHeight >= height) {
				break;
			}
			stretchedColors.clear();
			const double stretchFactor = static_cast<double>(height) / currentHeight;
			for (int i = 0; i < height; ++i) {
				int originalIndex = static_cast<int>(i / stretchFactor);
				if (originalIndex >= currentHeight) {
					originalIndex = currentHeight-1;
				}
				stretchedColors.insert(stretchedColors.begin() + i, unstretchedColors[originalIndex]);
			}
			break;
		}
		case StretchRuleVertical::PreserveTop: {
			while (static_cast<int>(stretchedColors.size()) < height) {
				stretchedColors.push_back(stretchedColors.back());
			}
			break;
		}
		case StretchRuleVertical::PreserveCenter: {
			const int delta_height = height - static_cast<int>(stretchedColors.size());
			for (int i = 0; i < delta_height / 2; ++i) {
				stretchedColors.insert(stretchedColors.begin(), stretchedColors.front());
				stretchedColors.push_back(stretchedColors.back());
			}
			break;
		}
		case StretchRuleVertical::PreserveBottom: {
			while (static_cast<int>(stretchedColors.size()) < height) {
				stretchedColors.insert(stretchedColors.begin(), stretchedColors.front());
			}
			break;
		}
		default:
			break;
	}
	return flag_t {
		.colors = stretchedColors,
		.description = flag.description,
		.stretchVertical = flag.stretchVertical,
		.stretchHorizontal = flag.stretchHorizontal
	};
}

std::vector<color_t> g_colorQueue;
std::vector<std::string> g_filesToCat;
unsigned int g_currentRow = 0;
unsigned int g_currentColumn = 0;
bool two_dimensiona_flag = false;
flag_t current2dFlag;
auto g_colorAdjustment = colorAdjust::none;

int stretchToHeight = 0;

#if defined(_WIN32)
bool g_useColors = _isatty(_fileno(stdout));
bool g_trueColor = true;
#else
bool isTrueColorTerminal() {
	char const* ct = getenv("COLORTERM");
	return ct ? strstr(ct, "truecolor") || strstr(ct, "24bit") : false;
}
bool g_useColors = isatty(STDOUT_FILENO);
bool g_trueColor = isTrueColorTerminal();
#endif
bool g_setBackgroundColor = false;


bool strEqual(char const* a, char const* b) {
	return strcmp(a, b) == 0;
}

bool startsWith(char const* a, char const* b) {
	int const length = strlen(b);
	for (int i = 0; i < length; ++i) {
		if (a[i] != b[i]) {
			return false;
		}
	}
	return true;
}

int bestNonTruecolorMatch(color_t const& color) {
	/*
	per wikipedia:
	    0-  7:  standard colors (as in ESC [ 30–37 m)
	    8- 15:  high intensity colors (as in ESC [ 90–97 m)
	->  16-231:  6 × 6 × 6 cube (216 colors): 16 + 36 × r + 6 × g + b (0 ≤ r, g, b ≤ 5)
	    232-255:  grayscale from black to white in 24 steps
	*/
	int const r = (color.r*6)/256;
	int const g = (color.g*6)/256;
	int const b = (color.b*6)/256;
	return 16 + (36*r) + (6*g) + b;
}

void pushFlag(flag_t const& flag) {
	if (std::holds_alternative<std::vector<color_t>>(flag.colors)) {
		std::vector<color_t> colors;
		if (stretchToHeight != 0) {
			const auto& stretchedFlag = stretch1dFlagTo(flag, stretchToHeight);
			colors = std::get<std::vector<color_t>>(stretchedFlag.colors);
		} else {
			colors = std::get<std::vector<color_t>>(flag.colors);
		}
		for (color_t const& color : colors) {
			g_colorQueue.push_back(color);
		}
	} else {
		two_dimensiona_flag = true;
		if (stretchToHeight != 0) {
			current2dFlag = stretch2dFlagTo(flag, 0, stretchToHeight);
		} else {
			current2dFlag = flag;
		}
	}
}

std::string resolveAlias(const std::string& arg) {
	if (const auto& alias = aliases.find(arg); alias != aliases.end()) {
		return alias->second;
	}
	return arg;
}

color_t adjustForReadability(color_t const& color) {
	if (g_colorAdjustment == colorAdjust::darken) {
		return color_t( // NOLINT(*-return-braced-init-list)
			(color.r*3)/4,
			(color.g*3)/4,
			(color.b*3)/4
		);
	} else if (g_colorAdjustment == colorAdjust::lighten) {
		return color_t( // NOLINT(*-return-braced-init-list)
			64+(color.r*3)/4,
			64+(color.g*3)/4,
			64+(color.b*3)/4
		);
	} else {
		return color;
	}
}

void setTextColor(color_t const& color) {
	if (!g_useColors)
		return;

	color_t const readableColor = adjustForReadability(color);

	if (g_trueColor) {
		fprintf(stdout, "\033[38;2;%d;%d;%dm", readableColor.r, readableColor.g, readableColor.b);
	} else {
		fprintf(stdout, "\033[38;5;%dm", bestNonTruecolorMatch(readableColor));
	}
}

void setBackgroundColor(color_t const& color) {
	if (!g_useColors)
		return;

	color_t const readableColor = adjustForReadability(color);

	if (g_trueColor) {
		fprintf(stdout, "\033[48;2;%d;%d;%dm", readableColor.r, readableColor.g, readableColor.b);
	} else {
		fprintf(stdout, "\033[48;5;%dm", bestNonTruecolorMatch(readableColor));
	}
}

void resetTextColor() {
	if (!g_useColors)
		return;

	fputs("\033[39m", stdout);
}

void resetBackgroundColor() {
	if (!g_useColors)
		return;

	fputs("\033[49m", stdout);
}

void setColor(color_t const& color) {
	if (g_setBackgroundColor) {
		setBackgroundColor(color);
	} else {
		setTextColor(color);
	}
}

void resetColor() {
	if (g_setBackgroundColor) {
		resetBackgroundColor();
	} else {
		resetTextColor();
	}
}

void parseCommandLine(const int argc, char** argv) {
	bool finishedReadingFlags = false;
	for (int i = 1; i < argc; ++i) {
		if (finishedReadingFlags) {
			g_filesToCat.emplace_back(argv[i]);
		}
		else if (strEqual(argv[i], "-s") || strEqual(argv[i], "--stretch")) {
			if (i + 1 < argc) {
				try {
					stretchToHeight = std::stoi(argv[++i]);
				} catch (std::invalid_argument&) {
					fprintf(stderr, "pridecat: Invalid height '%s'\n", argv[i]);
					exit(1);
				}
			} else {
				fprintf(stderr, "pridecat: Expected an argument after %s\n", argv[i]);
				exit(1);
			}
		}
		else if (strEqual(argv[i], "-h") || strEqual(argv[i], "--help")) {
			printf("pridecat!\n");
			printf("It's like cat but more colorful :)\n");

			printf("\nCurrently available flags:\n");
			for (const auto&[name, flag] : allFlags) {
				printf("  --%s", name.c_str());
				for (const auto&[short_name, long_name] : aliases) {
					if (name == long_name) {
						printf(",--%s", short_name.c_str());
					}
				}
				if (g_useColors) {
					putc(' ', stdout);
					if (std::holds_alternative<std::vector<color_t>>(flag.colors)) {
						for (const color_t color : std::get<std::vector<color_t>>(flag.colors)) {
							setBackgroundColor(color);
							putc(' ', stdout);
						}
					} else {
						auto& colors = std::get<std::vector<std::vector<color_t>>>(flag.colors);
						flag_t newFlag = flag;
						if (colors[0].size() < 30) {
							newFlag.colors = stretch2dFlagTo(flag, 30, 6).colors;
						}
						for (const std::vector<color_t>& color_row : std::get<std::vector<std::vector<color_t>>>(newFlag.colors)) {
							printf("\n      ");
							for (const color_t color : color_row) {
								setBackgroundColor(color);
								putc(' ', stdout);
							}
							resetBackgroundColor();
						}
					}
					resetBackgroundColor();
				}
				printf("\n");
				printf("      %s\n\n", flag.description.c_str());
			}

			printf("Additional options:\n");
			printf("  -b,--background\n");
			printf("      Change the background color instead of the text color\n\n");
			printf("  -f,--force\n");
			printf("      Force color even when stdout is not a tty\n\n");
			printf("  -t,--truecolor\n");
			printf("      Force truecolor output (even if the terminal doesn't seem to support it)\n\n");
			printf("  -T,--no-truecolor\n");
			printf("      Force disable truecolor output (even if the terminal does seem to support it)\n\n");
			printf("  -l,--lighten\n");
			printf("      Lighten colors slightly for improved readability on dark backgrounds\n\n");
			printf("  -d,--darken\n");
			printf("      Darken colors slightly for improved readability on light backgrounds\n\n");
			printf("  -s,--stretch <height>\n");
			printf("      Stretch the flag to a certain height before repeating\n\n");
			printf("  -h,--help\n");
			printf("      Display this message\n\n");

			printf("Examples:\n");
			printf("  pridecat f - g          Output f's contents, then stdin, then g's contents.\n");
			printf("  pridecat                Copy stdin to stdout, but with rainbows.\n");
			printf("  pridecat --trans --bi   Alternate between trans and bisexual pride flags.\n");
			exit(0);
		}
		else if (strEqual(argv[i], "-f") || strEqual(argv[i], "--force")) {
			g_useColors = true;
		}
		else if (strEqual(argv[i], "-t") || strEqual(argv[i], "--truecolor")) {
			g_trueColor = true;
		}
		else if (strEqual(argv[i], "-T") || strEqual(argv[i], "--no-truecolor")) {
			g_trueColor = false;
		}
		else if (strEqual(argv[i], "-b") || strEqual(argv[i], "--background")) {
			g_setBackgroundColor = true;
		}
		else if (strEqual(argv[i], "-l") || strEqual(argv[i], "--lighten")) {
			g_colorAdjustment = colorAdjust::lighten;
		}
		else if (strEqual(argv[i], "-d") || strEqual(argv[i], "--darken")) {
			g_colorAdjustment = colorAdjust::darken;
		}
		else if (strEqual(argv[i], "--")) {
			finishedReadingFlags = true;
		}
		else if (startsWith(argv[i], "--")) {
			std::string const flagName = resolveAlias(argv[i]+2);
			const auto& flag = allFlags.find(flagName);
			if (flag != allFlags.end()) {
				pushFlag(flag->second);
			} else {
				fprintf(stderr, "pridecat: Unknown flag '%s'\n", argv[i]);
				exit(1);
			}
		}
		else if (strEqual(argv[i], "-")) {
			// use an empty string in the array to represent stdin
			// so we can still actually have a file called '-'
			g_filesToCat.emplace_back("");
		}
		else {
			g_filesToCat.emplace_back(argv[i]);
		}
	}
}

void abortHandler(int signo) {
	resetColor();
	exit(signo);
}

void catFile2d(FILE* fh) {
	int longestLine = 0;
	char* line = nullptr;
	size_t len = 0;
	ssize_t read;

	while ((read = getline(&line, &len, fh)) != -1) {
		if (read > longestLine) {
			longestLine = static_cast<int>(read);
		}
	}
	fseek(fh, 0, SEEK_SET);

	const int flagHeight = static_cast<int>(std::get<std::vector<std::vector<color_t>>>(current2dFlag.colors).size());
	const auto flag = stretch2dFlagTo(current2dFlag, longestLine, flagHeight);
	const auto& colors = std::get<std::vector<std::vector<color_t>>>(flag.colors);


	int c;
	while ((c = getc(fh)) >= 0) {
		if (c == '\n') {
			g_currentRow++;
			g_currentColumn = 0;
			if (g_currentRow == colors.size()) {
				g_currentRow = 0;
			}
			resetColor();
			putc(c, stdout);
		} else {
			setColor(colors[g_currentRow][g_currentColumn]);
			putc(c, stdout);
			resetColor();
			g_currentColumn++;
		}
	}

	if (line) {
		free(line);
	}
}

void catFile(FILE* fh) {
	if (two_dimensiona_flag) {
		catFile2d(fh);
	}
	int c;
	while ((c = getc(fh)) >= 0) {
		if (c == '\n') {
			resetColor();
		}
		putc(c, stdout);
		if (c == '\n') {
			g_currentRow++;
			if (g_currentRow == g_colorQueue.size()) {
				g_currentRow = 0;
			}
			setColor(g_colorQueue[g_currentRow]);
		}
	}
}

#if defined(_WIN32)
bool tryEnableEscapeSequences()
{
	HANDLE const hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) {
		return false;
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(hOut, &dwMode)) {
		return false;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(hOut, dwMode)) {
		return false;
	}

	return true;
}
#endif

int main(int argc, char** argv) {
	signal(SIGINT, abortHandler);

#if defined(_WIN32)
	if (!tryEnableEscapeSequences()) {
		g_useColors = false;
	}
#endif

	parseCommandLine(argc, argv);

	if (g_colorQueue.empty()) {
		pushFlag(allFlags.at("lgbt"));
	}

	setColor(g_colorQueue[0]);

	if (g_filesToCat.empty()) {
		catFile(stdin);
	} else {
		for (auto const& filepath : g_filesToCat) {
			if (filepath.empty()) {
				catFile(stdin);
			} else {
				FILE* fh = fopen(filepath.c_str(), "rb");
				if (!fh) {
					fprintf(
						stderr,
						"pridecat: Could not open %s for reading.\n",
						filepath.c_str()
					);
					return 1;
				}
				catFile(fh);
				fclose(fh);
			}
		}
	}

	resetColor();

	return 0;
}
