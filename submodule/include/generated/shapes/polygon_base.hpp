#ifndef _RIVE_POLYGON_BASE_HPP_
#define _RIVE_POLYGON_BASE_HPP_
#include "core/field_types/core_double_type.hpp"
#include "core/field_types/core_uint_type.hpp"
#include "shapes/parametric_path.hpp"
namespace rive
{
	class PolygonBase : public ParametricPath
	{
	protected:
		typedef ParametricPath Super;

	public:
		static const int typeKey = 51;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(int typeKey) const override
		{
			switch (typeKey)
			{
				case PolygonBase::typeKey:
				case ParametricPathBase::typeKey:
				case PathBase::typeKey:
				case NodeBase::typeKey:
				case TransformComponentBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		int coreType() const override { return typeKey; }

		static const int pointsPropertyKey = 125;
		static const int cornerRadiusPropertyKey = 126;

	private:
		int m_Points = 5;
		float m_CornerRadius = 0;
	public:
		inline int points() const { return m_Points; }
		void points(int value)
		{
			if (m_Points == value)
			{
				return;
			}
			m_Points = value;
			pointsChanged();
		}

		inline float cornerRadius() const { return m_CornerRadius; }
		void cornerRadius(float value)
		{
			if (m_CornerRadius == value)
			{
				return;
			}
			m_CornerRadius = value;
			cornerRadiusChanged();
		}

		bool deserialize(int propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case pointsPropertyKey:
					m_Points = CoreUintType::deserialize(reader);
					return true;
				case cornerRadiusPropertyKey:
					m_CornerRadius = CoreDoubleType::deserialize(reader);
					return true;
			}
			return ParametricPath::deserialize(propertyKey, reader);
		}

	protected:
		virtual void pointsChanged() {}
		virtual void cornerRadiusChanged() {}
	};
} // namespace rive

#endif