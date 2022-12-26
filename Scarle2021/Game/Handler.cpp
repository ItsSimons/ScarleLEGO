
#include "pch.h"
#include "Handler.h"

/**
 * \brief Class to include in Scarle for the LEGO component
 * \param _AR Aspect Ratio
 * \param _GD GameData pointer
 * \param _DD DrawData pointer
 * \param _DD2D DrawDara2D pointer
 * \param _d3dDevice DX11 device pointer 
 * \param _d3dContext DD11 Context pointer
 * \param _fxFactory DX11 Effect Factory pointer
 */
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
	for(auto& block : composite_body_assembly)
	{
		delete block;
	}

	//Deletes the physic scene
	delete physic_scene;
}

/**
 * \param resolution Resolution of the game in pixels
 */
void LEGO::Handler::initialize(const Vector2& resolution)
{
	//User option to turn on debug mode
	if(debug_mode)
	{
		debug_render = std::make_unique<DebugRender>(d3dDevice, d3dContext);
	}

	//inits UI
	UI->initialize(d3dDevice, resolution);
	
	//Creates a composite platform for the player to drive on
	q3Body* platform;
	q3BodyDef bodyDef;
	bodyDef.bodyType = eStaticBody;
	platform = physic_scene->CreateBody( bodyDef );

	//Ammount and size of platforms
	//Tweaking X and Y will make the main area smaller/bigger
	const int platform_x = 8;
	const int platform_y = 8;
	const int platform_size_x = 100;
	const int platform_size_y = 100;
	//finds a midpoint to offset the platforms to the center
	const int mid_point_x = ((platform_x - 1) * platform_size_x) * 0.5f;
	const int mid_point_y = ((platform_y - 1) * platform_size_y) * 0.5f;

	//Places platforms in a grid, centers it to world origin
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
	composite_body_assembly.push_back(holding_obj);
	//Gives a cube as a starter building object
	holding_obj = new LEGOCube(d3dDevice, fxFactory, physic_scene, composite_body);
	holding_obj->setID(id_LEGOCube);
}

