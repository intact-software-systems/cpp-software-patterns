#include"RxData/cache/CacheDescription.h"

namespace RxData
{

/**
 * @brief CacheDescription::CacheDescription
 * @param name
 */
CacheDescription::CacheDescription(std::string name)
{
    this->name_ = name;
}

/**
 * @brief CacheDescription::~CacheDescription
 */
CacheDescription::~CacheDescription()
{

}

/**
 * @brief CacheDescription::getName
 * @return
 */
std::string CacheDescription::getName() const
{
    return name_;
}

/**
 * @brief CacheDescription::operator ==
 * @param other
 * @return
 */
bool CacheDescription::operator==(const CacheDescription &other) const
{
    return this->name_ == other.name_;
}

/**
 * @brief CacheDescription::operator !=
 * @param other
 * @return
 */
bool CacheDescription::operator!=(const CacheDescription &other) const
{
    return this->name_ != other.name_;
}

/**
 * @brief CacheDescription::operator <
 * @param other
 * @return
 */
bool CacheDescription::operator<(const CacheDescription &other) const
{
    return this->name_.length() < other.name_.length();
}

/**
 * @brief CacheDescription::operator >
 * @param other
 * @return
 */
bool CacheDescription::operator>(const CacheDescription &other) const
{
    return this->name_.length() > other.name_.length();
}

}
