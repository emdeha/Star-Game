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
		CT_CELESTIAL_BODY,
		CT_FUNCTIONAL_BEHAVIOR,
		CT_ROTATE_ORIGIN,
		CT_CLICKABLE,

		CT_LAST,
	};

	// Component type bits. Used by the entity systems to determine if an entity is compatible.
	const unsigned int CT_TRANSFORM_BIT = 1 << CT_TRANSFORM;
	const unsigned int CT_HEALTH_BIT = 1 << CT_HEALTH;
	const unsigned int CT_RENDERABLE_BIT = 1 << CT_RENDERABLE;
	const unsigned int CT_RIGID_BODY_BIT = 1 << CT_RIGID_BODY;
	const unsigned int CT_JOINT_BIT = 1 << CT_JOINT;
	const unsigned int CT_CELESTIAL_BODY_BIT = 1 << CT_CELESTIAL_BODY;
	const unsigned int CT_FUNCTIONAL_BEHAVIOR_BIT = 1 << CT_FUNCTIONAL_BEHAVIOR;
	const unsigned int CT_ROTATE_ORIGIN_BIT = 1 << CT_ROTATE_ORIGIN;
	const unsigned int CT_CLICKABLE_BIT = 1 << CT_CLICKABLE;
}


#endif