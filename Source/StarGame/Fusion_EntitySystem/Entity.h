#ifndef __ENTITY_H
#define __ENTITY_H


namespace FusionEngine
{
	typedef unsigned int EntityID;

	const EntityID NULL_ENTITY = 0;

	class Entity
	{
		friend class EntityManager;
		friend class EntitySystem;

	private:
		EntityID id;
		unsigned int index;
		unsigned int typeBits;

		// Only the parent class should be able to instantiate a new entity.
		Entity(EntityID newId) : id(newId), index(0), typeBits(0) {}

	public:
		~Entity() {}

		EntityID GetID() const { return id; }
		unsigned int GetIndex() const { return index; }
	};
}


#endif