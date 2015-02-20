#ifndef TRACKINGMANAGER_H_
#define TRACKINGMANAGER_H_

#include <boost/bind.hpp>
#include <boost/signals2/signal.hpp>

#include <havroc/common/CommandBuilder.h>
#include <havroc/tracking/TrackingDefinitions.h>

namespace havroc
{

	class TrackingManager
	{
	public:
		~TrackingManager(){}

		static TrackingManager* get();

		bool is_active() { return m_active; }

		template<class T>
		void register_shoulder_callback(void(T::*shoulder_callback)(float s_yaw, float s_pitch, float s_roll, uint8_t side), T* obj)
		{
			m_shoulder_event.connect(boost::bind(shoulder_callback, obj, _1, _2, _3, _4));
		}

		void register_shoulder_callback(void(*shoulder_callback)(float s_yaw, float s_pitch, float s_roll, uint8_t side))
		{
			m_shoulder_event.connect(shoulder_callback);
		}

		template<class T>
		void register_elbow_callback(void(T::*elbow_callback)(float e_yaw, float e_pitch, float e_roll, uint8_t side), T* obj)
		{
			m_elbow_event.connect(boost::bind(elbow_callback, obj, _1, _2, _3, _4));
		}

		void register_elbow_callback(void(*elbow_callback)(float e_yaw, float e_pitch, float e_roll, uint8_t side))
		{
			m_elbow_event.connect(elbow_callback);
		}

		template<class T>
		void register_wrist_callback(void(T::*wrist_callback)(float w_yaw, float w_pitch, float w_roll, uint8_t side), T* obj)
		{
			m_wrist_event.connect(boost::bind(wrist_callback, obj, _1, _2, _3, _4));
		}

		void register_wrist_callback(void(*wrist_callback)(float w_yaw, float w_pitch, float w_roll, uint8_t side))
		{
			m_wrist_event.connect(wrist_callback);
		}

		template<class T>
		void register_start_callback(void(T::*start_callback)(), T* obj)
		{
			m_start_event.connect(boost::bind(start_callback, obj));
		}

		void register_start_callback(void(*start_callback)())
		{
			m_start_event.connect(start_callback);
		}

		template<class T>
		void register_end_callback(void(T::*start_callback)(), T* obj)
		{
			m_end_event.connect(boost::bind(start_callback, obj));
		}

		void register_end_callback(void(*end_callback)())
		{
			m_end_event.connect(end_callback);
		}

		template<class T>
		void unregister_shoulder_callback(void(T::*shoulder_callback)(float s_yaw, float s_pitch, float s_roll, uint8_t side), T* obj)
		{
			m_shoulder_event.disconnect(boost::bind(shoulder_callback, obj, _1, _2, _3, _4));
		}

		void unregister_shoulder_callback(void(*shoulder_callback)(float s_yaw, float s_pitch, float s_roll, uint8_t side))
		{
			m_shoulder_event.disconnect(shoulder_callback);
		}

		template<class T>
		void unregister_elbow_callback(void(T::*elbow_callback)(float e_yaw, float e_pitch, float e_roll, uint8_t side), T* obj)
		{
			m_elbow_event.disconnect(boost::bind(elbow_callback, obj, _1, _2, _3, _4));
		}

		void unregister_elbow_callback(void(*elbow_callback)(float e_yaw, float e_pitch, float e_roll, uint8_t side))
		{
			m_elbow_event.disconnect(elbow_callback);
		}

		template<class T>
		void unregister_wrist_callback(void(T::*wrist_callback)(float w_yaw, float w_pitch, float w_roll, uint8_t side), T* obj)
		{
			m_wrist_event.disconnect(boost::bind(wrist_callback, obj, _1, _2, _3, _4));
		}

		void unregister_wrist_callback(void(*wrist_callback)(float w_yaw, float w_pitch, float w_roll, uint8_t side))
		{
			m_wrist_event.disconnect(wrist_callback);
		}

		template<class T>
		void unregister_start_callback(void(T::*start_callback)(), T* obj)
		{
			m_start_event.disconnect(boost::bind(start_callback, obj));
		}

		void unregister_start_callback(void(*start_callback)())
		{
			m_start_event.disconnect(start_callback);
		}

		template<class T>
		void unregister_end_callback(void(T::*start_callback)(), T* obj)
		{
			m_end_event.disconnect(boost::bind(start_callback, obj));
		}

		void unregister_end_callback(void(*end_callback)())
		{
			m_end_event.disconnect(end_callback);
		}


	private:
		TrackingManager();

		void tracking_command_handler(havroc::command_pkg* pkg);

		static TrackingManager* m_instance;

		boost::signals2::signal<void(float, float, float, uint8_t)> m_shoulder_event;
		boost::signals2::signal<void(float, float, float, uint8_t)> m_elbow_event;
		boost::signals2::signal<void(float, float, float, uint8_t)> m_wrist_event;

		boost::signals2::signal<void()> m_start_event;
		boost::signals2::signal<void()> m_end_event;

		bool m_active;
	};

} /* namespace havroc */

#endif /* TRACKINGMANAGER_H_ */

