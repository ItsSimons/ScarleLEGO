
#include "pch.h"
#include "Handler.h"

LEGO::Handler::Handler(GameData* _GD, DrawData* _DD, DrawData2D* _DD2D, ID3D11Device* _d3dDevice,
                       ID3D11DeviceContext1* _d3dContext, IEffectFactory* _fxFactory) : GD(_GD), DD(_DD), DD2D(_DD2D),
                       d3dDevice(_d3dDevice), d3dContext(_d3dContext), fxFactory(_fxFactory)
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

	//Temporary
	holding_obj = new LEGOStartingCube(d3dDevice, fxFactory, physic_scene, composite_body);
	//Not textured?
	holding_obj->materialize();
	scene_blocks.push_back(holding_obj);
	holding_obj = new LEGOCube(d3dDevice, fxFactory, physic_scene, composite_body);

	//Test json??
	json jsonfile;

	jsonfile["foo"] = "bar";
	

	std::ofstream o("pretty.json");
	o << std::setw(4) << jsonfile << std::endl;
}

void LEGO::Handler::update()
{
	//Updates the physic scene
	//Pausing available by stopping step
	if(step_physic_library){physic_scene->Step();}

	UI->update(GD);
	
	auto current_pos = holding_obj->GetPos();

	//Just movement done bad
	if(arrows.y == 1 && !moved_y)
	{
		holding_obj->SetPos(Vector3(current_pos.x, current_pos.y, current_pos.z - 5));
		moved_y = true;
	}
	else if(arrows.y == -1 && !moved_y)
	{
		holding_obj->SetPos(Vector3(current_pos.x, current_pos.y, current_pos.z + 5));
		moved_y = true;
	}
	if(arrows.x == 1 && !moved_x)
	{
		holding_obj->SetPos(Vector3(current_pos.x - 5, current_pos.y, current_pos.z));
		moved_x = true;
	}
	else if(arrows.x == -1 && !moved_x)
	{
		holding_obj->SetPos(Vector3(current_pos.x + 5, current_pos.y, current_pos.z));
		moved_x = true;
	}
	if(arrows.z == 1 && !moved_z)
	{
		holding_obj->SetPos(Vector3(current_pos.x, current_pos.y + 5, current_pos.z));
		moved_z = true;
	}
	else if(arrows.z == -1 && !moved_z)
	{
		holding_obj->SetPos(Vector3(current_pos.x, current_pos.y - 5, current_pos.z));
		moved_z = true;
	}

	//Placing a block
	if(placing && !place)
	{
		place = true;
		placing = false;

		//Actually placing 
		if(holding_obj->place())
		{
			//Saves block to render
			scene_blocks.push_back(holding_obj);

			//next object that will be used
			switch (current_object)
			{
			case 0:
				holding_obj = new LEGOCube(d3dDevice, fxFactory, physic_scene, composite_body);
				holding_obj->SetPos(current_pos);
				break;
			case 1:
				holding_obj = new LEGOThruster(d3dDevice, fxFactory, physic_scene, composite_body);
				holding_obj->SetPos(current_pos);
				break;
			case 2:
				holding_obj = new LEGOWing(d3dDevice, fxFactory, physic_scene, composite_body);
				holding_obj->SetPos(current_pos);
				break;
			case 3:
				holding_obj = new LEGOWheel(d3dDevice, fxFactory, physic_scene, composite_body);
				holding_obj->SetPos(current_pos);
				break;
			}
		}
	}

	//This is rotation, why the fuck did i call it make???
	if(made && !make)
	{
		make = true;
		made = false;

		switch (rotator)
		{
		case 0:
			holding_obj->yawObject(true);
			break;
		case 1:
			holding_obj->pitchObject(true);
			break;
		}
	}

	//Switches the block being hold
	if(made2 && !make2)
	{
		make2 = true;
		made2 = false;

		const auto& current_pos = holding_obj->GetPos();
		switch (current_object)
		{
		case 0:
			delete holding_obj;
			holding_obj = new LEGOCube(d3dDevice, fxFactory, physic_scene, composite_body);
			holding_obj->SetPos(current_pos);
			break;
		case 1:
			delete holding_obj;
			holding_obj = new LEGOWheel(d3dDevice, fxFactory, physic_scene, composite_body);
			holding_obj->SetPos(current_pos);
			break;
		case 2:
			delete holding_obj;
			holding_obj = new LEGOSteeringWheel(d3dDevice, fxFactory, physic_scene, composite_body);
			holding_obj->SetPos(current_pos);
			break;
		case 3:
			delete holding_obj;
			holding_obj = new LEGOThruster(d3dDevice, fxFactory, physic_scene, composite_body);
			holding_obj->SetPos(current_pos);
			break;
		}
	}
 	
	//Ticks the object being hold
	holding_obj->Tick(GD);
	
	for(auto& block : scene_blocks)
	{
		block->Tick(GD);
		//This should be what makes the magic happen
	}
	if(step_physic_library)
		for(auto& block : scene_blocks)
		{
			block->applyInputToBlock(GD, input_vector);
		}
	
	//Updates debug renderer
	if(debug_mode)
	{
		debug_render->update(GD, physic_scene);
	}
}

