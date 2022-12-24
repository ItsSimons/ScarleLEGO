#pragma once

//Scarle Headers
#include "GameData.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"
#include "q3.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

//Lego Headers
#include <iostream>
#include "UserInterface.h"
#include "DebugRender.h"
#include "CustomBaseObject.h"
#include "BlockIndex.h"

namespace LEGO
{
	class Handler
	{
	public:
		explicit Handler(GameData* _GD, DrawData* _DD, DrawData2D* _DD2D, ID3D11Device* _d3dDevice,
						 ID3D11DeviceContext1* _d3dContext, IEffectFactory* _fxFactory);
		~Handler();

		void initialize(const Vector2& resolution);

		void loadFromPath(const std::string& path);
		void saveToPath(const std::string& path);

		//Scarle
		void update();
		void render();
		void readInput();

	private:
		//Scarle and DX11 pointers
		GameData* GD = nullptr;
		DrawData* DD = nullptr;
		DrawData2D* DD2D = nullptr;
		ID3D11Device* d3dDevice = nullptr;
		ID3D11DeviceContext1* d3dContext = nullptr;
		IEffectFactory* fxFactory = nullptr;
		
		//Physic scene 
		q3Scene* physic_scene = nullptr;
		//Pointer to the "Vehicle"
		q3Body* composite_body = nullptr;
		
		//Debug render
		std::unique_ptr<DebugRender> debug_render = nullptr;
		bool debug_mode = true;

		//User Interface
		std::unique_ptr<UserInterface> UI;

		//WASD and SPACE movement
		Vector3 input_vector = Vector3::Zero;
		
		//Cose belle
		std::vector<CustomBaseObject*> scene_blocks;
		std::vector<LEGOPlatform*> scene_platforms;

		//test cuboidi
		CustomBaseObject* holding_obj = nullptr;
		
		Vector3 arrows = {0,0,0};
		bool moved_y = false;
		bool moved_x = false;
		bool moved_z = false;
		bool placing = false;
		bool place = false;
		bool make = false;
		bool made = false;
		bool step_physic_library = false;

		bool make2 = false;
		bool made2 = false;
		
		int rotator = 0;
		int current_object = 0;

		bool save = false;
		bool saved = false;
		bool load = false;
		bool loaded = false;
	};
}

