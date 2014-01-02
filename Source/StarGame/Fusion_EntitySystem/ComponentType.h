#ifndef __COMPONENT_TYPE_H
#define __COMPONENT_TYPE_H


namespace FusionEngine
{
	enum ComponentType
	{
		CT_TRANSFORM = 0,
		CT_RENDER,
		CT_UPDATABLE_BEHAVIOR,
		CT_COLLISION,
		CT_SKILL,
		CT_SATELLITE_CREATION,
		CT_SELECTOR_APPLIED_SKILL,
		CT_TIMED_SKILL,
        
        
        /////////////
		///  Old  ///
		/////////////



		CT_HEALTH,
		CT_RENDERABLE,
		CT_RENDERABLE_GEN_DATA,
		CT_RENDERABLE_UNLIT,
		CT_RENDERABLE_LIT,
		CT_RIGID_BODY, 
		CT_JOINT,
		CT_CELESTIAL_BODY,
		
		CT_ROTATE_ORIGIN,
		CT_CLICKABLE,
		CT_LIGHT,
		CT_SUN,
		CT_SATELLITE, 
		CT_SATELLITE_ORBIT,

		CT_LAST,
	};

	// Component type bits. Used by the entity systems to determine if an entity is compatible.
	const unsigned int CT_TRANSFORM_BIT = 1 << CT_TRANSFORM;
	const unsigned int CT_RENDER_BIT = 1 << CT_RENDER;
	const unsigned int CT_UPDATABLE_BEHAVIOR_BIT = 1 << CT_UPDATABLE_BEHAVIOR;
	const unsigned int CT_COLLISION_BIT = 1 << CT_COLLISION;
	const unsigned int CT_SKILL_BIT = 1 << CT_SKILL;
	const unsigned int CT_SATELLITE_CREATION_BIT = 1 << CT_SATELLITE_CREATION;
	const unsigned int CT_SELECTOR_APPLIED_SKILL_BIT = 1 << CT_SELECTOR_APPLIED_SKILL;
	const unsigned int CT_TIMED_SKILL_BIT = 1 << CT_TIMED_SKILL;


	/////////////
	///  Old  ///
	/////////////



	const unsigned int CT_HEALTH_BIT = 1 << CT_HEALTH;
	const unsigned int CT_RENDERABLE_BIT = 1 << CT_RENDERABLE;
	const unsigned int CT_RENDERABLE_UNLIT_BIT = 1 << CT_RENDERABLE_UNLIT;
	const unsigned int CT_RENDERABLE_LIT_BIT = 1 << CT_RENDERABLE_LIT;
	const unsigned int CT_RIGID_BODY_BIT = 1 << CT_RIGID_BODY;
	const unsigned int CT_JOINT_BIT = 1 << CT_JOINT;
	const unsigned int CT_CELESTIAL_BODY_BIT = 1 << CT_CELESTIAL_BODY;
	
	const unsigned int CT_ROTATE_ORIGIN_BIT = 1 << CT_ROTATE_ORIGIN;
	const unsigned int CT_CLICKABLE_BIT = 1 << CT_CLICKABLE;
	const unsigned int CT_LIGHT_BIT = 1 << CT_LIGHT;
	const unsigned int CT_SUN_BIT = 1 << CT_SUN;
	const unsigned int CT_SATELLITE_BIT = 1 << CT_SATELLITE;
	const unsigned int CT_SATELLITE_ORBIT_BIT = 1 << CT_SATELLITE_ORBIT;
	const unsigned int CT_RENDERABLE_GEN_DATA_BIT = 1 << CT_RENDERABLE_GEN_DATA;
	
}


#endif