#pragma once

//! Macros to provide runtime type info
#define DECLARE_OBJECT(ClassName, ParentClassName) \
		typedef ParentClassName super; \
		public: \
		virtual const char* GetClassName() const { return ClassName::GetStaticClassName(); } \
		static const char* GetStaticClassName() { return #ClassName; } \
		static const char* GetStaticParentClassName() { return #ParentClassName; } \
		virtual bool IsA(int typeID) const { \
			if(ClassName::TypeID == typeID) return true; \
			else { \
				if(ClassName::TypeID == Object::TypeID) return false; \
				else return super::IsA(typeID); \
			} \
		} \
		virtual int GetTypeID() const { return ClassName::TypeID; } \
		static int TypeID;

#define DEFINE_OBJECT(ClassName) int ClassName::TypeID = (m_TypeCount++);

#define DYNAMIC_CAST(object, TargetClassName) ((object && object->IsA(TargetClassName::TypeID)) ? static_cast<TargetClassName*>(object) : nullptr)

namespace platz {
	class Object {

		DECLARE_OBJECT(Object, Object);

	public:

		virtual ~Object() = default;

	protected:

		static int m_TypeCount;
	};
}
