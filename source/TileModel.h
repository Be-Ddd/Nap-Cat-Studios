//
//  TileModel.h
//  ship
//
//  Created by Felicia Chen on 2/23/26.
//

#ifndef __TILE_MODEL_H__
#define __TILE_MODEL_H__

#include <cugl/cugl.h>

enum class TileType {
    FLOOR,
    WALL
};

class TileObject {
private:
    /** The row number of tile */
    int _row;
    /** The col number of tile */
    int _col;
    /** The type of tile */
    TileType _type;

public:
    /**
     * Initializes a new tile object at the given position.
     *
     * @param row   The row number
     * @param col   The column number
     * @param type The type of tile
     *
     * @return true if initialization was successful
     */
    TileObject(int row, int col, TileType type) {
        _row = row;
        _col = col;
        _type = type;
    }
    
    /** Gets the row number of tile */
    int getRow() const { return _row; }
    
    /** Gets the col number of tile */
    int getCol() const { return _col; }
    
    /** Gets the type of tile */
    TileType getType() const { return _type; }
    
    /** Returns true if it is walkable */
    bool isWalkable() const {
        return _type != TileType::WALL;
    }
};
