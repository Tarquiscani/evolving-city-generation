#include "hip_roof_matrix_visual_debug.hh"
#if HIPROOFMATRIX_VISUALDEBUG


namespace tgm
{



void HipRoofMatrixVisualDebug::start(int const map_length, int const map_width)
{
    base_start(map_length * 2 + 4, map_width * 2 + 4, 1); //Make a map large enough to hold any possible roof matrix.
}


void HipRoofMatrixVisualDebug::set_matrixDimensions(int const matrix_length, int const matrix_width)
{
    change_frame({ 0, 0, 0, matrix_length, matrix_width, 1 });
}


void HipRoofMatrixVisualDebug::print_matrix(HipRoofAlgorithm::Matrix const& matrix)
{
    validate_inStepAction();
    if (!is_recordingEnabled())
        return;


    using HipRoofAlgorithm::Microtile;


    std::vector<TileHighlightingWithInfo> starting_hls;
    std::vector<TileHighlightingWithInfo> north_hls;
    std::vector<TileHighlightingWithInfo> east_hls;
    std::vector<TileHighlightingWithInfo> south_hls;
    std::vector<TileHighlightingWithInfo> west_hls;
    std::vector<TileHighlightingWithInfo> mixed_hls;
    std::vector<TileHighlightingWithInfo> flat_hls;

    for(int y = 0; y < matrix.width(); ++y)
        for (int x = 0; x < matrix.length(); ++x)
        {
            auto microtile = matrix.get(x, y);
                    
            if (microtile.height() == -1)
                continue;


            std::ostringstream oss;
            oss << "Height: " << microtile.height() << "  -  Inclination: ";


            auto incl = microtile.inclinations();


            if ((incl & Microtile::north).any())
                oss << "/N";
            if ((incl & Microtile::east).any())
                oss << "/E";
            if ((incl & Microtile::south).any())
                oss << "/S";
            if ((incl & Microtile::west).any())
                oss << "/W";
            if ((incl & Microtile::flat).any())
                oss << "/Flat";
                    
            oss << "     " << incl.to_string();
            oss << "  -  Type: " << microtile.type();

            if      (incl.none())
            {
                starting_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else if ((incl | Microtile::north) == Microtile::north)
            {
                north_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else if ((incl | Microtile::east) == Microtile::east)
            {
                east_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else if ((incl | Microtile::south) == Microtile::south)
            {
                south_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else if ((incl | Microtile::west) == Microtile::west)
            {
                west_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else if ((incl | Microtile::flat) == Microtile::flat)
            {
                flat_hls.push_back({ {x, y, 0}, oss.str() });
            }
            else
            {
                mixed_hls.push_back({ {x, y, 0}, oss.str() });
            }
        }
            
    highlight_tiles(starting_hls.begin(), starting_hls.end(), Color::Black);
    highlight_tiles(north_hls.begin(), north_hls.end(), Color(0x808000FF)); //Olive
    highlight_tiles(east_hls.begin(), east_hls.end(), Color(0xf58231FF)); //Orange
    highlight_tiles(south_hls.begin(), south_hls.end(), Color(0xffe119FF)); //Yellow
    highlight_tiles(west_hls.begin(), west_hls.end(), Color(0xfabebeFF)); //Pink
    highlight_tiles(mixed_hls.begin(), mixed_hls.end(), Color(0x800000FF)); //Maroon
    highlight_tiles(flat_hls.begin(), flat_hls.end(), Color(0x42d4f4FF)); //Cyan
}



} // namespace tgm


#endif //HIPROOFMATRIX_VISUALDEBUG