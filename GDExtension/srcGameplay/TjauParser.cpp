#include "TjauParser.hpp"

#include "../src/GraphiteGlobals.hpp"
#include "Barline.hpp"
#include "BigBlueNote.hpp"
#include "BigGhostNote.hpp"
#include "BigGreenNote.hpp"
#include "BigRedNote.hpp"
#include "BigYellowNote.hpp"
#include "BlueNote.hpp"
#include "Fraction.hpp"
#include "GhostNote.hpp"
#include "GreenNote.hpp"
#include "RedNote.hpp"
#include "YellowNote.hpp"

#include <cctype>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// -------------------------------- Utilities --------------------------------

static std::string Trim(const std::string& s)
{
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos)
        return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static std::string StripComment(const std::string& s)
{
    size_t p = s.find("//");
    return (p != std::string::npos) ? s.substr(0, p) : s;
}

static std::string ToUpper(const std::string& s)
{
    std::string r = s;
    for (size_t i = 0; i < r.size(); i++)
        r[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(r[i])));
    return r;
}

static bool StartsWithCI(const std::string& s, const std::string& p)
{
    if (s.size() < p.size())
        return false;
    for (size_t i = 0; i < p.size(); i++)
    {
        if (std::toupper(static_cast<unsigned char>(s[i])) != std::toupper(static_cast<unsigned char>(p[i])))
            return false;
    }
    return true;
}

static std::vector<std::string> SplitLines(const std::string& content)
{
    std::vector<std::string> lines;
    std::string cur;
    for (size_t i = 0; i < content.size(); i++)
    {
        char c = content[i];
        if (c == '\n')
        {
            lines.push_back(cur);
            cur.clear();
        }
        else if (c != '\r')
        {
            cur += c;
        }
    }
    if (!cur.empty())
        lines.push_back(cur);
    return lines;
}

static bool ParseFraction(const std::string& s, Fraction& result)
{
    bool success = false;
    result.assignFromString(s, success);
    return success;
}

static bool TryParseInt(const std::string& s, int& value)
{
    if (s.empty())
        return false;
    errno = 0;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (errno != 0 || end != s.c_str() + s.size())
        return false;
    if (v < INT32_MIN || v > INT32_MAX)
        return false;
    value = static_cast<int>(v);
    return true;
}

static int ToInt(const std::string& s, int fallback)
{
    int v = 0;
    return TryParseInt(s, v) ? v : fallback;
}

static int ParseCourseName(const std::string& name)
{
    std::string n = ToUpper(Trim(name));
    if (n == "EASY" || n == "0")
        return 0;
    if (n == "NORMAL" || n == "1")
        return 1;
    if (n == "HARD" || n == "2")
        return 2;
    if (n == "ONI" || n == "3")
        return 3;
    if (n == "EDIT" || n == "4" || n == "URA")
        return 4;
    return -1;
}

static bool SplitKeyValue(const std::string& line, std::string& key, std::string& val)
{
    size_t colon = line.find(':');
    if (colon == std::string::npos)
    {
        key = "";
        val = "";
        return false;
    }
    key = ToUpper(Trim(line.substr(0, colon)));
    val = Trim(line.substr(colon + 1));
    return true;
}

static void ParseCommandLine(const std::string& line, std::string& cmd, std::string& arg)
{
    std::string body = line.substr(1);
    size_t i = 0;
    while (i < body.size() && (std::isalpha(static_cast<unsigned char>(body[i])) || std::isdigit(static_cast<unsigned char>(body[i]))))
        i++;
    cmd = ToUpper(body.substr(0, i));
    std::string rest = body.substr(i);
    size_t j = 0;
    while (j < rest.size() && (rest[j] == ':' || std::isspace(static_cast<unsigned char>(rest[j]))))
        j++;
    arg = Trim(rest.substr(j));
}

static Fraction& ZeroFraction()
{
    static Fraction zero;
    return zero;
}

static int64_t FractionToPicoseconds(const Fraction& f)
{
    Fraction ps;
    ps.assignFromUInt64(1000000000000ULL);
    Fraction copy = f;
    copy.multiply(ps);
    bool ok = false;
    return copy.toInt(ok);
}

