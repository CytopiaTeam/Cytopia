#include<algorithm>

template<class T>
MapGrid<T>::MapGrid(T node) : m_gridNodes{node} {}

template<class T>
MapGrid<T>::~MapGrid()
{
  m_gridNodes.clear();
}

template<class T>
void MapGrid<T>::addNode(T node)
{
  m_gridNodes.push_back(node);
}

template<class T>
void MapGrid<T>::removeNode(Point coordinate)
{
  m_gridNodes.erase(std::remove_if(m_gridNodes.begin(), m_gridNodes.end(),
        [coordinate](const T &node) {
        return node.coordinate == coordinate;
        }),
      m_gridNodes.end());
}

template<class T>
bool MapGrid<T>::isNeighbor(Point coordinate) const
{
  for (const T &node : m_gridNodes)
  {
    if (node.coordinate.isDirectNeighborOf(coordinate))
    {
      return true;
    }
  }
  return false;
}

template<class T>
bool MapGrid<T>::isMemberOf(Point coordinate) const
{
  return m_gridNodes.end() != std::find_if(m_gridNodes.begin(), m_gridNodes.end(),
      [&coordinate](const T &node) {
      return node.coordinate == coordinate;
      });
}

