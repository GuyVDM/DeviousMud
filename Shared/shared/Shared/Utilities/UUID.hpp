#pragma once
#include <random>
#include <chrono>
#include <limits>

namespace DM 
{
    namespace Utils 
    {
        /// <summary>
        /// Universal unique identifyer. Generates a unsigned 64 bit unique handle. 
        /// </summary>
        class UUID
        {
        private:
            uint64_t m_identifyer;

        public:
            inline static UUID generate()
            {
                uint64_t identifyer;

                // Get current timestamp
                uint64_t timestamp = static_cast<uint64_t>(std::time(nullptr));

                // Generate random bits
                std::random_device rd;
                std::mt19937_64 gen(rd());

                constexpr uint64_t maxValue = ULLONG_MAX;

                std::uniform_int_distribution<uint64_t> dis(0, maxValue);
                uint64_t randomBits = dis(gen);

                // Combine timestamp and random bits
                identifyer = (timestamp << 32) | (randomBits & 0xFFFFFFFF);
                return UUID(identifyer);
            }

            inline UUID() 
            {
                m_identifyer = 0;
            }

            inline UUID(const uint64_t _id) 
            {
                m_identifyer = _id;
            }

            inline operator uint64_t() const 
            {
                return m_identifyer;
            }

            inline bool operator==(const UUID& _other) const
            {
                return m_identifyer == _other.m_identifyer;
            }

            inline bool operator==(const uint64_t& _indentifyer) const
            {
                return m_identifyer == _indentifyer;
            }

            inline bool operator!=(const UUID& _other) const
            {
                return !(m_identifyer == _other.m_identifyer);
            }

            inline bool operator!=(const uint64_t& _indentifyer) const
            {
                return m_identifyer != _indentifyer;
            }

            friend struct ::std::hash<UUID>;
        };
    }
}

namespace std
{
    template <>
    struct hash<DM::Utils::UUID>
    {
        size_t operator()(const DM::Utils::UUID& uuid) const
        {
            // Use the value of the UUID as the hash
            return hash<uint64_t>()(uuid.m_identifyer);
        }
    };
}