// Scarle --------------------------------------------------------------------------------------------------------------

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

			//All the input from the USER gets queued inside the input queue.
			//Each update all the stored input actions get executed until the queue is empty.
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
					UI->toggleVisibilityUI(); //Toggle ON/OFF
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
				//Pops the current input action
				input_queue.pop();
			}
		}
		//Ticks UI
		UI->update(GD);
		//Ticks the object being hold
		holding_obj->Tick(GD);
	}
	
	for(auto& block : composite_body_assembly)
	{
		block->Tick(GD);
		//on driving mode calls the movement behaviour of each object
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

	for(auto& block : composite_body_assembly)
	{
		block->Draw(DD);
	}

	//no need to render UI and holding obj in driving mode
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

// Input handling ------------------------------------------------------------------------------------------------------

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
		//Links a button to a input_index entry.
		//Similar to how inputs are mapped on unity/unreal.
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

/**
 * \brief Links a physical button to a input index entry
 * \param pressed if true adds the input entry to the input queue
 * \param button input index
 */
void LEGO::Handler::addToInputQueue(const bool& pressed, InputIndex button)
{
	if(pressed)
	{
		//If pressed, add it once then set bool to false to not add it again
		if(!input_pressed[button])
		{
			input_queue.push(button);
			input_pressed[button] = true;
		}
	}
	else
	{
		//When button gets released set bool to false to allow to be added again
		if(input_pressed[button])
		{
			input_pressed[button] = false;
		}
	}
}

// UI Selection --------------------------------------------------------------------------------------------------------

/**
 * \brief On call, if the mouse is hovering over a UI element it will be selected
 */
void LEGO::Handler::trySelectingFromUI()
{
	//gets data from the UI.
	//Data is based on mouse pos, more info in UserInterface class
	const BlockIndex block_id = UI->getSelectionBlockID();
	const std::string save_path = UI->tryGetSavePath();
	const std::string load_path = UI->tryGetLoadPath();

	//If a block ID is returned, apply block to holding OBJ. block can then be used to build.
	if(block_id != id_invalid)
	{
		//Saves pos and rot, then deletes old holding obj.
		const Vector3 block_pos = holding_obj->GetPos();
		const Vector3 block_rot = holding_obj->GetPitchYawRoll();
		delete holding_obj;

		//makes a new holding obj with old data but new block ID
		holding_obj = BlockHelper::MakeBlock(
			block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body);
	}

	//On valid path, try to save current vehicle to it
	if(save_path != "null" && !save_path.empty())
	{
		//saving here
		saveToPath(save_path);
	}

	//On valid path, try to load vehicle from it
	if(load_path != "null" && !load_path.empty())
	{
		//loading here
		loadFromPath(load_path);
	}
}

// Load & Saving -------------------------------------------------------------------------------------------------------

/**
 * \brief Given a path a JSON file, loads a vehicle from it.
 * \param path file path as string
 */
void LEGO::Handler::loadFromPath(const std::string& path)
{
	//opens json file 
	std::ifstream file(path);
	//Parses the file and creates a virtual c++ copy
	json jsonfile = json::parse(file);
	//closes actual file, copy will be used from now on
	file.close();
	
	if(!jsonfile.empty())
	{
		//Clears the already existing blocks that may have been placed
		while(!composite_body_assembly.empty())
		{
			delete composite_body_assembly.back();
			composite_body_assembly.pop_back();
		}

		//For each block entry in the JSON, reads the necessary data to build a block.
		for (auto value : jsonfile)
		{
			const Vector3 block_pos = Vector3(
				value["position"]["x"], value["position"]["y"], value["position"]["z"]);
			const Vector3 block_rot = Vector3(
				value["rotation"]["pitch"], value["rotation"]["yaw"], value["rotation"]["roll"]);
			const BlockIndex block_id = value["type"];

			//Assembles and materialises the block with the new data.
			composite_body_assembly.push_back(BlockHelper::MakeBlock(
				block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body));
			//buonds check and place validation is not run as there is not the need to as
			//this file is written and read only via lego component
			composite_body_assembly.back()->materialize();
		}
	}
}

/**
 * \brief Given a path to a JSON file, saves a vehicle in it 
 * \param path file path as string
 */
void LEGO::Handler::saveToPath(const std::string& path)
{
	//Creates a virtual JSON file 
	json jsonfile;

	//Saves the data needed to replicate the block in the future inside the virtual JSON, does it for each block
	//currently present in the scene.
	for (auto& block : composite_body_assembly)
	{
		const auto& block_pos = block->GetPos();
		const auto& block_rot = block->GetPitchYawRoll();

		//The virtual JSON file can be treated as an array, as before it is written to file is handled as a normal
		//C++ data container
		//Pushes back id, pos and rot for each block 
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

	//Opens the file we want to write to
	std::ofstream file(path);
	//Writes to file and closes it
	file << jsonfile;
	file.close();
}

// Placing & Removing --------------------------------------------------------------------------------------------------

/**
 * \brief On call tries to place and materialize the current holding OBJ in the physic world
 * \param block_pos position to be placed to
 */
void LEGO::Handler::tryPlacingBlock(const Vector3& block_pos)
{
	//Actually placing 
	if(holding_obj->place())
	{
		//Object has been placed correctly, get ID and save it
		const BlockIndex& block_id = holding_obj->getID();
		const Vector3& block_rot = holding_obj->GetPitchYawRoll();
		composite_body_assembly.push_back(holding_obj);

		//Creates a new identical object in hand to build with
		holding_obj = BlockHelper::MakeBlock(
			block_pos, block_rot, block_id, d3dDevice, fxFactory, physic_scene, composite_body);
	}
}

/**
 * \brief Deletes last added block to the block assembly vector
 */
void LEGO::Handler::deleteLastPlacedBlock()
{
	//Won't delete the starting cube
	if(composite_body_assembly.size() > 1)
	{
		//Deletes last placed block
		delete composite_body_assembly.back();
		composite_body_assembly.pop_back();
	}
}

// Materialization -----------------------------------------------------------------------------------------------------

/**
 * \brief Applies physics to vehicle, creates a follow camera and disables UI and building mode
 */
void LEGO::Handler::materializeCompositeBody()
{
	driving_mode = true;
	UI->setDrivingMode(driving_mode);

	//For the camera, a base offset is applied as it would be overlapping with the vehicle otherwise
	Vector3 offset_pos = Vector3(10,60,80);
	Vector3 farthest_pos = Vector3(0,0,0);

	//For each block in the composite body, sees what the farthest away from the origin, so the same position as
	//the starting block, to determine distance and angle of the follow camera
	for (const auto& block : composite_body_assembly)
	{
		const Vector3 block_pos = block->GetPos();

		if(block_pos.x > farthest_pos.x)
			farthest_pos.x = block_pos.x;
		if(block_pos.y > farthest_pos.y)
			farthest_pos.y = block_pos.y;
		if(block_pos.z > farthest_pos.z)
			farthest_pos.z = block_pos.z;
	}

	//Applies offset
	farthest_pos = (farthest_pos * 1.15f) + offset_pos;

	//Creates new follow up camera, and sets follow camera to deafult.
	new_TPScam = new TPSCamera(
		0.25f * XM_PI, AR, 1.0f, 10000.0f, composite_body_assembly.front(),
		Vector3::UnitY, Vector3(0.0f, farthest_pos.y, farthest_pos.z));
	GD->m_GS = GS_PLAY_TPS_CAM;
}

// Getters & Setters ---------------------------------------------------------------------------------------------------

/**
 * \return returns a new follow up camera when provided.
 */
TPSCamera* LEGO::Handler::getNewTPScam()
{
	if(new_TPScam != nullptr)
	{
		//if a new camera has been made, sends it back to scarle
		TPSCamera* temp_ptr = new_TPScam;
		new_TPScam = nullptr;
		return temp_ptr;
	}
	return nullptr;
}
