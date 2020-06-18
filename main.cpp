#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <signal.h>
#if defined(_WIN32)
#include <Windows.h>
#include <io.h>
#else
#include <getopt.h>
#include <unistd.h>
#endif
#include <fstream>
#include <iostream>

#define array_size(x) (sizeof(x)/sizeof(x[0]))

struct color_t {
	uint8_t r,g,b;
	color_t(uint32_t rgb)
	: r((rgb >> 16) & 0xff)
	, g((rgb >> 8) & 0xff)
	, b(rgb & 0xff)
	{}
};

struct flag_t {
	std::vector<color_t> colors;
	std::string description;
};

std::map<std::string, flag_t const> allFlags = {
	{ "lgbt", { 
		// colors from https://www.schemecolor.com/lgbt-flag-colors.php
		{ 0xFF0018,0xFFA52C,0xFFFF41,0x008018,0x0000F9,0x86007D }, 
		"Classic 6-color rainbow flag popular since 1979"
	} },

	{ "lgbt-1978", { 
		// colors from https://en.wikipedia.org/wiki/File:Gay_flag_8.svg
		{ 0xFF69B4,0xFF0000,0xFF8E00,0xFFFF00,0x008E00,0x00C0C0,0x400098,0x8E008E },
		"Original 8-color rainbow flag designed by Gilbert Baker in 1978"
	} },

	{ "lgbtpoc", {
		// colors from https://en.wikipedia.org/wiki/File:Philadelphia_Pride_Flag.svg
		{ 0x000000,0x784F17,0xE40303,0xFF8C00,0xFFED00,0x008026,0x004DFF,0x750787 },
		"POC-inclusive rainbow flag designed by Philadelphia City Council in 2017"
	} },

	{ "transgender", { 
		// colors from https://www.schemecolor.com/transgender-pride-flag-colors.php
		{ 0x55CDFC,0xF7A8B8,0xFFFFFF,0xF7A8B8,0x55CDFC },
		"Transgender pride flag designed by Monica Helms in 1999"
	} },

	{ "bisexual", {
		// colors from https://www.schemecolor.com/bisexuality-flag-colors.php
		{ 0xD60270,0xD60270,0x9B4F96,0x0038A8,0x0038A8 },
		"Bisexual pride flag designed by Michael Page in 1998"
	} },

	{ "asexual", {
		// colors from https://www.schemecolor.com/asexual-pride-flag-colors.php
		{ 0x000000,0xA4A4A4,0xFFFFFF,0x810081 },
		"Asexual pride flag designed by AVEN user 'standup' in 2010"
	} },

	{ "pansexual", {
		// colors from https://www.schemecolor.com/pansexuality-flag-colors.php
		{ 0xFF1B8D,0xFF1B8D,0xFFDA00,0xFFDA00,0x1BB3FF,0x1BB3FF, },
		"Pansexual pride flag designed by Evie Varney in 2010"
	} },

	{ "nonbinary", {
		// colors from https://www.schemecolor.com/non-binary-gender-flag-colors.php
		{ 0xFFF430,0xFFFFFF,0x9C59D1,0x000000 },
		"Non-binary pride flag designed by Kye Rowan in 2014"
	} },

	{ "lipstick-lesbian", {
		// colors from https://en.wikipedia.org/wiki/File:Lipstick_Lesbian_flag_without_lips.svg
		{ 0xA40061,0xB75592,0xD063A6,0xEDEDEB,0xE4ACCF,0xC54E54,0x8A1E04 },
		"Lipstick lesbian pride flag designed by Natalie McCray in 2010"
	} },

	{ "new-lesbian", {
		// colors from https://en.wikipedia.org/wiki/File:Lesbian_pride_flag_2018.svg
		{ 0xD52D00,0xEF7627,0xFF9A56,0xFFFFFF,0xD162A4,0xB55690,0xA30262 },
		"New lesbian pride flag designed by Emily Gwen in 2018"
	} },

	{ "genderqueer", {
		// colors from https://genderqueerid.com/about-flag
		{ 0xB57EDC,0xB57EDC,0xFFFFFF,0xFFFFFF,0x4A8123,0x4A8123, },
		"Genderqueer pride flag designed by Marilyn Roxie in 2011"
	} },
};

std::map<std::string, std::string> aliases = {
	{ "trans", "transgender" },
	{ "bi", "bisexual" },
	{ "ace", "asexual" },
	{ "pan", "pansexual" },
	{ "nb", "nonbinary" },
	{ "enby", "nonbinary" },
	{ "pink-lesbian", "lipstick-lesbian" },
};

std::vector<color_t> g_colorQueue;
std::vector<std::string> g_filesToCat;
unsigned int g_currentRow = 0;

#if defined(_WIN32)
bool g_useColors = _isatty(_fileno(stdout));
bool g_trueColor = true;
#else
bool g_useColors = isatty(STDOUT_FILENO);
bool g_trueColor = getenv("COLORTERM");
#endif

bool g_numberLines = false;
bool g_showEnds = false;
bool g_squeezeEmpty = false;
bool g_numberNonblank = false;
unsigned int g_blankLines = 0;

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
	int const r = (color.r / 51);
	int const g = (color.g / 51);
	int const b = (color.b / 51);
	return 16 + (36*r) + (6*g) + b;
}

void pushFlag(flag_t const& flag) {
	for (color_t const& color : flag.colors) {
		g_colorQueue.push_back(color);
	}
}

std::string resolveAlias(const std::string& arg) {
	const auto& alias = aliases.find(arg);
	if (alias != aliases.end()) {
		return alias->second;
	}
	return arg;
}

