#ifndef _RIVE_ARTBOARD_BASE_HPP_
#define _RIVE_ARTBOARD_BASE_HPP_
#include "container_component.hpp"
#include "core/field_types/core_double_type.hpp"
namespace rive
{
	class ArtboardBase : public ContainerComponent
	{
	protected:
		typedef ContainerComponent Super;

	public:
		static const int typeKey = 1;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(int typeKey) const override
		{
			switch (typeKey)
			{
				case ArtboardBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		int coreType() const override { return typeKey; }

		static const int widthPropertyKey = 7;
		static const int heightPropertyKey = 8;
		static const int xPropertyKey = 9;
		static const int yPropertyKey = 10;
		static const int originXPropertyKey = 11;
		static const int originYPropertyKey = 12;

	private:
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		float m_X = 0.0f;
		float m_Y = 0.0f;
		float m_OriginX = 0.0f;
		float m_OriginY = 0.0f;
	public:
		inline float width() const { return m_Width; }
		void width(float value)
		{
			if (m_Width == value)
			{
				return;
			}
			m_Width = value;
			widthChanged();
		}

		inline float height() const { return m_Height; }
		void height(float value)
		{
			if (m_Height == value)
			{
				return;
			}
			m_Height = value;
			heightChanged();
		}

		inline float x() const { return m_X; }
		void x(float value)
		{
			if (m_X == value)
			{
				return;
			}
			m_X = value;
			xChanged();
		}

		inline float y() const { return m_Y; }
		void y(float value)
		{
			if (m_Y == value)
			{
				return;
			}
			m_Y = value;
			yChanged();
		}

		inline float originX() const { return m_OriginX; }
		void originX(float value)
		{
			if (m_OriginX == value)
			{
				return;
			}
			m_OriginX = value;
			originXChanged();
		}

		inline float originY() const { return m_OriginY; }
		void originY(float value)
		{
			if (m_OriginY == value)
			{
				return;
			}
			m_OriginY = value;
			originYChanged();
		}

		bool deserialize(int propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case widthPropertyKey:
					m_Width = CoreDoubleType::deserialize(reader);
					return true;
				case heightPropertyKey:
					m_Height = CoreDoubleType::deserialize(reader);
					return true;
				case xPropertyKey:
					m_X = CoreDoubleType::deserialize(reader);
					return true;
				case yPropertyKey:
					m_Y = CoreDoubleType::deserialize(reader);
					return true;
				case originXPropertyKey:
					m_OriginX = CoreDoubleType::deserialize(reader);
					return true;
				case originYPropertyKey:
					m_OriginY = CoreDoubleType::deserialize(reader);
					return true;
			}
			return ContainerComponent::deserialize(propertyKey, reader);
		}

	protected:
		virtual void widthChanged() {}
		virtual void heightChanged() {}
		virtual void xChanged() {}
		virtual void yChanged() {}
		virtual void originXChanged() {}
		virtual void originYChanged() {}
	};
} // namespace rive

#endif