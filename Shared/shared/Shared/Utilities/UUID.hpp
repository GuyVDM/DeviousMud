#pragma once
#include <random>
#include <chrono>
#include <limits>

namespace DEVIOUSMUD 
{
    namespace RANDOM 
    {
        /// <summary>
        /// Universal unique identifyer. Generates a unsigned 64 bit unique handle. 
        /// </summary>
        class UUID
        {
        private:
            uint64_t identifyer;

        public:
            inline static UUID generate()
            {
                uint64_t identifyer;

                // Get current timestamp
                uint64_t timestamp = static_cast<uint64_t>(std::time(nullptr));

                // Generate random bits
                std::random_device rd;
                std::mt19937_64 gen(rd());

                constexpr uint64_t max_value = ULLONG_MAX;

                std::uniform_int_distribution<uint64_t> dis(0, max_value);
                uint64_t randomBits = dis(gen);

                // Combine timestamp and random bits
                identifyer = (timestamp << 32) | (randomBits & 0xFFFFFFFF);
                return UUID(identifyer);
            }

            inline UUID() 
            {
                identifyer = 0;
            }

            inline UUID(const uint64_t _id) 
            {
                identifyer = _id;
            }

            inline operator uint64_t() const 
            {
                return identifyer;
            }

            inline bool operator==(const UUID& _other) const
            {
                return identifyer == _other.identifyer;
            }

            inline bool operator==(const uint64_t& _indentifyer) const
            {
                return identifyer == _indentifyer;
            }

            inline bool operator!=(const UUID& _other) const
            {
                return !(identifyer == _other.identifyer);
            }

            inline bool operator!=(const uint64_t& _indentifyer) const
            {
                return identifyer != _indentifyer;
            }

            friend struct ::std::hash<UUID>;
        };
    }
}

namespace std
{
    template <>
    struct hash<DEVIOUSMUD::RANDOM::UUID>
    {
        size_t operator()(const DEVIOUSMUD::RANDOM::UUID& uuid) const
        {
            // Use the value of the UUID as the hash
            return hash<uint64_t>()(uuid.identifyer);
        }
    };
}