#include "help_command.h"
#include "reservation_store.h"
#include "uuid_generator.h"

#include <iostream>

void print_help(const char *program)
{
    std::cout
        << "Usage:\n"
        << "  " << program << " uuid [device-name] [options]\n"
        << "  " << program << " reserve [--check] <name>\n"
        << "  " << program << " reserve --list\n"
        << "  " << program << " free <name>\n"
        << "  " << program << " help\n"
        << "\n"
        << "Commands:\n"
        << "  uuid      Generate a UUID from a device name or phrase.\n"
        << "  reserve   Reserve a name, or check if it is already reserved.\n"
        << "  free      Remove a reserved name.\n"
        << "  help      Show this help.\n"
        << "\n"
        << "Reservation file:\n"
        << "  " << default_reservation_path() << "\n"
        << "  Override with GEN_UUID_RESERVATIONS.\n";
}

void print_uuid_help(const char *program)
{
    std::cout
        << "Usage: " << program << " uuid [device-name] [--base UUID] [--service N] [--characteristic N] [--descriptor N] [--name TEXT]\n"
        << "\n"
        << "Options:\n"
        << "  --base, -b    Base UUID template. Default: " << DEFAULT_BASE_UUID << "\n"
        << "               Layout: ae615[SVC_IDX]-[CHAR_IDX]-4[DESC_IDX]-8000-[XXXXXXXXXXXX]\n"
        << "  --service     Service index as decimal or 0x-prefixed hexadecimal (0..0xfff).\n"
        << "  --characteristic\n"
        << "               Characteristic index as decimal or 0x-prefixed hexadecimal (0..0xffff).\n"
        << "  --descriptor  Descriptor index as decimal or 0x-prefixed hexadecimal (0..0xfff).\n"
        << "  --name, -n    Device name/phrase. A positional value can be used instead.\n"
        << "  --help, -h    Show UUID command help.\n";
}

void print_reserve_help(const char *program)
{
    std::cout
        << "Usage:\n"
        << "  " << program << " reserve [--check] <name>\n"
        << "  " << program << " reserve --list\n"
        << "\n"
        << "Options:\n"
        << "  --check, -c   Check whether the name is reserved without changing it.\n"
        << "  --list, -l    List all reserved names.\n"
        << "  --help, -h    Show reserve command help.\n";
}

void print_free_help(const char *program)
{
    std::cout
        << "Usage: " << program << " free <name>\n"
        << "\n"
        << "Options:\n"
        << "  --help, -h    Show free command help.\n";
}
