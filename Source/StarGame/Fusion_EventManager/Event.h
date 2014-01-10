#ifndef __EVENT_H
#define __EVENT_H


namespace FusionEngine
{
	typedef unsigned int EventType;

	class IEventData
	{
	private:
		EventType type;

	public:
		IEventData(EventType newType) : type(newType) {}

		EventType GetType() const { return type; }
	};

	class IEventListener
	{
	public:
		virtual ~IEventListener() {}

		virtual bool HandleEvent(const IEventData &eventData) = 0;
	};
}


#endif