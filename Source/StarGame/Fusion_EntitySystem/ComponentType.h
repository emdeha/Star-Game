#ifndef __COMPONENT_TYPE_H
#define __COMPONENT_TYPE_H


namespace FusionEngine
{
	enum ComponentType
	{
		CT_TRANSFORM = 0,
		CT_HEALTH,
		CT_RENDERABLE,
		CT_RIGID_BODY, 
		CT_JOINT,

		CT_LAST,
	};

	// Component type bits. Used by the entity systems to determine if an entity is compatible.
	const unsigned int CT_TRANSFORM_BIT = 1 << CT_TRANSFORM;
	const unsigned int CT_HEALTH_BIT = 1 << CT_HEALTH;
	const unsigned int CT_RENDERABLE_BIT = 1 << CT_RENDERABLE;
	const unsigned int CT_RIGID_BODY_BIT = 1 << CT_RIGID_BODY;
	const unsigned int CT_JOINT_BIT = 1 << CT_JOINT;
}


#endif