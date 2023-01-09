#pragma once
#include <queue>

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

	//Input mapping enumeration
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
	
	/**
	 * \brief Main class if the LEGO component
	 */
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
		//Input handling
		void addToInputQueue(const bool& pressed, InputIndex button);
		
		//UI selection
		void trySelectingFromUI();
		
		//Load & Saving
		void loadFromPath(const std::string& path);
		void saveToPath(const std::string& path);
		
		//Placing, collisions & removing
		void isBlockPlaceable();
		void tryPlacingBlock(const Vector3& block_pos);
		void deleteLastPlacedBlock();
		
		//Materializes 
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
		float AR; //Aspect Ratio
		
		//Scene and vehicle data
		std::vector<LEGOPlatform*> scene_platforms{};
		std::vector<CustomBaseObject*> composite_body_assembly{};
		CustomBaseObject* holding_obj = nullptr;
		TPSCamera* new_TPScam = nullptr; //TPS camera return
		float grid_movement = 5; //movement in building mode, in pixels
		bool placeable = false;
		bool driving_mode = false;

		//Input handling
		std::map<InputIndex, bool> input_pressed{};
		std::queue<InputIndex> input_queue{};

		//Directional movement (driving mode)
		Vector3 movement_vector = Vector3::Zero;
	};
}

