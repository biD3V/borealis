#pragma once

#include <borealis/absolute_layout.hpp>
#include <borealis/image.hpp>
#include <borealis/label.hpp>
#include <borealis/button.hpp>
#include <borealis/rectangle.hpp>
#include <borealis/scroll_view.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

namespace brls
{
    struct IndexPath {
        int row;
        int column;
    };

    class GridItem : public Button
    {
        protected:
            std::string label;

        public:
            GridItem(std::string label = "");
            //~GridItem();
            
    };

    // class GridContentView : public AbsoluteLayout
    // {
    //     public:
    //         GridContentView(Grid* grid, unsigned rows, unsigned columns);
            
    //     private:
    //         Grid *grid;
    // };

    class Grid : public AbsoluteLayout
    {
        private:
            int rows;
            int columns;
            brls::NavigationMap navigationMap;

        public:
            Grid(int row, int column);
            //~Grid();

            brls::View* getDefaultFocus() override;
            brls::View* getNextFocus(brls::FocusDirection direction, brls::View* currentView) override;
            void layout(NVGcontext* vg, brls::Style* style, brls::FontStash* stash);
            int getIndex(int row, int column);
            IndexPath getIndexPath(int i);
    };
} // namespace brls
