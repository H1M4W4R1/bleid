#include "reservation_store.h"

#include <cstdlib>
#include <cerrno>
#include <fstream>
#include <stdexcept>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace
{
std::string join_path(const std::string &left, const std::string &right)
{
#ifdef _WIN32
    const char separator = '\\';
#else
    const char separator = '/';
#endif
    if (left.empty())
    {
        return right;
    }
    if (left[left.size() - 1] == '/' || left[left.size() - 1] == '\\')
    {
        return left + right;
    }
    return left + separator + right;
}

bool make_directory(const std::string &path)
{
#ifdef _WIN32
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
    return mkdir(path.c_str(), 0777) == 0 || errno == EEXIST;
#endif
}

std::string storage_directory()
{
    const char *app_data = std::getenv("APPDATA");
    if (app_data != 0 && app_data[0] != '\0')
    {
        return join_path(app_data, "gen_uuid");
    }

    const char *home = std::getenv("USERPROFILE");
    if (home != 0 && home[0] != '\0')
    {
        return join_path(home, ".gen_uuid");
    }

    const char *posix_home = std::getenv("HOME");
    if (posix_home != 0 && posix_home[0] != '\0')
    {
        return join_path(posix_home, ".gen_uuid");
    }

    return ".";
}

bool uses_override_path()
{
    const char *override_path = std::getenv("GEN_UUID_RESERVATIONS");
    return override_path != 0 && override_path[0] != '\0';
}
}

std::string default_reservation_path()
{
    const char *override_path = std::getenv("GEN_UUID_RESERVATIONS");
    if (override_path != 0 && override_path[0] != '\0')
    {
        return override_path;
    }

    return join_path(storage_directory(), "reserved_names.txt");
}

ReservationStore::ReservationStore()
    : file_path_(default_reservation_path())
{
    load();
}

bool ReservationStore::is_reserved(const std::string &name) const
{
    for (std::vector<std::string>::const_iterator it = names_.begin(); it != names_.end(); ++it)
    {
        if (*it == name)
        {
            return true;
        }
    }
    return false;
}

bool ReservationStore::reserve(const std::string &name)
{
    if (is_reserved(name))
    {
        return false;
    }

    names_.push_back(name);
    save();
    return true;
}

bool ReservationStore::free_name(const std::string &name)
{
    for (std::vector<std::string>::iterator it = names_.begin(); it != names_.end(); ++it)
    {
        if (*it == name)
        {
            names_.erase(it);
            save();
            return true;
        }
    }
    return false;
}

std::vector<std::string> ReservationStore::names() const
{
    return names_;
}

std::string ReservationStore::path() const
{
    return file_path_;
}

void ReservationStore::load()
{
    names_.clear();

    std::ifstream in(file_path_.c_str());
    if (!in)
    {
        return;
    }

    std::string line;
    while (std::getline(in, line))
    {
        if (!line.empty())
        {
            names_.push_back(line);
        }
    }
}

void ReservationStore::save() const
{
    const std::string directory = storage_directory();
    if (!uses_override_path() && !make_directory(directory))
    {
        throw std::runtime_error("could not create reservation directory: " + directory);
    }

    std::ofstream out(file_path_.c_str(), std::ios::trunc);
    if (!out)
    {
        throw std::runtime_error("could not write reservation file: " + file_path_);
    }

    for (std::vector<std::string>::const_iterator it = names_.begin(); it != names_.end(); ++it)
    {
        out << *it << "\n";
    }
}
