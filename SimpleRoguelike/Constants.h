#pragma once
#include <iostream>
#include <bitset>

using Entity = std::uint16_t;
using Sprite = std::uint16_t;
using Tileset = std::string;
using ComponentType = std::uint8_t;

const Entity MAX_ENTITIES = 5000;
const Entity NULL_ENTITY = 0;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

const int MAX_MAP_SIZE = 100;