#include "Level.h"

Level::Level()
{

}


Level::Level(ID3D11Device * device, ID3D11DeviceContext * context)
{
	m_pD3DDevice = device;
	m_pImmediateContext = context;
}

Level::~Level()
{
	if (objects.size() != 0)
		objects.clear();
}

// adds the objects of the scene to the vector
void Level::startLevel()
{
	// populating the objects of the scene

	if (objects.size() == 0)
	{
		objects.reserve(6);
		for (size_t i = 0; i < 5; i++)
		{
			Model model = *new Model(m_pD3DDevice, m_pImmediateContext);
			model.LoadObjModel((char*)"Assets/cube.obj");
			model.AddTexture((char*)"Assets/texture.jpg");
			model.setAmbient_light_colour(g_ambient_light_colour);
			model.setDirectionalLight(g_directional_light_shines_from, g_directional_light_colour, g_directional_light_rotation);
			model.setPointLight(g_point_light_colour, g_point_light_position);
			model.setPosition(i * 3, i * 3, i * 3);

			objects.push_back(model);
		}

		// adding the ground to the objects list

		Model ground = *new Model(m_pD3DDevice, m_pImmediateContext);
		ground.setScale(30, 1, 30);
		ground.LoadObjModel((char*)"Assets/cube.obj");
		ground.AddTexture((char*)"Assets/texture.jpg");
		ground.setAmbient_light_colour(g_ambient_light_colour);
		ground.setDirectionalLight(g_directional_light_shines_from, g_directional_light_colour, g_directional_light_rotation);
		ground.setPointLight(g_point_light_colour, g_point_light_position);
		ground.setPosition(-1, -1, -1);

		objects.push_back(ground);
	}
}

// draw every object of the level
void Level::drawLevel(XMMATRIX* view, XMMATRIX* projection)
{
	if (objects.size() != 0)
	{
		for (size_t i = 0; i < objects.size(); i++)
		{
			objects.at(i).Draw(view, projection);
		}
	}
}
