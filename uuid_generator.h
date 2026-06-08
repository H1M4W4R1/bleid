#ifndef UUID_GENERATOR_H
#define UUID_GENERATOR_H

#include <string>

extern const char *DEFAULT_BASE_UUID;

std::string generate_uuid(std::string base,
                          const std::string &phrase,
                          bool has_service,
                          unsigned int service,
                          bool has_characteristic,
                          unsigned int characteristic,
                          bool has_descriptor,
                          unsigned int descriptor);

unsigned long long parse_unsigned(const std::string &text, unsigned long long maximum, const char *name);

#endif
