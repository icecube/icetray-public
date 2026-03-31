/**
 *
 * SPDX-FileCopyrightText: 2025 The IceTray Contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Definition of I3XDOMLaunch class
 *
 * @file I3XDOMLaunch.h
 * @date 2025-06-02
 * @author Schmidt
 * @author Stuttard
 *
 */
#ifndef UPGRADELCFLAGS_H_INCLUDED
#define UPGRADELCFLAGS_H_INCLUDED

#include <cstdint>
#include <bitset>

enum UpgradeLCFlags : std::uint8_t {
  NoLC = 0,                  // Either no LC is set or no LC conditions satisfied
  OnboardLC = (1 << 0),      // Onboard noise suppression LC for mDOMs
  SingleModuleLC = (1 << 1), // Offline LC with multiple PMTs on a single module
  MultiModuleLC = (1 << 2),  // Offline LC across multiple modules
  DEggInclusiveLC = (1 << 3),// Offline LC including at least 1 DEgg
  DEggExclusiveLC = (1 << 4),// Offline LC including only DEggs
  NonUniformLC = (1 << 5),   // Offline LC including mDOMs and DEggs with separate settings
};

constexpr
UpgradeLCFlags operator|(UpgradeLCFlags lhs, UpgradeLCFlags rhs)
{
  return static_cast<UpgradeLCFlags>(
      static_cast<std::uint8_t>(lhs) |
      static_cast<std::uint8_t>(rhs));
}

constexpr
UpgradeLCFlags operator&(UpgradeLCFlags lhs, UpgradeLCFlags rhs)
{
  return static_cast<UpgradeLCFlags>(
      static_cast<std::uint8_t>(lhs) &
      static_cast<std::uint8_t>(rhs));
}

constexpr
UpgradeLCFlags operator^(UpgradeLCFlags lhs, UpgradeLCFlags rhs)
{
  return static_cast<UpgradeLCFlags>(
      static_cast<std::uint8_t>(lhs) ^
      static_cast<std::uint8_t>(rhs));
}

constexpr
UpgradeLCFlags operator~(UpgradeLCFlags f)
{
  return static_cast<UpgradeLCFlags>(
      ~static_cast<std::uint8_t>(f));
}

inline
UpgradeLCFlags& operator|=(UpgradeLCFlags& lhs, UpgradeLCFlags rhs)
{
  lhs = lhs | rhs;
  return lhs;
}

inline
UpgradeLCFlags& operator&=(UpgradeLCFlags& lhs, UpgradeLCFlags rhs)
{
  lhs = lhs & rhs;
  return lhs;
}

inline
UpgradeLCFlags& operator^=(UpgradeLCFlags& lhs, UpgradeLCFlags rhs)
{
  lhs = lhs ^ rhs;
  return lhs;
}

inline
std::ostream& operator<<(std::ostream& os, const UpgradeLCFlags& flags){
  using U = std::underlying_type_t<UpgradeLCFlags>;
  return os << std::bitset<8>(static_cast<U>(flags));
}

#endif  // UPGRADELCFLAGS_H_INCLUDED
