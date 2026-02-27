//
//  TileModel.h
//  ship
//
//  Created by Felicia Chen on 2/23/26.
//

#ifndef __TILE_MODEL_H__
#define __TILE_MODEL_H__

#include <cugl/cugl.h>
#include "ValuableSet.h"

class TileObject {
private:
    /** The row number of tile */
    int _row;
    /** The col number of tile */
    int _col;
    /** The pointer to valuable if there is one */
    std::shared_ptr<ValuableSet::Valuable> _valuable;

public:
    TileObject() : _row(0), _col(0), _valuable(nullptr) {}
    /**
     * Initializes a new tile object at the given position.
     *
     * @param row   The row number
     * @param col   The column number
     * @param type The type of tile
     *
     * @return true if initialization was successful
     */
    TileObject(int row, int col,std::shared_ptr<ValuableSet::Valuable> val = nullptr)
        : _row(row), _col(col), _valuable(val) {}
    
    /** Gets the row number of tile */
    int getRow() const { return _row; }
    
    /** Gets the col number of tile */
    int getCol() const { return _col; }
    
    /** Returns true if there is valuable */
    bool hasValuable() const {
        return _valuable != nullptr;
    }
    
    /** Access the valuable */
    std::shared_ptr<ValuableSet::Valuable> getValuable() const {
        return _valuable;
    }
};

class TileModel {
private:
    int _rows;
    int _cols;
    float _tileSize;
    cugl::Vec2 _origin;
    std::vector<std::vector<TileObject>> _tiles;
    
public:
    bool init (int rows, int cols, float tileSize, float leftoffset, float bottomoffset) {
        _rows = rows;
        _cols = cols;
        _tileSize = tileSize;
        _origin = cugl::Vec2(leftoffset, bottomoffset);
        _tiles.resize(_rows);
        for (int r = 0; r < _rows; r++) {
            _tiles[r].resize(_cols);
            for (int c = 0; c < _cols; c++) {
                _tiles[r][c] = TileObject(r, c);
            }
        }
        CULog("TILEMODEL INIT");
        return true;
    };
    
    std::pair<int,int> worldToGrid(const cugl::Vec2& world) const {
        int grid_x = static_cast<int>((world.x - _origin.x) / _tileSize);
        int grid_y = static_cast<int>((world.y - _origin.y) / _tileSize);
        return {grid_x, grid_y};
    }
};

#endif
