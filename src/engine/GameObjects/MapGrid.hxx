#ifndef MAPGRID_HXX_
#define MAPGRID_HXX_

#include "../basics/Point.hxx"
#include <vector>

template <class T> class MapGrid
{
public:
  explicit MapGrid(T node);
  virtual ~MapGrid() = 0;

  size_t size() { return m_gridNodes.size(); };

  /**
   * @brief Add a T node to this gridnode
   *
   * @param node to add
   */
  virtual void addNode(T node);

  /**
   * @brief Remove a node on a given coordinate from this gridnode
   *
   * @param coordinate of the node
   */
  virtual void removeNode(Point coordinate);

  /**
   * @brief If this coordinate is a neighbor of one of the node in the grid.
   *
   * @param coordinate the point to check
   * @return if the given coordinate is a neighbor of this grid
   */
  bool isNeighbor(Point coordinate) const;

  /**
   * @brief If this coordinate is part of the grid.
   *
   * @param coordinate the point to check
   * @return if the given coordinate is a member of this grid
   */
  bool isMemberOf(Point coordinate) const;

  // iterators
  auto begin() { return m_gridNodes.begin(); }
  auto end() { return m_gridNodes.end(); }

protected:
  std::vector<T> m_gridNodes;
};

#include "MapGrid.inl.hxx"

#endif