void LEGO::Handler::render()
{
	UI->render(DD2D);
	
	for (auto platform : scene_platforms)
	{
		platform->Draw(DD);
	}
	
	for(auto& block : scene_blocks)
	{
		block->Draw(DD);
	}

	holding_obj->Draw(DD);	
	
	//Renders the debug nodes
	if(debug_mode)
	{
		debug_render->render(DD);
	}
}

void LEGO::Handler::readInput()
{
	//WASD and SPACE movement vector
	//W & S
	if(!(GD->m_KBS.W && GD->m_KBS.S))
	{
		if(GD->m_KBS.W)
		{
			input_vector.x = 1;
		}
		else if(GD->m_KBS.S)
		{
			input_vector.x = -1;
		}
		else
		{
			input_vector.x = 0;
		}
	}
	else
	{
		input_vector.x = 0;
	}
	//A & D
	if(!(GD->m_KBS.A && GD->m_KBS.D))
	{
		if(GD->m_KBS.A)
		{
			input_vector.y = -1;
		}
		else if(GD->m_KBS.D)
		{
			input_vector.y = 1;
		}
		else
		{
			input_vector.y = 0;
		}
	}
	else
	{
		input_vector.y = 0;
	}
	//Space & CTRL
	if(!(GD->m_KBS.Space && GD->m_KBS.LeftControl))
	{
		if(GD->m_KBS.Space)
		{
			input_vector.z = 1;
		}
		else if(GD->m_KBS.LeftControl)
		{
			input_vector.z = -1;
		}
		else
		{
			input_vector.z = 0;
		}
	}
	else
	{
		input_vector.z = 0;
	}

	//gets selection?
	if(GD->m_MS.leftButton)
	{
		const Vector3 spawn_pos = holding_obj->GetPos();
		CustomBaseObject* block_hold = UI->getSelection(spawn_pos, d3dDevice, fxFactory, physic_scene, composite_body);
		if(block_hold != nullptr)
		{
			delete holding_obj;
			holding_obj = block_hold;
		}
	}

	
	
	//THE MOST UTTERLY RETARDED WAY OF INPUT EVER
	//Afterall, this is for testing
	if(GD->m_KBS.Z)
	{
		composite_body->SetAngularVelocity(q3Vec3(1,1,1));
	}

	if(GD->m_KBS.X)
	{
		
		step_physic_library = true;
	}


	if(GD->m_KBS.V)
	{
		if(scene_blocks.size() > 1)
		{
			//Deletes a specific one
			delete scene_blocks[scene_blocks.size()-1];
			scene_blocks.pop_back();
		}
	}
	
	///VERY VERY TEMPORARY 
	//Very temporary direction
	if(GD->m_KBS.Up)
	{
		arrows.y = 1;
	}
	else if(GD->m_KBS.Down)
	{
		arrows.y = -1;
	}
	else
	{
		moved_y = false;
		arrows.y = 0;
	}

	if(GD->m_KBS.Left)
	{
		arrows.x = 1;
	}
	else if(GD->m_KBS.Right)
	{
		arrows.x = -1;
	}
	else
	{
		moved_x = false;
		arrows.x = 0;
	}

	if(GD->m_KBS.Enter)
	{
        
		placing = true;
	}
	else
	{
		place = false;
		placing = false;
	}

	if(GD->m_KBS.NumPad7)
	{
		arrows.z = 1;
	}
	else if(GD->m_KBS.NumPad1)
	{
		arrows.z = -1;
	}
	else
	{
		moved_z = false;
		arrows.z = 0;
	}

	if(GD->m_KBS.T)
	{
		made = true;
	}
	else
	{
		make = false;
		made = false;
	}

	//diocane
	if(GD->m_KBS.NumPad2)
	{
		rotator = 0;
	}
	if(GD->m_KBS.NumPad5)
	{
		rotator = 1;
	}
	if(GD->m_KBS.NumPad8)
	{
		rotator = 2;
	}

	//diocane
	if(GD->m_KBS.NumPad9)
	{
		current_object = 0;
	}
	if(GD->m_KBS.NumPad6)
	{
		current_object = 1;
	}
	if(GD->m_KBS.NumPad3)
	{
		current_object = 2;
	}
	if(GD->m_KBS.Q)
	{
		current_object = 3;
	}

	if(GD->m_KBS.M)
	{
		made2 = true;
	}
	else
	{
		make2 = false;
		made2 = false;
	}
}
