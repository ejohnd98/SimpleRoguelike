#pragma once
#include <iostream>
#include <bitset>

using Entity = std::uint16_t;
using ComponentType = std::uint8_t;

const Entity MAX_ENTITIES = 5000;
const Entity NULL_ENTITY = 0;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;