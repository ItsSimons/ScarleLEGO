#pragma once

#include "DebugExtractor.h"

//Takes the output value of the extractor and visualizes the vertices
class DebugRender
{
public:
	explicit DebugRender(ID3D11Device* _d3dDevice, ID3D11DeviceContext1* _d3dContext)
		: d3dDevice(_d3dDevice), d3dContext(_d3dContext), extractor()
	{
	}

	//Deletes all the memory allocated by the debugger on exit
	~DebugRender()
	{
		while(!debug_cubes.empty())
		{
			delete debug_cubes[debug_cubes.size()-1];
			debug_cubes.pop_back();
		}
	}

	void update(GameData* GD, q3Scene* physic_scene)
	{
		physic_scene->Render(&extractor);

		//Get the data from the extractor
		auto& debug_cubes_pos = extractor.GetDebugCubes();
		
		//Pops cubes off if they are not longer in need to be rendered
		while(debug_cubes_pos.size() < debug_cubes.size())
		{
			delete debug_cubes[debug_cubes.size()-1];
			debug_cubes.pop_back();
		}
		//Pushes cubes back in if they need to be rendered
		while(debug_cubes_pos.size() > debug_cubes.size())
		{
			//Hitbox cubes are bright pink
			cube = new GPGO(d3dContext, GPGO_CUBE, (float*)&Colors::HotPink, params);
			debug_cubes.push_back(cube);
		}

		//assigns the position of the vertices to each cube to visualize
		//the hit box
		int i = 0;
		for(const auto& _cube : debug_cubes)
		{
			_cube->SetPos(debug_cubes_pos[i]);
			_cube->Tick(GD);
			i++;
		}
		
		//When done clears the cube vector
		debug_cubes_pos.clear();
	}

	void render(DrawData* DD) 
	{
		for(const auto& _cube: debug_cubes)
		{
			_cube->Draw(DD);
		}
	}

private:
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext1* d3dContext;
	
	std::vector<GameObject*> debug_cubes{};
	GPGO* cube = nullptr;

	DebugExtractor extractor;
	//Size of the cubes
	float params[1] = {1.f};
};