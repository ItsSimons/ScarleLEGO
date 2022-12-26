
#include "pch.h"
#include "Handler.h"

LEGO::Handler::Handler(float _AR, GameData* _GD, DrawData* _DD, DrawData2D* _DD2D, ID3D11Device* _d3dDevice,
                       ID3D11DeviceContext1* _d3dContext, IEffectFactory* _fxFactory) : AR(_AR), GD(_GD), DD(_DD),
					   DD2D(_DD2D), d3dDevice(_d3dDevice), d3dContext(_d3dContext), fxFactory(_fxFactory)
{
	//Sets the physic library to calculate steps with a 16.6667ms interval
	//(as game is locked to 60fps, an accumulator is not needed)
	physic_scene = new q3Scene(1.f / 60.f);
	physic_scene->SetAllowSleep(true);
	physic_scene->SetEnableFriction(true);
	//Unrealistic gravity but feels better
	physic_scene->SetGravity(q3Vec3(0,-19.62, 0));

	//Inits the UI
	UI = std::make_unique<UserInterface>();
}

LEGO::Handler::~Handler() 
{
	//Deletes all the platforms
	for (auto platform : scene_platforms)
	{
		delete platform;
	}
	
	//Deletes all the block of the vehicle
	for(auto& block : scene_blocks)
	{
		delete block;
	}

	//Deletes the physic scene
	delete physic_scene;
}

void LEGO::Handler::initialize(const Vector2& resolution)
{
	//User option to turn on debug mode
	if(debug_mode)
	{
		debug_render = std::make_unique<DebugRender>(d3dDevice, d3dContext);
	}
	
	UI->initialize(d3dDevice, resolution);
	
	//Creates a composite platform for the player to drive on
	q3Body* platform;
	q3BodyDef bodyDef;
	bodyDef.bodyType = eStaticBody;
	platform = physic_scene->CreateBody( bodyDef );

	//Ammount and size of platforms
	const int platform_x = 8;
	const int platform_y = 8;
	const int platform_size_x = 100;
	const int platform_size_y = 100;
	//finds a midpoint to offset the platforms to the center
	const int mid_point_x = ((platform_x - 1) * platform_size_x) * 0.5f;
	const int mid_point_y = ((platform_y - 1) * platform_size_y) * 0.5f;
	
	for (int i = 0; i < platform_x; ++i)
	{
		for (int j = 0; j < platform_y; ++j)
		{
			//Creates a platform and adds it to the platforms composite body
			auto current_platform = new LEGOPlatform(d3dDevice, fxFactory, physic_scene, platform);
			current_platform->SetPos(Vector3(
				platform_size_x * i - mid_point_x, -50,
				platform_size_y * j - mid_point_y));
			current_platform->materialize();
			scene_platforms.push_back(current_platform);
		}
	}
	//Ticks all the newly created platforms at least once
	for (auto platform : scene_platforms)
	{
		platform->Tick(GD);
	}

	//Creates the mobile composite body
	//This will be the "vehicle"
	q3BodyDef composite_body_def;
	composite_body_def.bodyType = eDynamicBody;
	composite_body = physic_scene->CreateBody(composite_body_def);
	
	//Places the basic starting cube
	holding_obj = new LEGOStartingCube(d3dDevice, fxFactory, physic_scene, composite_body);
	holding_obj->setID(id_LEGOStartingCube);
	holding_obj->materialize();
	scene_blocks.push_back(holding_obj);
	//Gives a cube as a starter building object
	holding_obj = new LEGOCube(d3dDevice, fxFactory, physic_scene, composite_body);
	holding_obj->setID(id_LEGOCube);
}

