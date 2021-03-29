#ifndef _RIVE_BONE_BASE_HPP_
#define _RIVE_BONE_BASE_HPP_
#include "bones/skeletal_component.hpp"
#include "core/field_types/core_double_type.hpp"
namespace rive
{
	class BoneBase : public SkeletalComponent
	{
	protected:
		typedef SkeletalComponent Super;

	public:
		static const int typeKey = 40;

		/// Helper to quickly determine if a core object extends another without
		/// RTTI at runtime.
		bool isTypeOf(int typeKey) const override
		{
			switch (typeKey)
			{
				case BoneBase::typeKey:
				case SkeletalComponentBase::typeKey:
				case TransformComponentBase::typeKey:
				case ContainerComponentBase::typeKey:
				case ComponentBase::typeKey:
					return true;
				default:
					return false;
			}
		}

		int coreType() const override { return typeKey; }

		static const int lengthPropertyKey = 89;

	private:
		float m_Length = 0;
	public:
		inline float length() const { return m_Length; }
		void length(float value)
		{
			if (m_Length == value)
			{
				return;
			}
			m_Length = value;
			lengthChanged();
		}

		bool deserialize(int propertyKey, BinaryReader& reader) override
		{
			switch (propertyKey)
			{
				case lengthPropertyKey:
					m_Length = CoreDoubleType::deserialize(reader);
					return true;
			}
			return SkeletalComponent::deserialize(propertyKey, reader);
		}

	protected:
		virtual void lengthChanged() {}
	};
} // namespace rive

#endif