#include "gimp_square.hh"


#include "settings/graphics_settings.hh"


namespace tgm
{



GIMPSquare::GIMPSquare(float const left_ld,  float const top_ld,
					   float const left_hd,  float const top_hd) :
	GIMPSquare(left_ld,  top_ld,  GSet::lowDefinition_pptxu,       GSet::lowDefinition_pptxu,
			   left_hd,  top_hd,  GSet::highDefinition_pptxu,      GSet::highDefinition_pptxu) {}


GIMPSquare::GIMPSquare(float const left_ld,  float const top_ld,  float const width_ld,  float const height_ld,
					   float const left_hd,  float const top_hd,  float const width_hd,  float const height_hd)
{

	switch (GSet::texture_definition())
	{
		case TextureDefinition::LowDefinition:
			{
				left = left_ld;
				top = top_ld;
				width = width_ld;
				height = height_ld;
				break;
			}

		case TextureDefinition::HighDefinition:
			{
				left = left_hd;
				top = top_hd;
				width = width_hd;
				height = height_hd;
				break;
			}

		case TextureDefinition::UltraHighDefinition:
			{
				// Normally sprites in 4k are just the double of sprites in Full HD.
				auto hdToUhd_ratio = GSet::ultraHighDefinition_pptxu / GSet::highDefinition_pptxu;
				left = left_hd * hdToUhd_ratio;
				top = top_hd * hdToUhd_ratio;
				width = width_hd * hdToUhd_ratio;
				height = height_hd * hdToUhd_ratio;
				break;
			}

		default:
			throw std::runtime_error("Unexpected texture definition.");
			break;
	}
}


auto operator<<(std::ostream & os, GIMPSquare const& gs) -> std::ostream &
{
	os << "GIMPSquare { left: " << gs.left << ", top: " << gs.top << ", width: " << gs.width << ", height: " << gs.height << "} ";

	return os;
}



} // namespace tgm