static double FractionToDouble(const Fraction& f)
{
    Fraction copy = f;
    bool ok = false;
    return copy.toDouble(ok);
}

// --------------------------------- Model ---------------------------------

struct GlobalMeta
{
    std::string title;
    std::string subtitle;
    std::string titleJa;
    std::string titleZh;
    std::string subtitleJa;
    std::string subtitleZh;
    Fraction bpm;
    Fraction offset;
    std::string wave;
    int level;

    GlobalMeta() : level(0)
    {
        bpm.assignFromUInt64(120);
    }
};

struct Token
{
    bool isCommand;
    std::string cmd;
    std::string arg;
    char noteChar;

    Token(bool isCmd, const std::string& c, const std::string& a, char nc) : isCommand(isCmd), cmd(c), arg(a), noteChar(nc)
    {
    }
};

struct PendingNote
{
    bool active;
    char type;
    Fraction startTime;
    Fraction bpm;
    int greenNoteHits;
    double scrollMultiplier;

    PendingNote() : active(false), type('0'), greenNoteHits(0), scrollMultiplier(1.0)
    {
    }
};

struct ParserState
{
    Fraction bpm;
    int measureNum;
    int measureDen;
    double scroll;
    bool gogo;
    bool barlineVisible;
    Fraction time;
    int measureIndex;
    std::vector<int> greenNoteValues;
    int greenNoteIndex;
    PendingNote pendingNote;

    ParserState() : measureNum(4), measureDen(4), scroll(1.0), gogo(false), barlineVisible(true), measureIndex(0), greenNoteIndex(0)
    {
        bpm.assignFromUInt64(120);
    }
};

static void ApplyCommand(ParserState& st, const std::string& cmd, const std::string& arg)
{
    if (cmd == "BPMCHANGE")
    {
        Fraction v;
        if (ParseFraction(arg, v) && v.greaterThan(ZeroFraction()))
            st.bpm = v;
    }
    else if (cmd == "MEASURE")
    {
        size_t slash = arg.find('/');
        if (slash != std::string::npos)
        {
            int n = ToInt(Trim(arg.substr(0, slash)), st.measureNum);
            int d = ToInt(Trim(arg.substr(slash + 1)), st.measureDen);
            if (n > 0 && d > 0)
            {
                st.measureNum = n;
                st.measureDen = d;
            }
        }
    }
    else if (cmd == "SCROLL")
    {
        errno = 0;
        char* end = nullptr;
        double scrollVal = std::strtod(arg.c_str(), &end);
        if (errno == 0 && end == arg.c_str() + arg.size())
            st.scroll = scrollVal;
    }
    else if (cmd == "GOGOSTART")
        st.gogo = true;
    else if (cmd == "GOGOEND")
        st.gogo = false;
    else if (cmd == "BARLINEON")
        st.barlineVisible = true;
    else if (cmd == "BARLINEOFF")
        st.barlineVisible = false;
    else if (cmd == "DELAY")
    {
        Fraction v;
        if (ParseFraction(arg, v))
            st.time.add(v);
    }
}

static Fraction MeasureDurationSeconds(ParserState& st, int divisions)
{
    Fraction beats;
    beats.assignFromInt(4);

    Fraction measureNumF;
    measureNumF.assignFromInt(st.measureNum);
    beats.multiply(measureNumF);

    Fraction measureDenF;
    measureDenF.assignFromInt(st.measureDen);
    beats.divide(measureDenF);

    if (divisions > 0)
    {
        Fraction divF;
        divF.assignFromInt(divisions);
        beats.divide(divF);
    }

    if (st.bpm.greaterThan(ZeroFraction()))
    {
        Fraction sixty;
        sixty.assignFromInt(60);
        Fraction bpmCopy = st.bpm;
        sixty.divide(bpmCopy);
        beats.multiply(sixty);
    }
    return beats;
}