void LEGO::Handler::update()
{
	if(driving_mode)
	{
		//If in driving mode steps the physics scene
		physic_scene->Step();
	}
	else
	{
		//Input handler
		if(!input_queue.empty())
		{
			const auto current_pos = holding_obj->GetPos();

			while(!input_queue.empty())
			{
				switch (input_queue.front())
				{
				case input_up:
					holding_obj->SetPos(current_pos - Vector3(0,grid_movement,0));
					break;

				case input_down:
					holding_obj->SetPos(current_pos + Vector3(0,grid_movement,0));
					break;

				case input_left:
					holding_obj->SetPos(current_pos - Vector3(grid_movement,0,0));
					break;

				case input_right:
					holding_obj->SetPos(current_pos + Vector3(grid_movement,0,0));
					break;

				case input_forward:
					holding_obj->SetPos(current_pos - Vector3(0,0,grid_movement));
					break;

				case input_backward:
					holding_obj->SetPos(current_pos + Vector3(0,0,grid_movement));
					break;

				case input_show_UI:
					UI->toggleVisibilityUI();
					break;

				case input_interact:
					tryPlacingBlock(current_pos);
					break;

				case input_rotate_yaw:
					holding_obj->yawObject(true);
					break;

				case input_rotate_pitch:
					holding_obj->pitchObject(true);
					break;

				case input_delete:
					deleteLastPlacedBlock();
					break;
			
				case input_select:
					trySelectingFromUI();
					break;
			
				case input_materialize:
					materializeCompositeBody();
					break;
				}
			
				input_queue.pop();
			}
		}
		//Ticks UI
		UI->update(GD);
		//Ticks the object being hold
		holding_obj->Tick(GD);
	}
	
	for(auto& block : scene_blocks)
	{
		block->Tick(GD);
		if(driving_mode)
			block->applyInputToBlock(GD, movement_vector);
	}
	
	//Updates debug renderer
	if(debug_mode)
	{
		debug_render->update(GD, physic_scene);
	}
}

void LEGO::Handler::render()
{
	for (auto platform : scene_platforms)
	{
		platform->Draw(DD);
	}

	for(auto& block : scene_blocks)
	{
		block->Draw(DD);
	}
	
	if(!driving_mode)
	{
		holding_obj->Draw(DD);
		UI->render(DD2D);
	}
	
	//Renders the debug nodes
	if(debug_mode)
	{
		debug_render->render(DD);
	}
}

void LEGO::Handler::readInput()
{
	if(driving_mode)
	{
		//WASD and SPACE movement vector
		//W & S
		if(!(GD->m_KBS.W && GD->m_KBS.S))
		{
			if(GD->m_KBS.W)
			{
				movement_vector.x = 1;
			}
			else if(GD->m_KBS.S)
			{
				movement_vector.x = -1;
			}
			else
			{
				movement_vector.x = 0;
			}
		}
		else
		{
			movement_vector.x = 0;
		}
		//A & D
		if(!(GD->m_KBS.A && GD->m_KBS.D))
		{
			if(GD->m_KBS.A)
			{
				movement_vector.y = -1;
			}
			else if(GD->m_KBS.D)
			{
				movement_vector.y = 1;
			}
			else
			{
				movement_vector.y = 0;
			}
		}
		else
		{
			movement_vector.y = 0;
		}
		//Space & CTRL
		if(!(GD->m_KBS.Space && GD->m_KBS.LeftControl))
		{
			if(GD->m_KBS.Space)
			{
				movement_vector.z = 1;
			}
			else if(GD->m_KBS.LeftControl)
			{
				movement_vector.z = -1;
			}
			else
			{
				movement_vector.z = 0;
			}
		}
		else
		{
			movement_vector.z = 0;
		}
	}
	else
	{
		//Building mode inputs
		addToInputQueue(GD->m_KBS.W, input_forward);
		addToInputQueue(GD->m_KBS.S, input_backward);
		addToInputQueue(GD->m_KBS.A, input_left);
		addToInputQueue(GD->m_KBS.D, input_right);
		addToInputQueue(GD->m_KBS.Q, input_up);
		addToInputQueue(GD->m_KBS.E, input_down);
		addToInputQueue(GD->m_KBS.Tab, input_show_UI);
		addToInputQueue(GD->m_KBS.Space, input_interact);
		addToInputQueue(GD->m_KBS.R, input_rotate_yaw);
		addToInputQueue(GD->m_KBS.T, input_rotate_pitch);
		addToInputQueue(GD->m_KBS.Back, input_delete);
		addToInputQueue(GD->m_MS.leftButton, input_select);
		addToInputQueue(GD->m_KBS.Enter, input_materialize);
	}
}

