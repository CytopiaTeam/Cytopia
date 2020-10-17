#ifndef ISOMATH_HXX_
#define ISOMATH_HXX_

#include <vector>
#include <algorithm>
#include <SDL.h>

#include "point.hxx"

// calculate clicked column (x coordinate) without heigh taken into account.
/** \brief Calculates screen space coordinates to isometric space coordinates.
* A formula is used to calculate the iso coordinates from given screen coordinates. No tile height is taken into account.
* @note This function will not work for elevated tiles
* \returns Point() - object containing the calculated isometric coordinates of the tile that matches the screen coordinates
* @param Point() screenCoordinates - object containing screen space coordinates
*/
Point calculateIsoCoordinates(const SDL_Point &screenCoordinates);

/** \brief converts coordinates from isometric to screen space
* The given isometric coordinates (which contain height information) are converted to the screen coordinates. The coordinates represent the x, y position of the
* tile where it is drawn (if tile height / width is added, the whole bounding box could be calculated)
* Camera Offset and current zoomLevel is taken into account
*  \returns Point() object containing screen space coordinates
* @param Point() isoCoordinates - object containing isometric coordinates
* @param bool calcWithoutOffset - optional parameter to calculate screenspace coordinates without zoomLevel and cameraOffset taken into account
*/
SDL_Point convertIsoToScreenCoordinates(const Point &isoCoordinates, bool calcWithoutOffset = false);

/** \brief converts screen space coordinates to isometric space coordinates.
* To convert screen coordinates in to isometric coordinates, all the textures inside the nodes are checked, if the click is inside the bounding box of a texture
* and if so, another check ensures, that the click is not on a transparent pixel. The isometric coordinates of the map node containing the texture with
* the highest Z-Level (drawing order) is returned.
* Camera Offset and current zoomLevel is taken into account
* \returns Point() - object containing the isometric coordinates of the tile that matches the screen coordinates
* @param Point() screenCoordinates - object containing screen space coordinates
*/
Point convertScreenToIsoCoordinates(const SDL_Point &screenCoordinates);

/** \brief Check if given coordinates are within map boundaries
* Checks if coordinates are within map boundaries
* @param Point object - coordinates to check
* @return bool - true if coordinates are inside the map bounds.
*/
bool isPointWithinMapBoundaries(int x, int y);
bool isPointWithinMapBoundaries(const Point &isoCoordinates);
bool isPointWithinMapBoundaries(const std::vector<Point> &isoCoordinates);

/** \brief Creates a line between two points using the Bresenham Line algorithm
* Creates a line between two points using the Bresenham Line algorithm
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
std::vector<Point> createBresenhamLine(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

/** \brief Gets all nodes in a rectangle between start and end point
* Gets all nodes in a rectangle between start and end point
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
std::vector<Point> getRectangleSelectionNodes(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

/** \brief Gets all nodes in a rectangular line from start and end point
* Gets all nodes in a rectangular line between start and end point
* @param Point() - start coordinates
* @param Point() - end coordinates
* @return std::vector<Point>() - contains coordinates for each tile between start and end coordinates, including start and end
*/
std::vector<Point> getRectangularLineSelectionNodes(const Point &isoCoordinatesStart, const Point &isoCoordinatesEnd);

/// Clamp value
//TODO: Remove this when switching to C++17 and use std::clamp instead
template <typename T> T clamp(const T &n, const T &lower, const T &upper) { return std::max(lower, std::min(n, upper)); }

#endif