static void AddNoteToCourse(Course& course, char type, const Fraction& startTimeSec, const Fraction& stopTimeSec, const Fraction& bpm, int greenNoteHits, double scrollMultiplier)
{
    int64_t startPs = FractionToPicoseconds(startTimeSec);
    int64_t stopPs = FractionToPicoseconds(stopTimeSec);
    double scrollBpm = FractionToDouble(bpm);

    if (GraphiteGlobals::playbackRate != 1.0)
    {
        startPs = static_cast<int64_t>(static_cast<double>(startPs) / GraphiteGlobals::playbackRate);
        stopPs = static_cast<int64_t>(static_cast<double>(stopPs) / GraphiteGlobals::playbackRate);
        scrollBpm *= GraphiteGlobals::playbackRate;
    }

    switch (type)
    {
    case '1':
    {
        RedNote* note = new RedNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.redNotes.push_back(note);
        break;
    }
    case '2':
    {
        BlueNote* note = new BlueNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.blueNotes.push_back(note);
        break;
    }
    case '3':
    {
        BigRedNote* note = new BigRedNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.bigRedNotes.push_back(note);
        break;
    }
    case '4':
    {
        BigBlueNote* note = new BigBlueNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.bigBlueNotes.push_back(note);
        break;
    }
    case '5':
    {
        YellowNote* note = new YellowNote();
        note->startTimePicoseconds = startPs;
        note->endTimePicoseconds = stopPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.yellowNotes.push_back(note);
        break;
    }
    case '6':
    {
        BigYellowNote* note = new BigYellowNote();
        note->startTimePicoseconds = startPs;
        note->endTimePicoseconds = stopPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.bigYellowNotes.push_back(note);
        break;
    }
    case '7':
    {
        GreenNote* note = new GreenNote();
        note->startTimePicoseconds = startPs;
        note->endTimePicoseconds = stopPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        note->remainingSpamHits = greenNoteHits;
        course.greenNotes.push_back(note);
        break;
    }
    case '9':
    {
        BigGreenNote* note = new BigGreenNote();
        note->startTimePicoseconds = startPs;
        note->endTimePicoseconds = stopPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        note->remainingSpamHits = greenNoteHits;
        course.bigGreenNotes.push_back(note);
        break;
    }
    case 'g':
    {
        GhostNote* note = new GhostNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.ghostNotes.push_back(note);
        break;
    }
    case 'G':
    {
        BigGhostNote* note = new BigGhostNote();
        note->startTimePicoseconds = startPs;
        note->scrollBPM = scrollBpm;
        note->scrollMultiplier = scrollMultiplier;
        course.bigGhostNotes.push_back(note);
        break;
    }
    default:
        break;
    }
}

static void ProcessMeasureBuffer(std::vector<Token>& buffer, ParserState& st, Course& course)
{
    int N = 0;
    for (size_t i = 0; i < buffer.size(); i++)
    {
        if (!buffer[i].isCommand)
            N++;
    }

    Measure measure;
    measure.measureIndex = st.measureIndex++;
    measure.lastScrollMultiplier = st.scroll;
    measure.lastBpm = FractionToDouble(st.bpm);
    course.measures.push_back(measure);

    if (st.barlineVisible)
    {
        Barline* barline = new Barline();
        barline->startTimePicoseconds = FractionToPicoseconds(st.time);
        barline->scrollBPM = measure.lastBpm;
        barline->scrollMultiplier = st.scroll;
        if (GraphiteGlobals::playbackRate != 1.0)
        {
            barline->startTimePicoseconds = static_cast<int64_t>(static_cast<double>(barline->startTimePicoseconds) / GraphiteGlobals::playbackRate);
            barline->scrollBPM *= GraphiteGlobals::playbackRate;
        }
        course.barlineNotes.push_back(barline);
    }

    for (size_t i = 0; i < buffer.size(); i++)
    {
        Token& t = buffer[i];
        if (t.isCommand)
        {
            ApplyCommand(st, t.cmd, t.arg);
        }
        else
        {
            char c = t.noteChar;
            if (c == '8')
            {
                if (st.pendingNote.active)
                {
                    AddNoteToCourse(course, st.pendingNote.type, st.pendingNote.startTime, st.time, st.pendingNote.bpm, st.pendingNote.greenNoteHits, st.pendingNote.scrollMultiplier);
                    st.pendingNote.active = false;
                }
            }
            else if (c == '9' && st.pendingNote.active && st.pendingNote.type == '9')
            {
                AddNoteToCourse(course, '9', st.pendingNote.startTime, st.time, st.pendingNote.bpm, st.pendingNote.greenNoteHits, st.pendingNote.scrollMultiplier);
                st.pendingNote.active = false;
            }
            else
            {
                switch (c)
                {
                case '1':
                case '2':
                case '3':
                case '4':
                case 'g':
                case 'G':
                    AddNoteToCourse(course, c, st.time, st.time, st.bpm, 0, st.scroll);
                    break;
                case '5':
                case '6':
                    st.pendingNote.active = true;
                    st.pendingNote.type = c;
                    st.pendingNote.startTime = st.time;
                    st.pendingNote.bpm = st.bpm;
                    st.pendingNote.greenNoteHits = 0;
                    st.pendingNote.scrollMultiplier = st.scroll;
                    break;
                case '7':
                case '9':
                {
                    int hits = 0;
                    if (st.greenNoteIndex < static_cast<int>(st.greenNoteValues.size()))
                        hits = st.greenNoteValues[st.greenNoteIndex++];
                    st.pendingNote.active = true;
                    st.pendingNote.type = c;
                    st.pendingNote.startTime = st.time;
                    st.pendingNote.bpm = st.bpm;
                    st.pendingNote.greenNoteHits = hits;
                    st.pendingNote.scrollMultiplier = st.scroll;
                    break;
                }
                default:
                    break;
                }
            }

            st.time.add(MeasureDurationSeconds(st, N));
        }
    }

    if (N == 0)
    {
        st.time.add(MeasureDurationSeconds(st, 0));
    }
}

