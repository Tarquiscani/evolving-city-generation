#include "hip_roof_matrix.hh"


namespace tgm
{



namespace HipRoofAlgorithm
{
    ////
    //	Used at the beginning. Fill the roofable microtiles with the maximum height of the roof.
    ////
    void Matrix::fill(Vector2i const pos)
    {
        auto const max_height = std::max(m_length, m_width);

        get(pos).fill(max_height);
    }


    void Matrix::project_border(Vector2i const v1_pos, Vector2i const v1_versor, 
                        Vector2i const v2_pos, Vector2i const v2_versor, 
                        Vector2i const side_versor)
    {
        auto inclination = sideVersor_to_microtileInclination(side_versor);

        auto max_height = std::max(m_length, m_width) / 2;

        //For each segment of this projection (of microtiles with the same height)
        for (int height = 0; height < max_height; ++height)
        {
            // Diagonal projection of the vertices. 
            auto starting_pos = v1_pos + v1_versor * height;
            // Make pastTheEnd_pos not the last microtile of the border, but the past-the-end microtile of the border to simplify the while iteration.
            auto pastTheEnd_pos = v2_pos + v2_versor * height + side_versor;
                    


            auto current_pos = starting_pos;
            while (current_pos != pastTheEnd_pos)
            {
                if(in_range(current_pos))
                {
                    get(current_pos).update(height, inclination);
                }

                current_pos += side_versor;
            }

            // If we have a triangle-shaped projection and have already reached the top of the triangle, the projection in complete
            if (starting_pos == pastTheEnd_pos - side_versor)
                break;
        }
    }
}



} //namespace tgm