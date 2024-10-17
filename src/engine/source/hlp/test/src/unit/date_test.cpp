#include <gtest/gtest.h>

#include "hlp_test.hpp"
#include <base/logging.hpp>

auto constexpr NAME = "dateParser";
static const std::string TARGET = "/TargetField";
auto BUILD_YEAR = std::string(__DATE__).substr(7);

auto initAndGetDateParser() -> hlp::ParserBuilder
{
    logging::testInit();
    static bool hasInitiated = false;
    if (!hasInitiated)
    {
        hlp::initTZDB("data/tzdb", false);
        hasInitiated = true;
    }
    return hlp::parsers::getDateParser;
};

INSTANTIATE_TEST_SUITE_P(
    DateBuild,
    HlpBuildTest,
    ::testing::Values(
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"not_match"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00Z"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00.000Z"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00Z"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00.000Z"}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"01/01/22"}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"02/02/22"}}),
        //  BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01 00:00:00.000 this_should_fail"}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T00:00:00Z asd"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00Z", "en_US.UTF-8"}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"2020-01-01T01:00:00Z", "wrong_locale"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"%d %b %y %R %z"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"we dont verify the format string %"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"%d %b %y %R %z", "en_US.UTF-8"}}),
        BuildT(FAILURE, initAndGetDateParser(), {NAME, TARGET, {}, {"%d %b %y %R %z", "wrong_locale"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"ANSIC", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"UnixDate", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RubyDate", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC822", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC822Z", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC850", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC1123", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC1123Z", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC3339", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"RFC3164", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"SYSLOG", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"ISO8601", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"ISO8601Z", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"HTTPDATE", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"NGINX_ERROR", "en_US.UTF-8"}}),
        BuildT(SUCCESS, initAndGetDateParser(), {NAME, TARGET, {}, {"POSTGRES", "en_US.UTF-8"}})));

INSTANTIATE_TEST_SUITE_P(
    DateParse,
    HlpParseTest,
    ::testing::Values(
        ParseT(SUCCESS,
               "Monday, 02-Jan-06 15:04:05 MST ###123",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:05.000Z"}})", TARGET.substr(1))),
               30,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%A, %d-%b-%y %T %Z", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Jun 14 15:16:01",
               j(fmt::format(R"({{"{}": "{}-06-14T15:16:01.000Z"}})", TARGET.substr(1), BUILD_YEAR)),
               15,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%b %d %T", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "June 14 15:16:01",
               j(fmt::format(R"({{"{}": "{}-06-14T15:16:01.000Z"}})", TARGET.substr(1), BUILD_YEAR)),
               16,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%B %d %T", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2019-12-12",
               j(fmt::format(R"({{"{}": "2019-12-12T00:00:00.000Z"}})", TARGET.substr(1))),
               10,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%F", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2019-12-12 ABC",
               j(fmt::format(R"({{"{}": "2019-12-12T00:00:00.000Z"}})", TARGET.substr(1))),
               10,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%F", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2019-12-12ABC",
               j(fmt::format(R"({{"{}": "2019-12-12T00:00:00.000Z"}})", TARGET.substr(1))),
               10,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%F", "en_US.UTF-8"}}),
        ParseT(FAILURE, "ABC2019-12-12", {}, 0, initAndGetDateParser(), {NAME, TARGET, {}, {"%F", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T16:15:55.000Z"}})", TARGET.substr(1))),
               24,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %Y", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55.103786 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T16:15:55.103Z"}})", TARGET.substr(1))),
               31,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %Y", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55.103786 2016 #####",
               j(fmt::format(R"({{"{}": "2016-12-26T16:15:55.103Z"}})", TARGET.substr(1))),
               31,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %Y", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55.103 MST 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T09:15:55.103Z"}})", TARGET.substr(1))),
               32,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %Z %Y", "en_US.UTF-8"}}),
        ParseT(FAILURE,
               "Mon Dec 26 16:15:55.103 MST 2016",
               {},
               0,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %z %Y", "en_US.UTF-8"}}),
        //         ISO8601 no time zone and time zone needs two different formats
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151+02:00",
               j(fmt::format(R"({{"{}": "2018-08-14T12:30:02.203Z"}})", TARGET.substr(1))),
               32,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%T%Ez", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151Z",
               j(fmt::format(R"({{"{}": "2018-08-14T14:30:02.203Z"}})", TARGET.substr(1))),
               27,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%TZ", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151Z other things",
               j(fmt::format(R"({{"{}": "2018-08-14T14:30:02.203Z"}})", TARGET.substr(1))),
               27,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%TZ", "en_US.UTF-8"}}),
        // timezone offsets
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151-02:00",
               j(fmt::format(R"({{"{}": "2018-08-14T16:30:02.203Z"}})", TARGET.substr(1))),
               32,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%T%Ez", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151-02:00 QWERTY",
               j(fmt::format(R"({{"{}": "2018-08-14T16:30:02.203Z"}})", TARGET.substr(1))),
               32,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%T%Ez", "en_US.UTF-8"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55 -0000 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T16:15:55.000Z"}})", TARGET.substr(1))),
               strlen("Mon Dec 26 16:15:55 -0000 2016"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %z %Y"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:15:55 -0700 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T23:15:55.000Z"}})", TARGET.substr(1))),
               strlen("Mon Dec 26 16:15:55 -0700 2016"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %T %z %Y"}}),
        ParseT(SUCCESS,
               "26 Dec 16 23:15 MST",
               j(fmt::format(R"({{"{}": "2016-12-26T16:15:00.000Z"}})", TARGET.substr(1))),
               strlen("26 Dec 16 23:15 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%d %b %y %R %Z"}}),
        ParseT(
            FAILURE, "26 Dec 16 23:15 -0000", {}, 21, initAndGetDateParser(), {NAME, TARGET, {}, {"%d %b %y %R %Z"}}),
        ParseT(SUCCESS,
               "26 Dec 16 23:15 -0000",
               j(fmt::format(R"({{"{}": "2016-12-26T23:15:00.000Z"}})", TARGET.substr(1))),
               strlen("26 Dec 16 23:15 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%d %b %y %R %z"}}),
        ParseT(
            FAILURE, "26 Dec 16 23:15 -0000", {}, 21, initAndGetDateParser(), {NAME, TARGET, {}, {"%d %b %y %R %Z"}}),
        ParseT(SUCCESS,
               "Monday, 26-Dec-16 16:16:55 MST",
               j(fmt::format(R"({{"{}": "2016-12-26T09:16:55.000Z"}})", TARGET.substr(1))),
               strlen("Monday, 26-Dec-16 16:16:55 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%A, %d-%b-%y %T %Z"}}),
        ParseT(FAILURE,
               "Monday, 26-Dec-16 16:16:55 -0000",
               {},
               32,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%A, %d-%b-%y %T %Z"}}),
        ParseT(SUCCESS,
               "Monday, 26-Dec-16 16:16:55 -0000",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
               strlen("Monday, 26-Dec-16 16:16:55 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%A, %d-%b-%y %T %z"}}),
        ParseT(FAILURE,
               "Monday, 26-Dec-16 16:16:55 MST",
               {},
               0,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%A, %d-%b-%y %T %z"}}),
        ParseT(SUCCESS,
               "Mon, 26-Dec-16 16:16:55 MST",
               j(fmt::format(R"({{"{}": "2016-12-26T09:16:55.000Z"}})", TARGET.substr(1))),
               strlen("Mon, 26-Dec-16 16:16:55 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a, %d-%b-%y %T %Z"}}),
        ParseT(FAILURE,
               "Mon, 26-Dec-16 16:16:55 -0000",
               {},
               29,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a, %d-%b-%y %T %Z"}}),
        ParseT(SUCCESS,
               "Mon, 26-Dec-16 16:16:55 -0000",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
               strlen("Mon, 26-Dec-16 16:16:55 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a, %d-%b-%y %T %z"}}),
        ParseT(FAILURE,
               "Mon, 26-Dec-16 16:16:55 MST",
               {},
               0,
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a, %d-%b-%y %T %z"}}),
        ParseT(SUCCESS,
               "2016-12-26T16:16:55Z00:00",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
               strlen("2016-12-26T16:16:55Z00:00"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%TZ%Ez"}}),
        ParseT(SUCCESS,
               "2016-12-26T16:16:55Z07:00",
               j(fmt::format(R"({{"{}": "2016-12-26T09:16:55.000Z"}})", TARGET.substr(1))),
               strlen("2016-12-26T16:16:55Z07:00"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%FT%TZ%Ez"}}),
        ParseT(SUCCESS,
               "December 26 16:16:55.123 UTC",
               j(fmt::format(R"({{"{}": "{}-12-26T16:16:55.123Z"}})", TARGET.substr(1), BUILD_YEAR)),
               strlen("December 26 16:16:55.123 UTC"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%B %d %R:%6S %Z"}}),
        ParseT(SUCCESS,
               "December 26 16:16:55.123 -0000",
               j(fmt::format(R"({{"{}": "{}-12-26T16:16:55.123Z"}})", TARGET.substr(1), BUILD_YEAR)),
               strlen("December 26 16:16:55.123 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%B %d %R:%6S %z"}}),
        ParseT(SUCCESS,
               "Dec 26 16:16:55.123 UTC",
               j(fmt::format(R"({{"{}": "{}-12-26T16:16:55.123Z"}})", TARGET.substr(1), BUILD_YEAR)),
               strlen("Dec 26 16:16:55.123 UTC"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%b %d %R:%6S %Z"}}),
        ParseT(SUCCESS,
               "Dec 26 16:16:55.123 -0000",
               j(fmt::format(R"({{"{}": "{}-12-26T16:16:55.123Z"}})", TARGET.substr(1), BUILD_YEAR)),
               strlen("Dec 26 16:16:55.123 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%b %d %R:%6S %z"}}),
        ParseT(SUCCESS,
               "26/Dec/2016:16:16:55 -0000",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
               strlen("26/Dec/2016:16:16:55 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%d/%b/%Y:%T %z"}}),
        // TODO: this is not working
        //  ParseT(SUCCESS,
        //         "2016/12/26 16:16:55",
        //         j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
        //         strlen("2016/12/26 16:16:55"),
        //         initAndGetDateParser(),
        //         {NAME, TARGET, {}, {"%D %T"}}),
        ParseT(SUCCESS,
               "Mon Dec 26 16:16:55.103786 2016",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.103Z"}})", TARGET.substr(1))),
               strlen("Mon Dec 26 16:16:55.103786 2016"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%a %b %d %H:%M:%9S %Y"}}),
        ParseT(SUCCESS,
               "2016-12-26 16:16:55 UTC",
               j(fmt::format(R"({{"{}": "2016-12-26T16:16:55.000Z"}})", TARGET.substr(1))),
               strlen("2016-12-26 16:16:55 UTC"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"%F %H:%M:%6S %Z"}}),
        ParseT(SUCCESS,
               "Mon Jan 2 15:04:05 2006",
               j(fmt::format(R"({{"{}": "2006-01-02T15:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Mon Jan 2 15:04:05 2006"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"ANSIC"}}),
        ParseT(SUCCESS,
               "Mon Jan 2 15:04:05 MST 2006",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Mon Jan 2 15:04:05 MST 2006"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"UnixDate"}}),
        ParseT(SUCCESS,
               "Mon Jan 02 15:04:05 -0700 2006",
               j(fmt::format(R"({{"{}": "2006-01-02T22:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Mon Jan 02 15:04:05 -0700 2006"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RubyDate"}}),
        ParseT(SUCCESS,
               "02 Jan 06 15:04 MST",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:00.000Z"}})", TARGET.substr(1))),
               strlen("02 Jan 06 15:04 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC822"}}),
        ParseT(SUCCESS,
               "02 Jan 06 15:04 -0000",
               j(fmt::format(R"({{"{}": "2006-01-02T15:04:00.000Z"}})", TARGET.substr(1))),
               strlen("02 Jan 06 15:04 -0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC822Z"}}),
        ParseT(SUCCESS,
               "Monday, 02-Jan-06 15:04:05 MST",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Monday, 02-Jan-06 15:04:05 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC850"}}),
        ParseT(SUCCESS,
               "Mon, 02 Jan 2006 15:04:05 MST",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Mon, 02 Jan 2006 15:04:05 MST"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC1123"}}),
        ParseT(SUCCESS,
               "Mon, 02 Jan 2006 15:04:05 -0700",
               j(fmt::format(R"({{"{}": "2006-01-02T22:04:05.000Z"}})", TARGET.substr(1))),
               strlen("Mon, 02 Jan 2006 15:04:05 -0700"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC1123Z"}}),
        ParseT(SUCCESS,
               "2006-01-02T15:04:05Z07:00",
               j(fmt::format(R"({{"{}": "2006-01-02T08:04:05.000Z"}})", TARGET.substr(1))),
               strlen("2006-01-02T15:04:05Z07:00"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC3339"}}),
        ParseT(SUCCESS,
               "Mar  1 18:48:50.483 UTC",
               j(fmt::format(R"({{"{}": "2024-03-01T18:48:50.483Z"}})", TARGET.substr(1))),
               strlen("Mar  1 18:48:50.483 UTC"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"RFC3164"}}),
        ParseT(SUCCESS,
               "Jun 14 15:16:01",
               j(fmt::format(R"({{"{}": "2024-06-14T15:16:01.000Z"}})", TARGET.substr(1))),
               strlen("Jun 14 15:16:01"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"SYSLOG"}}),
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151+02:00",
               j(fmt::format(R"({{"{}": "2018-08-14T12:30:02.203Z"}})", TARGET.substr(1))),
               strlen("2018-08-14T14:30:02.203151+02:00"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"ISO8601"}}),
        ParseT(SUCCESS,
               "2018-08-14T14:30:02.203151Z",
               j(fmt::format(R"({{"{}": "2018-08-14T14:30:02.203Z"}})", TARGET.substr(1))),
               strlen("2018-08-14T14:30:02.203151Z"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"ISO8601Z"}}),
        ParseT(SUCCESS,
               "26/Dec/2016:16:22:14 +0000",
               j(fmt::format(R"({{"{}": "2016-12-26T16:22:14.000Z"}})", TARGET.substr(1))),
               strlen("26/Dec/2016:16:22:14 +0000"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"HTTPDATE"}}),
        ParseT(SUCCESS,
               "2019/10/30 23:26:34",
               j(fmt::format(R"({{"{}": "2019-10-30T23:26:34.000Z"}})", TARGET.substr(1))),
               strlen("2019/10/30 23:26:34"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"NGINX_ERROR"}}),
        ParseT(SUCCESS,
               "2021-02-14 10:45:33 UTC",
               j(fmt::format(R"({{"{}": "2021-02-14T10:45:33.000Z"}})", TARGET.substr(1))),
               strlen("2021-02-14 10:45:33 UTC"),
               initAndGetDateParser(),
               {NAME, TARGET, {}, {"POSTGRES"}})));