void setColor(color_t const& color) {
	if (!g_useColors)
		return;

	if (g_trueColor) {
		fprintf(stdout, "\033[38;2;%d;%d;%dm", color.r, color.g, color.b);
	} else {
		// apparently the default macOS Terminal.app still needs this?? (as of 10.14 Mojave)
		fprintf(stdout, "\033[38;5;%dm", bestNonTruecolorMatch(color));
	}
}

void resetColor() {
	if (g_useColors) {
		fputs("\033[39m", stdout);
	}
}

void showUsage() {
	printf("pridecat!\n");
	printf("It's like cat but more colorful :)\n");
	
	printf("\nCurrently available flags:\n");
	for (const auto& flag : allFlags) {
		printf("  --%s", flag.first.c_str());
		for (const auto& alias : aliases) {
			if (flag.first == alias.second) {
				printf(",--%s", alias.first.c_str());
			}
		}
		if (g_useColors) {
			printf(" ");
			for (const auto& color : flag.second.colors) {
				setColor(color);
				printf("█");
			}
			resetColor();
		}
		printf("\n");
		printf("      %s\n\n", flag.second.description.c_str());
	}
	
	printf("Additional options:\n");
	printf("  -f,--force\n");
	printf("      Force color even when stdout is not a tty\n\n");
	printf("  -t,--truecolor\n");
	printf("      Force truecolor output (even if the terminal doesn't seem to support it)\n\n");
	printf("  -h,--help\n");
	printf("      Display this message\n\n");
	printf("  -n,--number\n");
	printf("      number all output lines\n\n");
	printf("  -E,--show-ends\n");
	printf("      display $ at the end of each line\n\n");
	printf("  -s,--squeeze-blank\n");
	printf("      suppress repeated empty output lines\n\n");
	printf("  -b,--number-nonblank\n");
	printf("      number nonempty output lines, overrides -n\n\n");
	
	printf("Examples:\n");
	printf("  pridecat f - g          Output f's contents, then stdin, then g's contents.\n");
	printf("  pridecat                Copy stdin to stdout, but with rainbows.\n");
	printf("  pridecat --trans --bi   Alternate between trans and bisexual pride flags.\n");
}

void parseCommandLine(int argc, char** argv) {
	bool finishedReadingFlags = false;
	for (int i = 1; i < argc; ++i) {
		if (finishedReadingFlags) {
			g_filesToCat.push_back(argv[i]);
		}
		else if (strEqual(argv[i], "-h") || strEqual(argv[i], "--help")) {
			showUsage();
			exit(0);
		}
		else if (strEqual(argv[i], "-f") || strEqual(argv[i], "--force")) {
			g_useColors = true;
		}
		else if (strEqual(argv[i], "-t") || strEqual(argv[i], "--truecolor")) {
			g_trueColor = true;
		}
		else if (strEqual(argv[i], "--number")) {
			g_numberLines = true;
		}
		else if (strEqual(argv[i], "--number-nonblank")) {
			g_numberLines = true;
			g_numberNonblank = true;
		}
		else if (strEqual(argv[i], "--show-ends")) {
			g_showEnds = true;
		}
		else if (strEqual(argv[i], "--squeeze-blank")) {
			g_squeezeEmpty = true;
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
		else if (startsWith(argv[i], "-")) {
			bool unknownChar = false;
			for (char* c = &(argv[i][1]); *c != '\0'; c++) {
				switch(*c) {
					case 'h':
						showUsage();
						exit(0);
						break;
					case 'f':
						g_useColors = true;
						break;
					case 't':
						g_trueColor = true;
						break;
					case 'b':
						g_numberNonblank = true;
						g_numberLines = true;
						break;
					case 'n':
						g_numberLines = true;
						break;
					case 'E':
						g_showEnds = true;
						break;
					case 's':
						g_squeezeEmpty = true;
						break;
					default:
						fprintf(stderr, "unknown flag: '%c'\n", *c);
						unknownChar = true;
						break;
				}
			}
			if (unknownChar) {
				showUsage();
				exit(1);
			}
		}
		else if (strEqual(argv[i], "-")) {
			// use an empty string in the array to represent stdin
			// so we can still actually have a file called '-'
			g_filesToCat.push_back("");
		}
		else {
			g_filesToCat.push_back(argv[i]);
		}
	}
}

void abortHandler(int signo) {
	resetColor();
	exit(signo);
}

void catFile(std::istream& fh) {
	std::string line;
	bool repeatedEmpty = false;
	while (std::getline(fh, line)) {
		if (g_squeezeEmpty and repeatedEmpty and line.empty()) {
			continue;
		}
		else if (line.empty()) {
			repeatedEmpty = true;
			g_blankLines++;
		}
		else {
			repeatedEmpty = false;
		}
		g_currentRow++;
		setColor(g_colorQueue[g_currentRow % g_colorQueue.size()]);
		if (g_numberLines) {
			unsigned int actualLineNumber = g_currentRow;
			if (g_numberNonblank) {
				actualLineNumber -= g_blankLines;
			}
			if (not (g_numberNonblank and line.empty())) {
				// line numbers in cat are 1 indexed
				fprintf(stdout, "%6u  ", actualLineNumber);
			}
		}
		fprintf(stdout, "%s", line.c_str());
		if (g_showEnds) {
			fprintf(stdout, "$");
		}
		fprintf(stdout, "\n");
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
		catFile(std::cin);
	} else {
		for (auto const& filepath : g_filesToCat) {
			if (filepath == "") {
				catFile(std::cin);
			} else {
				std::ifstream fh(filepath.c_str(), std::ifstream::in | std::ifstream::binary);
				if (!fh) {
					fprintf(
						stderr,
						"pridecat: Could not open %s for reading.\n",
						filepath.c_str()
					);
					return 1;
				}
				catFile(fh);
			}
		}
	}

	resetColor();

	return 0;
}
