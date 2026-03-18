#pragma once
#include <cstdint>
#include <cstddef>

namespace dae
{
    using Event = unsigned int;

    template <int Length>
    struct sdbm_hash
    {
        consteval static unsigned int _calculate(const char* const text, unsigned int& value)
        {
            const unsigned int character = sdbm_hash<Length - 1>::_calculate(text, value);
            value = character + (value << 6) + (value << 16) - value;
            return text[Length - 1];
        }

        consteval static unsigned int calculate(const char* const text)
        {
            unsigned int value = 0;
            const auto character = _calculate(text, value);
            return character + (value << 6) + (value << 16) - value;
        }
    };

    template <>
    struct sdbm_hash<1>
    {
        consteval static unsigned int _calculate(const char* const text, unsigned int&)
        {
            return text[0];
        }
    };

    template <size_t N>
    consteval unsigned int make_sdbm_hash(const char (&text)[N])
    {
        return sdbm_hash<N - 1>::calculate(text);
    }

    namespace Events
    {
        constexpr Event PlayerDied    = make_sdbm_hash("PlayerDied");
        constexpr Event PlayerRespawn = make_sdbm_hash("PlayerRespawn");
        constexpr Event PointsChanged = make_sdbm_hash("PointsChanged");
        constexpr Event GameWon       = make_sdbm_hash("GameWon");
    }
}
