#include <math.h>

#include <borealis/animations.hpp>
#include <borealis/application.hpp>
#include <borealis/dropdown.hpp>
#include <borealis/header.hpp>
#include <borealis/i18n.hpp>
#include <borealis/grid.hpp>
#include <borealis/logger.hpp>
#include <borealis/swkbd.hpp>
#include <borealis/table.hpp>

using namespace brls::i18n::literals;

namespace brls
{
    GridItem::GridItem(std::string label)
        : Button(ButtonStyle::REGULAR)
        , label(label)
    {
        
    }

    Grid::Grid(int rows, int columns)
        : rows(rows)
        , columns(columns)
    {
        size_t totalViews = this->rows * this->columns;

        for (size_t i = 0; i < totalViews; i++)
        {
            GridItem *item = new GridItem();
            this->addView(item);
        }

        for (size_t i = 0; i < this->getViewCount(); i++)
        {
            IndexPath indexPath = getIndexPath(i);
            size_t curRow = indexPath.row;
            size_t curCol = indexPath.column;
            
            this->navigationMap.add(
                this->getChild(i),
                brls::FocusDirection::RIGHT,
                this->getChild(getIndex(curRow,(curCol + 1) % this->columns))
            );

            this->navigationMap.add(
                this->getChild(i),
                brls::FocusDirection::LEFT,
                this->getChild(getIndex(curRow,(curCol - 1) % this->columns))
            );

            this->navigationMap.add(
                this->getChild(i),
                brls::FocusDirection::DOWN,
                this->getChild(getIndex((curRow + 1) % this->rows,curCol))
            );

            this->navigationMap.add(
                this->getChild(i),
                brls::FocusDirection::UP,
                this->getChild(getIndex((curRow - 1) % this->rows,curCol))
            );
        } 
    }

    void Grid::layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash) {
        int x = this->getX();
        int y = this->getY();

        int padding = 25;

        int buttonWidth = ((this->width - padding * 2) / this->columns) - padding;
        int buttonHeight = ((this->height - padding * 2) / this->rows) - padding;

        for (size_t i = 0; i < this->getViewCount(); i++)
        {
            IndexPath indexPath = getIndexPath(i);
            
            this->getChild(i)->setBoundaries(
                x + padding + ((padding + buttonWidth) * indexPath.column),
                y + padding + ((padding + buttonHeight) * indexPath.row),
                buttonWidth,
                buttonHeight
            );
        }
        
    }

    brls::View* Grid::getDefaultFocus()
    {
        return this->getChild(0)->getDefaultFocus();
    }

    brls::View* Grid::getNextFocus(brls::FocusDirection direction, brls::View* currentView)
    {
        return this->navigationMap.getNextFocus(direction,currentView);
    }

    int Grid::getIndex(int row, int column)
    {
        return (row * this->columns) + column;
    }

    IndexPath Grid::getIndexPath(int i)
    {
        struct IndexPath indexPath;
        if (i < this->columns) {
            indexPath.row = 0;
            indexPath.column = i;
        } else {
            indexPath.row = floor(i / this->columns);
            indexPath.column = i % this->columns;
        }
        return indexPath;
    }
}