void LEGO::Handler::loadFromPath(const std::string& path)
{
	std::ifstream file(path);
	json jsonfile = json::parse(file);
	file.close();

	if(!jsonfile.empty())
	{
		while(!scene_blocks.empty())
		{
			delete scene_blocks.back();
			scene_blocks.pop_back();
		}
			
		for (auto value : jsonfile)
		{
			const Vector3 block_pos = Vector3(
				value["position"]["x"], value["position"]["y"], value["position"]["z"]);
			const Vector3 block_rot = Vector3(
				value["rotation"]["pitch"], value["rotation"]["yaw"], value["rotation"]["roll"]);
			const BlockIndex block_id = value["type"];
				
			scene_blocks.push_back(BlockHelper::MakeBlock(
				block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body));
			scene_blocks.back()->materialize();
		}
	}
}

void LEGO::Handler::saveToPath(const std::string& path)
{
	json jsonfile;

	for (auto& block : scene_blocks)
	{
		const auto& block_pos = block->GetPos();
		const auto& block_rot = block->GetPitchYawRoll();
			
		jsonfile.push_back(
			json{
				{"position", {
					{"x", block_pos.x},
					{"y", block_pos.y},
					{"z", block_pos.z}
				}
				},
				{"rotation", {
					{"pitch", block_rot.x},
					{"yaw", block_rot.y},
					{"roll", block_rot.z},
				}
				},
				{"type", block->getID()}
			}
		);
	}
	
	std::ofstream file(path);
	file << jsonfile;
	file.close();
}

void LEGO::Handler::addToInputQueue(const bool& pressed, InputIndex button)
{
	if(pressed)
	{
		if(!input_pressed[button])
		{
			input_queue.push(button);
			input_pressed[button] = true;
		}
	}
	else
	{
		if(input_pressed[button])
		{
			input_pressed[button] = false;
		}
	}
}

void LEGO::Handler::tryPlacingBlock(const Vector3& block_pos)
{
	//Actually placing 
	if(holding_obj->place())
	{
		//Object has been placed correctly, get ID and save it
		const BlockIndex& block_id = holding_obj->getID();
		const Vector3& block_rot = holding_obj->GetPitchYawRoll();
		scene_blocks.push_back(holding_obj);

		//Creates a new identical object in hand
		holding_obj = BlockHelper::MakeBlock(
			block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body);
	}
}

void LEGO::Handler::deleteLastPlacedBlock()
{
	if(scene_blocks.size() > 1)
	{
		//Deletes last placed block
		delete scene_blocks.back();
		scene_blocks.pop_back();
	}
}

void LEGO::Handler::trySelectingFromUI()
{
	const BlockIndex block_id = UI->getSelectionBlockID();
	const std::string save_path = UI->tryGetSavePath();
	const std::string load_path = UI->tryGetLoadPath();

	if(block_id != id_invalid)
	{
		const Vector3 block_pos = holding_obj->GetPos();
		const Vector3 block_rot = holding_obj->GetPitchYawRoll();
		delete holding_obj;
			
		holding_obj = BlockHelper::MakeBlock(
			block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body);
	}

	if(save_path != "null" && !save_path.empty())
	{
		//saving here
		saveToPath(save_path);
	}

	if(load_path != "null" && !load_path.empty())
	{
		//loading here
		loadFromPath(load_path);
	}
}

void LEGO::Handler::materializeCompositeBody()
{
	driving_mode = true;
	UI->setDrivingMode(driving_mode);

	Vector3 offset_pos = Vector3(10,60,80);
	Vector3 farthest_pos = Vector3(0,0,0);
	
	for (const auto& block : scene_blocks)
	{
		Vector3 block_pos = block->GetPos();

		block_pos = Vector3(abs(block_pos.x), abs(block_pos.y), abs(block_pos.z));

		if(block_pos.x > farthest_pos.x)
			farthest_pos.x = block_pos.x;
		if(block_pos.y > farthest_pos.y)
			farthest_pos.y = block_pos.y;
		if(block_pos.z > farthest_pos.z)
			farthest_pos.z = block_pos.z;
	}

	farthest_pos = (farthest_pos * 1.15f) + offset_pos;
	
	new_TPScam = new TPSCamera(
		0.25f * XM_PI, AR, 1.0f, 10000.0f, scene_blocks.front(),
		Vector3::UnitY, Vector3(0.0f, farthest_pos.y, farthest_pos.z));
	GD->m_GS = GS_PLAY_TPS_CAM;
}

TPSCamera* LEGO::Handler::getNewTPScam()
{
	if(new_TPScam != nullptr)
	{
		TPSCamera* temp_ptr = new_TPScam;
		new_TPScam = nullptr;
		return temp_ptr;
	}
	return nullptr;
}
