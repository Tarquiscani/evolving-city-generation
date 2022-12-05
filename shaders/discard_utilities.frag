////
//	Helper functions useful to discard fragments that lies between the camera and the target.
//	Note that the shader that incorporates this chunk need to define the associated uniforms. They must
//	also be assigned at runtime with the appropriate values.
////




#define PI 3.14159265358979323846





////
//	Discard the fragment if it lies above the area currently occupied by the target.
////
vec4 discard_if_abovePlayerArea(vec4 color)
{
	if(u_cameraTarget_zWorldPos < vs_world_pos.z)
	{
		float amplitude = 0.1f;			
		float outline_width = 0.08f;

		for(int i = 0; i < u_noRoofRects_size; ++i)
		{
			vec2 beg = u_noRoofRects[i*2];
			vec2 end = u_noRoofRects[i*2 + 1];

			if (   vs_world_pos.x >= beg.x && vs_world_pos.y >= beg.y
				&& vs_world_pos.x <= end.x && vs_world_pos.y <= end.y  )
			{
				discard;
			}
			
			

			/*
			// Fake/net-like transparency.
			if (   vs_world_pos.x >= beg.x && vs_world_pos.y >= beg.y
				&& vs_world_pos.x <= end.x && vs_world_pos.y <= end.y  )
			{
				if(   (int(vs_world_pos.x) + int(vs_world_pos.y)) % 2 == 0 )
					discard;
			}
			*/



			/*
			// Shaded profile for the no-roof area
			float line_thickness = 0.05f;
			int line_count = 10;
			float border_width = line_thickness * 2.f * line_count;

			if ( vs_world_pos.y >= beg.y - border_width && vs_world_pos.y <= end.y + border_width )
			{
				for(int i = 0; i < line_count; ++i)
				{
					float offset = float(i) * line_thickness * 2.f;
					if(   (vs_world_pos.x > beg.x - offset && vs_world_pos.x < beg.x - (offset - line_thickness))
					   || (vs_world_pos.x > end.x + (offset - line_thickness) && vs_world_pos.x < end.x + offset)
					   && (int(vs_world_pos.x) % 2 == 0)          )
						discard;
				}
			}

			
			if ( vs_world_pos.x >= beg.x - border_width && vs_world_pos.x <= end.x + border_width )
			{
				for(int i = 0; i < line_count; ++i)
				{
					float offset = float(i) * line_thickness * 2.f;

					if(   (vs_world_pos.y > beg.y - offset && vs_world_pos.y < beg.y - (offset - line_thickness))
					   || (vs_world_pos.y > end.y + (offset - line_thickness) && vs_world_pos.y < end.y + offset)   )
						discard;
				}
			}
			*/




			
			// Sinusoidal profile for the no-roof area
			float vertWave_length = (end.y - beg.y) / 500.f * 2 * PI;
			float y = (vs_world_pos.y - beg.y) / vertWave_length;

			float horWave_length = (end.x - beg.x) / 500.f * 2 * PI;
			float x = (vs_world_pos.x - beg.x) / horWave_length - PI;

			if (   ( vs_world_pos.x >= beg.x - amplitude * (sin(y) + 1) && vs_world_pos.x <= end.x + amplitude * (sin(y) + 1) )
				&& ( vs_world_pos.y >= beg.y - amplitude * (cos(x) + 1) && vs_world_pos.y <= end.y + amplitude * (cos(x) + 1) )   )
			{
				discard;
			}
			else if (   ( vs_world_pos.x >= beg.x - amplitude * (sin(y) + 1) - outline_width && vs_world_pos.x <= end.x + amplitude * (sin(y) + 1) + outline_width )
				     && ( vs_world_pos.y >= beg.y - amplitude * (cos(x) + 1) - outline_width && vs_world_pos.y <= end.y + amplitude * (cos(x) + 1) + outline_width )   )
			{
				color = vec4(0, 0, 0, 1);			
			}			
		}
		

		// Discard fragments lying in the intersections of no roof rects (necessary because near rects aren't adjacent, but there is some space in between)
		for(int i = 0; i < u_noRoofIntersections_size; ++i)
		{
			vec2 beg = u_noRoofIntersections[i*2];
			vec2 end = u_noRoofIntersections[i*2 + 1];

			if (   vs_world_pos.x >= beg.x && vs_world_pos.y >= beg.y
				&& vs_world_pos.x <= end.x && vs_world_pos.y <= end.y  )
			{
				discard;
			}

			
			// Sinusoidal profile for the no-roof area
			float vertWave_length = (end.y - beg.y) / 50.f * 2 * PI;
			float y = (vs_world_pos.y - beg.y) / vertWave_length;

			float horWave_length = (end.x - beg.x) / 50.f * 2 * PI;
			float x = (vs_world_pos.x - beg.x) / horWave_length - PI;

			if (   ( vs_world_pos.x >= beg.x - amplitude * (sin(y) + 1) && vs_world_pos.x <= end.x + amplitude * (sin(y) + 1) )
				&& ( vs_world_pos.y >= beg.y - amplitude * (cos(x) + 1) && vs_world_pos.y <= end.y + amplitude * (cos(x) + 1) )   )
			{
				discard;
			}
			else if (   ( vs_world_pos.x >= beg.x - amplitude * (sin(y) + 1) - outline_width && vs_world_pos.x <= end.x + amplitude * (sin(y) + 1) + outline_width )
				     && ( vs_world_pos.y >= beg.y - amplitude * (cos(x) + 1) - outline_width && vs_world_pos.y <= end.y + amplitude * (cos(x) + 1) + outline_width )   )
			{
				color = vec4(0, 0, 0, 1);	
			}	
		}

	}

	return color;
}

/*
////
//  Discard the fragment if it belongs to a circular transparent area around the target.
//	Hide everything that lies between the target and the camera.
////
vec4 discard_if_abovePlayerCircle(vec4 color)
{
	float dist_x = gl_FragCoord.x - u_viewport.x / 2;
	float dist_y = gl_FragCoord.y - u_viewport.y / 2;

	if(    u_cameraTarget_zWorldPos < vs_world_pos.z
	    && sqrt(dist_x * dist_x + dist_y * dist_y)  <= u_viewport.y / 4)
	{
		float hole_borderWidth = u_viewport.y / 500;

		if( u_viewport.y / 4 - sqrt(dist_x * dist_x + dist_y * dist_y) < hole_borderWidth)
		{
			color = vec4(0.0, 0.0, 0.0, 1.0);
		}
		else
		{
			discard;
		}
	}

	return color;
}
*/