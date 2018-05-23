#pragma once

#include <QVector2D>
#include <limits>
#include <functional>
#include <QMouseEvent>
#include <memory>
#include <type_traits>
#include <iostream>

/* filename of the config file */
constexpr char config_path[] = "config.json";

constexpr int animFrameMS = 16;
constexpr int drawFrameMS = 16;

constexpr double DOUBLEINF = std::numeric_limits<double>::max();
