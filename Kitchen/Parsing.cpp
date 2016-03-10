#include "Parsing.h"

#include <errno.h>

#include <Logging.h>

#define MAX_FIELD_COUNT 4
#define MAX_PAYLOAD_SIZE 50

struct Record
{
    Command command;
    size_t count;
    const char *  fields[MAX_FIELD_COUNT];
};

bool hex_to_byte(const char * str, byte & result)
{
    result = strtol(str, nullptr, 16);
    if (errno != 0) {
        LOG_ERROR("Fail to parse hex to byte: %s", str);
        return false;
    }

    return true;
}

bool string_to_white(const char * str, RGBW & result)
{
    size_t len = strlen(str);
    if (len != 2) {
        LOG_ERROR("Unexpected length of hex string: %d", len);
        return false;
    }

    result.R = 0;
    result.G = 0;
    result.B = 0;
    return hex_to_byte(str, result.W);
}

bool string_to_ulong(const char * str, unsigned long & result)
{
    result = strtoul(str, nullptr, 10);
    if (errno != 0) {
        LOG_ERROR("Fail to unsigned int in string: %s", str);
        return false;
    }

    return true;
}

Record parse_command_string(const char * input)
{
    Record result = {Command::NONE , 0, {}};

    if (!input) {
        LOG_ERROR("Empty payload");
        return result;
    }

    size_t len = strlen(input);
    if (len < 1) {
        LOG_ERROR("Too short payload: %d", len);
        return result;
    }
    if (len >= MAX_PAYLOAD_SIZE) {
        LOG_ERROR("Too long payload: %d", len);
        return result;
    }

    if (len > 2 && input[1] != ',') {
        LOG_ERROR("Wrong format");
        return result;
    }

    static char data[MAX_PAYLOAD_SIZE] = {};
    strncpy(data, input, MAX_PAYLOAD_SIZE);

    result.command = static_cast<Command>(data[0]);

    if (len > 2) {
        result.fields[0] = &data[2];
        result.count = 1;
    }

    for (size_t i = 2; i < len; i++) {
        if (data[i] == ',') {
            data[i] = '\0';
            result.fields[result.count++] = &data[i+1];
        }
    }

    return result;
}

bool string_to_rgb(const char * str, RGBW & result)
{
    result = {0, 0, 0, 0};

    if (!str) {
        LOG_ERROR("RGB string is NULL");
        return false;
    }

    size_t len = strlen(str);
    if (len != 3*2) {
        LOG_ERROR("Unexpected length of RGB string: %d", len);
        return false;
    }

    char buffer[7];
    strncpy(buffer, str, sizeof(buffer));

    if (!hex_to_byte(buffer+4, result.B)) {
        LOG_ERROR("Fail to parse B in string: %s", str);
        return false;
    }
    buffer[4] = '\0';
    if (!hex_to_byte(buffer+2, result.G)) {
        LOG_ERROR("Fail to parse G in string: %s", str);
        return false;
    }
    buffer[2] = '\0';
    if (!hex_to_byte(buffer, result.R)) {
        LOG_ERROR("Fail to parse R in string: %s", str);
        return false;
    }

    result.W = 0;
    return true;
}

bool parse_cmd_set_limit(const Record & record, LedType type, RGBW & result)
{
    if (record.count != 1) {
        LOG_ERROR("Wrong data");
        return false;
    }

    if (type == LedType::Color) {
        if (!string_to_rgb(record.fields[0], result)) {
            LOG_ERROR("Error parsing transition start value");
            return false;
        };
    }
    else if (type == LedType::White) {
        if (!string_to_white(record.fields[0], result)) {
            LOG_ERROR("Error parsing transition start value");
            return false;
        };
    }

    return true;
}

bool parse_cmd_transition(const Record & record, LedType type, Transition & transition)
{
    if (record.count != 3) {
        LOG_ERROR("Wrong data");
        return false;
    }

    const char * start = record.fields[0];
    const char * stop = record.fields[1];
    const char * time = record.fields[2];

    if (type == LedType::Color) {
        if (!string_to_rgb(start, transition.start)) {
            LOG_ERROR("Error parsing transition start value");
            return false;
        };
        if (!string_to_rgb(stop, transition.stop)) {
            LOG_ERROR("Error parsing transition stop value");
            return false;
        };
    }
    else if (type == LedType::White) {
        if (!string_to_white(start, transition.start)) {
            LOG_ERROR("Error parsing transition start value");
            return false;
        };
        if (!string_to_white(stop, transition.stop)) {
            LOG_ERROR("Error parsing transition stop value");
            return false;
        };
    }
    if (!string_to_ulong(time, transition.time)) {
        LOG_ERROR("Error parsing transition time");
        return false;
    };

    return true;
}

CommandData parse_command(const char * payload, LedType led_type)
{
    LOG_DEBUG("Input: %s", payload);

    Record record = parse_command_string(payload);
    LOG_DEBUG("Parsed: command=%c, count=%d, fields=%s,%s,%s,%s", record.command, record.count,
              record.fields[0], record.fields[1], record.fields[2], record.fields[3]);

    CommandData result{};

    bool res = false;
    switch (record.command) {
    case Command::LIGHT_SET_LIMIT:
        res = parse_cmd_set_limit(record, led_type, result.limit);
        break;
    case Command::TRANSITION_ONE_SHOT:
        res = parse_cmd_transition(record, led_type, result.transition);
        result.transition.is_loop = false;
        break;
    case Command::TRANSITION_LOOP:
        res = parse_cmd_transition(record, led_type, result.transition);
        result.transition.is_loop = true;
        break;
    default:
        LOG_ERROR("Unknown mode: %c", payload[0]);
        return CommandData{};
    }
    result.command = record.command;

    if (!res) {
        LOG_ERROR("Parsing error");
        return CommandData{};
    }

    LOG_DEBUG("Parsed: cmd='%c', transition={%02x%02x%02x%02x => %02x%02x%02x%02x, %lums}",
              result.command,
              result.transition.start.R, result.transition.start.G,
              result.transition.start.B, result.transition.start.W,
              result.transition.stop.R, result.transition.stop.G,
              result.transition.stop.B, result.transition.stop.W,
              result.transition.time);
    return result;
}
