#ifndef RESERVATION_STORE_H
#define RESERVATION_STORE_H

#include <string>
#include <vector>

std::string default_reservation_path();

class ReservationStore
{
public:
    ReservationStore();

    bool is_reserved(const std::string &name) const;
    bool reserve(const std::string &name);
    bool free_name(const std::string &name);
    std::vector<std::string> names() const;
    std::string path() const;

private:
    std::string file_path_;
    std::vector<std::string> names_;

    void load();
    void save() const;
};

#endif
