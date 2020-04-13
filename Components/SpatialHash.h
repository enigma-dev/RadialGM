/**
 * @file SpatialHash.h
 * @brief Implements a class for sparse spatial hashing collision detection.
 * @section License
 * Copyright (C) 2017, 2020 Robert Colton
 * License pending. All rights reserved.
 */

#ifndef SPATIALHASH_H
#define SPATIALHASH_H

#include <iterator>
#include <unordered_map>
#include <vector>
#include <algorithm>

struct Point {
  int x, y;
  Point(int x, int y) : x(x), y(y) {}

  friend inline bool operator==(Point const& lhs, Point const& rhs)
  {
    return (lhs.x == rhs.x) && (lhs.y == rhs.y);
  }
};

template<typename T>
class SpatialHash {
  struct PointHash {
    inline std::size_t operator()(const Point &v) const {
      return v.x * 31 + v.y;
    }
  };

  int cell_width, cell_height;
  using CellBucket = std::pair<std::vector<void*>,std::vector<void*>>;
  std::unordered_map<Point, CellBucket, PointHash> cells;

public:
  SpatialHash() : cell_width(64), cell_height(64) {};
  SpatialHash(int cell_width, int cell_height) :
    cell_width(cell_width), cell_height(cell_height) {}

  void setCellSize(const int cell_width, const int cell_height) {
    this->cell_width = cell_width;
    this->cell_height = cell_height;
  }

  void setCellWidth(const int cell_width) {
    this->cell_width = cell_width;
  }

  void setCellHeight(const int cell_height) {
    this->cell_height = cell_height;
  }

  int getCellWidth() const {
    return cell_width;
  }

  int getCellHeight() const {
    return cell_height;
  }

  void addPoint(void *const userdata) {
    T t(userdata);
    cells[Point(t.x() / cell_width, t.y() / cell_height)].first.push_back(userdata);
  }

  void addRectangle(void *const userdata) {
    T t(userdata);
    int x = t.x(), y = t.y(), width = t.width(), height = t.height();
    int xx = x / cell_width, yy = y / cell_height;
    for (int i = xx; i < ((x + width) / cell_width) + 1; ++i) {
      for (int ii = yy; ii < ((y + height) / cell_height) + 1; ++ii) {
        auto& cell = cells[Point(i, ii)];
        const bool origin = (i == xx && ii == yy);
        auto& cellProxies = origin ? cell.first : cell.second;
        cellProxies.push_back(userdata);
      }
    }
  }

  void removeProxy(void *const userdata) {
    T t(userdata);
    int x = t.x(), y = t.y(), width = t.width(), height = t.height();
    int xx = x / cell_width, yy = y / cell_height;
    for (int i = xx; i < ((x + width) / cell_width) + 1; ++i) {
      for (int ii = yy; ii < ((y + height) / cell_height) + 1; ++ii) {
        auto& cell = cells[Point(i, ii)];
        const bool origin = (i == xx && ii == yy);
        auto& cellProxies = origin ? cell.first : cell.second;
        const auto it = std::find(cellProxies.begin(), cellProxies.end(), userdata);
        cellProxies.erase(it);
      }
    }
  }

  std::vector<void*> queryWindow(const int x, const int y, const int width, const int height) {
    std::vector<void*> hits;
    const int xx = x / cell_width, yy = y / cell_height;
    for (int i = xx; i < ((x + width) / cell_width) + 1; ++i) {
      for (int ii = yy; ii < ((y + height) / cell_height) + 1; ++ii) {
        auto& cell = cells[Point(i, ii)];
        for (auto userdata : cell.first) {
          hits.push_back(userdata);
        }
        for (auto userdata : cell.second) {
          T t(userdata);
          auto px = t.x() / cell_width,
               py = t.y() / cell_height;
          // already looked at this proxy?
          if (std::max(px, xx) < i || std::max(py, yy) < ii) continue;
          hits.push_back(userdata);
        }
      }
    }
    return hits;
  }

  void clear() {
    std::unordered_map<Point, CellBucket, PointHash>().swap(cells);
  }
};

#endif // SPATIALHASH_H