// ------------------------------ Course parsing -----------------------------

static int ParseCourseBody(const std::vector<std::string>& lines, int idx, ParserState& st, Course& course)
{
    std::vector<Token> buffer;
    bool skipBranch = false;

    for (; idx < static_cast<int>(lines.size()); idx++)
    {
        std::string raw = StripComment(lines[idx]);
        std::string line = Trim(raw);
        if (line.empty())
            continue;

        if (line[0] == '#')
        {
            std::string cmd, arg;
            ParseCommandLine(line, cmd, arg);

            if (cmd == "END")
            {
                if (!buffer.empty())
                {
                    if (!skipBranch)
                    {
                        ProcessMeasureBuffer(buffer, st, course);
                    }
                    buffer.clear();
                }
                // Flush any pending note spanning across the course end
                if (st.pendingNote.active)
                {
                    AddNoteToCourse(course, st.pendingNote.type, st.pendingNote.startTime, st.pendingNote.startTime, st.pendingNote.bpm, st.pendingNote.greenNoteHits, st.pendingNote.scrollMultiplier);
                    st.pendingNote.active = false;
                }
                return idx + 1;
            }
            if (cmd == "BRANCHSTART")
            {
                skipBranch = false;
                continue;
            }
            if (cmd == "BRANCHEND")
            {
                skipBranch = false;
                continue;
            }
            if (cmd == "N")
            {
                skipBranch = false;
                continue;
            }
            if (cmd == "E")
            {
                skipBranch = true;
                continue;
            }
            if (cmd == "M")
            {
                skipBranch = true;
                continue;
            }

            if (skipBranch)
                continue;

            buffer.emplace_back(true, cmd, arg, '0');
            continue;
        }

        if (skipBranch)
            continue;

        for (size_t ci = 0; ci < line.size(); ci++)
        {
            char c = line[ci];
            if (std::isspace(static_cast<unsigned char>(c)))
                continue;
            if (c == ',')
            {
                ProcessMeasureBuffer(buffer, st, course);
                buffer.clear();
            }
            else if (std::isalnum(static_cast<unsigned char>(c)))
            {
                buffer.emplace_back(false, "", "", c);
            }
        }
    }

    if (!buffer.empty())
    {
        ProcessMeasureBuffer(buffer, st, course);
    }
    // Flush any pending note at end of course body
    if (st.pendingNote.active)
    {
        AddNoteToCourse(course, st.pendingNote.type, st.pendingNote.startTime, st.pendingNote.startTime, st.pendingNote.bpm, st.pendingNote.greenNoteHits, st.pendingNote.scrollMultiplier);
        st.pendingNote.active = false;
    }
    return idx;
}

