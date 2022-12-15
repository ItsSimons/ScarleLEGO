#pragma once

//Scarle Headers
#include "GameData.h"
#include "GameState.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"
#include "q3.h"

//Debugghing
#include <iostream>
#include "DebugRender.h"
#include "CustomBaseObject.h"
#include "LEGOthruster.h"
#include "LEGOwingRight.h"
#include "LEGOwheel.h"
#include "LEGOcube.h"

namespace LEGO
{
	class Handler
	{
	public:
		explicit Handler(GameData* _GD, DrawData* _DD, DrawData2D* _DD2D, ID3D11Device* _d3dDevice,
						 ID3D11DeviceContext1* _d3dContext, IEffectFactory* _fxFactory);
		~Handler();

		void initialize();
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

		//WASD and SPACE movement
		Vector3 input_vector = Vector3::Zero;
		
		//Cose belle
		std::vector<CustomBaseObject*> scene_blocks;

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
	};
}

