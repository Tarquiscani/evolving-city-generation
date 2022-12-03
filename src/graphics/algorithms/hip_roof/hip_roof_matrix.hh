#ifndef GM_HIP_ROOF_MATRIX_HH
#define GM_HIP_ROOF_MATRIX_HH


#include <algorithm>
#include <vector>

#include "hip_roof_microtile.hh"
#include "settings/graphics_settings.hh"
#include "system/vector2.hh"
#include "system/vector3.hh"


namespace tgm
{



namespace HipRoofAlgorithm
{
	class Matrix
	{
		private:
			using MicroCont = std::vector<Microtile>;

		public:
			Matrix(int const roofableZone_minX, int const roofableZone_maxX, int const roofableZone_minY, int const roofableZone_maxY) : 
				m_position{ roofableZone_minX - 1, roofableZone_minY - 1 },
				m_length{ (roofableZone_maxX + 1 - roofableZone_minX) * 2 + 5 }, 
				m_width{ (roofableZone_maxY + 1 - roofableZone_minY) * 2 + 5 }, 
				m_matrix(static_cast<MicroCont::size_type>(m_length) * m_width) 
			{
				if ((roofableZone_maxX - roofableZone_minX) < 0 || (roofableZone_maxY - roofableZone_minY) < 0)
				{
					std::cout << "roofableZone_minX: " << roofableZone_minX 
							  << "\nroofableZone_maxX: " << roofableZone_maxX 
							  << "\nroofableZone_minY: " << roofableZone_minY 
							  << "\nroofableZone_maxY: " << roofableZone_maxY << std::endl;

					throw std::runtime_error("Matrices with negative dimensions are not allowed. The smallest roofable areas is 1x1 (1 map tile).");
				}
			}

			auto length() const noexcept -> int { return m_length; }
			auto width() const noexcept -> int { return m_width; }
			
			auto get(Vector2i pos) -> Microtile & { return get(pos.x, pos.y); }
			auto get(Vector2i pos) const -> Microtile const& { return get(pos.x, pos.y); }

			auto get(int const x, int const y)		 -> Microtile &		 { assert_in_range(x, y); return m_matrix[static_cast<MicroCont::size_type>(y) * m_length + x]; }
			auto get(int const x, int const y) const -> Microtile const& { assert_in_range(x, y); return m_matrix[static_cast<MicroCont::size_type>(y) * m_length + x]; }
			
			////
			//	Useful function to iterate counterwise through the neighbors of the specified @pos.
			//	@nghb_id: A number ranging from '0' to '7'. '0' identifies the northern neighbor, '7' identifies the north-western neighbor.
			////
			#pragma warning(suppress: 4068)
			#pragma GCC diagnostic push
			#pragma warning(suppress: 4068)
			#pragma GCC diagnostic ignored "-Wreturn-type"
			#pragma warning(suppress: 4715)
			auto get_neighbor(Vector2i const pos, int const nghb_id) const -> Microtile const&
			{
				switch (nghb_id % 8)
				{
					case 0:
						return get(pos + Vector2i(-1, 0));
						break;
					case 1:
						return get(pos + Vector2i(-1, 1));
						break;
					case 2:
						return get(pos + Vector2i(0, 1));
						break;
					case 3:
						return get(pos + Vector2i(1, 1));
						break;
					case 4:
						return get(pos + Vector2i(1, 0));
						break; 
					case 5:
						return get(pos + Vector2i(1, -1));
						break;
					case 6:
						return get(pos + Vector2i(0, -1));
						break;
					case 7:
						return get(pos + Vector2i(-1, -1));
						break;
				}			
			}
			#pragma GCC diagnostic pop


			////
			//	Used at the beginning. Fill the microtiles of the roofable positions with the maximum height of the roof.
			////
			void fill(Vector2i const pos);

			////
			//	Project the border inside the matrix with increasing heights.
			//	@v1_pos, @v1_versor: Position and verse of projection of the first vertex of the border.
			//	@v2_pos, @v2_versor: Position and verse of projection of the second vertex of the border.
			//	@side_versor: Versor that point from the first vertex towards the second vertex.
			////
			void project_border(Vector2i const v1_pos, Vector2i const v1_versor,
								Vector2i const v2_pos, Vector2i const v2_versor,
								Vector2i const side_versor);
			

			////
			//	@pos: (map reference system -- in tiles)
			//	@return: (matrix reference system -- in microtiles)
			////
			auto tile_to_microtile(Vector3i const pos) const noexcept -> Vector2i
			{
				return { ((pos.x - m_position.x) * 2) + 1,
						 ((pos.y - m_position.y) * 2) + 1 };
			}
			
			////
			//	@pos: (in microtiles -- matrix reference system)
			//
			//	@return: (in units -- map reference system)
			////
			auto microtile_to_mapUnits(Vector2i const pos) const noexcept -> Vector2f
			{
				return { GSet::tiles_to_units(pos.x / 2.f + m_position.x),
						 GSet::tiles_to_units(pos.y / 2.f + m_position.y)  };
			}


		private:
			Vector2i m_position;

			int m_length = 0;
			int m_width = 0;
			MicroCont m_matrix;

			bool in_range(Vector2i const pos) { return pos.x >= 0 && pos.x < m_length && pos.y >= 0 && pos.y < m_width; }


			void assert_in_range(int const x, int const y) const
			{
				if (x < 0 || x >= m_length || y < 0 || y >= m_width)
					throw std::runtime_error("Out-of-range matrix coordinates.");
			}

			auto sideVersor_to_microtileInclination(Vector2i const side_versor) const -> MicrotileInclination
			{
				if (side_versor == Vector2i{  0,  1 })
					return MicrotileInclination::South;
				else if (side_versor == Vector2i{  1,  0 })
					return MicrotileInclination::West;
				else if (side_versor == Vector2i{  0, -1 })
					return MicrotileInclination::North;
				else if (side_versor == Vector2i{ -1,  0 })
					return MicrotileInclination::East;
				else
					throw std::runtime_error("Unexpected side_versor.");
			}
	};
}



} // namespace tgm


#endif //GM_HIP_ROOF_MATRIX_HH