// ------------------------------ Full document parse --------------------------------

static std::string ReadAllText(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
        return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static std::string StripBom(const std::string& content)
{
    if (content.size() >= 3 && static_cast<unsigned char>(content[0]) == 0xEF && static_cast<unsigned char>(content[1]) == 0xBB && static_cast<unsigned char>(content[2]) == 0xBF)
    {
        return content.substr(3);
    }
    return content;
}

bool parseTjauContent(const std::string& content, Chart& outChart, std::string& outError)
{
    std::vector<std::string> lines = SplitLines(StripBom(content));

    // ---- Pass 1: global header ----
    GlobalMeta meta;
    int idx = 0;
    for (; idx < static_cast<int>(lines.size()); idx++)
    {
        std::string line = Trim(StripComment(lines[idx]));
        if (line.empty())
            continue;
        if (StartsWithCI(line, "COURSE:") || StartsWithCI(line, "#START"))
            break;

        std::string key, val;
        if (!SplitKeyValue(line, key, val))
            continue;

        if (key == "TITLE")
            meta.title = val;
        else if (key == "TITLEJA")
            meta.titleJa = val;
        else if (key == "TITLEZH")
            meta.titleZh = val;
        else if (key == "SUBTITLE")
            meta.subtitle = val;
        else if (key == "SUBTITLEJA")
            meta.subtitleJa = val;
        else if (key == "SUBTITLEZH")
            meta.subtitleZh = val;
        else if (key == "WAVE")
            meta.wave = val;
        else if (key == "LEVEL")
            meta.level = ToInt(val, 0);
        else if (key == "BPM")
            ParseFraction(val, meta.bpm);
        else if (key == "OFFSET")
            ParseFraction(val, meta.offset);
    }

    outChart = Chart();
    outChart.title = meta.title;
    outChart.wave = meta.wave;
    outChart.defaultBpm = meta.bpm.toFractionString();
    outChart.defaultBpmDouble = FractionToDouble(meta.bpm);
    outChart.defaultOffset = meta.offset.toFractionString();
    outChart.defaultOffsetPicoseconds = FractionToPicoseconds(meta.offset);

    // ---- Pass 2: course blocks ----
    int pendingCourseId = -1;
    int pendingLevel = meta.level;
    std::vector<int> pendingBalloon;

    while (idx < static_cast<int>(lines.size()))
    {
        std::string line = Trim(StripComment(lines[idx]));
        if (line.empty())
        {
            idx++;
            continue;
        }

        if (StartsWithCI(line, "COURSE:"))
        {
            pendingCourseId = ParseCourseName(Trim(line.substr(7)));
            if (pendingCourseId < 0)
                pendingCourseId = -1;
            pendingLevel = meta.level;
            pendingBalloon.clear();
            idx++;
            continue;
        }

        if (StartsWithCI(line, "#START"))
        {
            if (pendingCourseId < 0)
            {
                // Skip the whole course block: no valid course was declared.
                int depth = 1;
                idx++;
                while (idx < static_cast<int>(lines.size()) && depth > 0)
                {
                    std::string l2 = Trim(StripComment(lines[idx]));
                    if (StartsWithCI(l2, "#START"))
                        depth++;
                    else if (StartsWithCI(l2, "#END"))
                        depth--;
                    idx++;
                }
                continue;
            }

            std::string cmd, arg;
            ParseCommandLine(line, cmd, arg);
            bool isP2 = ToUpper(arg) == "P2";
            idx++;

            if (isP2)
            {
                int depth = 1;
                while (idx < static_cast<int>(lines.size()) && depth > 0)
                {
                    std::string l2 = Trim(StripComment(lines[idx]));
                    if (StartsWithCI(l2, "#START"))
                        depth++;
                    else if (StartsWithCI(l2, "#END"))
                        depth--;
                    idx++;
                }
                continue;
            }

            Course course;
            course.courseNumber = pendingCourseId;
            course.level = pendingLevel;

            ParserState st;
            st.bpm = meta.bpm;
            st.greenNoteValues = pendingBalloon;

            idx = ParseCourseBody(lines, idx, st, course);
            outChart.courses.push_back(course);
            continue;
        }

        std::string k, v;
        if (SplitKeyValue(line, k, v))
        {
            if (k == "LEVEL")
                pendingLevel = ToInt(v, 0);
            else if (k == "BALLOON")
            {
                pendingBalloon.clear();
                size_t start = 0;
                while (start <= v.size())
                {
                    size_t comma = v.find(',', start);
                    if (comma == std::string::npos)
                        comma = v.size();
                    std::string part = Trim(v.substr(start, comma - start));
                    int bv = 0;
                    if (TryParseInt(part, bv))
                        pendingBalloon.push_back(bv);
                    if (comma == v.size())
                        break;
                    start = comma + 1;
                }
            }
        }
        idx++;
    }

    if (outChart.title.empty())
    {
        outError = "No TITLE found";
        return false;
    }

    return true;
}

bool parseTjauFile(const std::string& filePath, Chart& outChart, std::string& outError)
{
    std::string content = ReadAllText(filePath);
    if (content.empty())
    {
        outError = "Could not open or read file: " + filePath;
        return false;
    }
    return parseTjauContent(content, outChart, outError);
}

// ------------------------------ Metadata parse --------------------------------

bool readTjauMetadataContent(const std::string& content, TjauMetadata& outMeta, std::string& outError)
{
    std::vector<std::string> lines = SplitLines(StripBom(content));

    int globalLevel = 0;
    int idx = 0;
    for (; idx < static_cast<int>(lines.size()); idx++)
    {
        std::string line = Trim(StripComment(lines[idx]));
        if (line.empty())
            continue;
        if (StartsWithCI(line, "COURSE:") || StartsWithCI(line, "#START"))
            break;

        std::string key, val;
        if (!SplitKeyValue(line, key, val))
            continue;

        if (key == "TITLE")
            outMeta.title = val;
        else if (key == "SUBTITLE")
            outMeta.subtitle = val;
        else if (key == "LEVEL")
            globalLevel = ToInt(val, 0);
    }

    std::vector<TjauMetadataCourse> courses;
    int pendingCourseId = -1;
    int pendingLevel = globalLevel;

    while (idx < static_cast<int>(lines.size()))
    {
        std::string line = Trim(StripComment(lines[idx]));
        if (line.empty())
        {
            idx++;
            continue;
        }

        if (StartsWithCI(line, "COURSE:"))
        {
            pendingCourseId = ParseCourseName(Trim(line.substr(7)));
            if (pendingCourseId < 0)
                pendingCourseId = -1;
            pendingLevel = globalLevel;
            idx++;
            continue;
        }

        if (StartsWithCI(line, "#START"))
        {
            std::string cmd, arg;
            ParseCommandLine(line, cmd, arg);
            bool isP2 = ToUpper(arg) == "P2";
            idx++;

            if (pendingCourseId >= 0 && !isP2)
            {
                TjauMetadataCourse c;
                c.courseId = pendingCourseId;
                c.level = pendingLevel;
                courses.push_back(c);
            }

            // Skip the course body up to the matching #END.
            int depth = 1;
            while (idx < static_cast<int>(lines.size()) && depth > 0)
            {
                std::string l2 = Trim(StripComment(lines[idx]));
                if (StartsWithCI(l2, "#START"))
                    depth++;
                else if (StartsWithCI(l2, "#END"))
                    depth--;
                idx++;
            }
            continue;
        }

        std::string k, v;
        if (SplitKeyValue(line, k, v))
        {
            if (k == "LEVEL")
                pendingLevel = ToInt(v, 0);
        }
        idx++;
    }

    outMeta.courses = std::move(courses);

    if (outMeta.title.empty())
    {
        outError = "No TITLE found";
        return false;
    }

    return true;
}

bool readTjauMetadataFile(const std::string& filePath, TjauMetadata& outMeta, std::string& outError)
{
    std::string content = ReadAllText(filePath);
    if (content.empty())
    {
        outError = "Could not open or read file: " + filePath;
        return false;
    }
    return readTjauMetadataContent(content, outMeta, outError);
}