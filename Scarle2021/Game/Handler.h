#pragma once
#include <queue>
#include <iostream>

//Scarle Headers
#include "GameData.h"
#include "DrawData.h"
#include "DrawData2D.h"
#include "ObjectList.h"

//Lego Headers
#include "UserInterface.h"
#include "DebugRender.h"
#include "CustomBaseObject.h"
#include "BlockIndex.h"
#include "q3.h"

//Json file manager
#include <nlohmann/json.hpp>

namespace LEGO
{
	using json = nlohmann::json;
	
	enum InputIndex
	{
		input_up,
		input_down,
		input_left,
		input_right,
		input_forward,
		input_backward,
		input_show_UI,
		input_interact,
		input_rotate_yaw,
		input_rotate_pitch,
		input_delete,
		input_select,
		input_materialize
	};
	
	class Handler
	{
	public:
		explicit Handler(float _AR, GameData* _GD, DrawData* _DD, DrawData2D* _DD2D, ID3D11Device* _d3dDevice,
						 ID3D11DeviceContext1* _d3dContext, IEffectFactory* _fxFactory);
		~Handler();

		void initialize(const Vector2& resolution);

		

		//Scarle
		void update();
		void render();
		void readInput();

		//Getters & Setters
		TPSCamera* getNewTPScam();

	private:
		void loadFromPath(const std::string& path);
		void saveToPath(const std::string& path);
		
		void addToInputQueue(const bool& pressed, InputIndex button);

		void tryPlacingBlock(const Vector3& block_pos);

		void deleteLastPlacedBlock();

		void trySelectingFromUI();

		void materializeCompositeBody();
		
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
		bool debug_mode = false;

		//User Interface
		std::unique_ptr<UserInterface> UI{};

		//WASD and SPACE movement
		Vector3 movement_vector = Vector3::Zero;

		//TPS camera return
		TPSCamera* new_TPScam = nullptr;
		
		//Cose belle
		std::vector<CustomBaseObject*> scene_blocks{};
		std::vector<LEGOPlatform*> scene_platforms{};

		//test cuboidi
		CustomBaseObject* holding_obj = nullptr;


		float AR;
		
		float grid_movement = 5;
		bool driving_mode = false;


		
		std::map<InputIndex, bool> input_pressed{};
		std::queue<InputIndex> input_queue{};
	};
}

