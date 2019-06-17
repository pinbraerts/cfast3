#ifndef CFAST_OBJECT_HPP
#define CFAST_OBJECT_HPP

#include <algorithm>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_set>

#include "Pointer.hpp"

using byte = unsigned char;

template<class T, class U> struct _has_mixin { static constexpr bool value = false; };
template<class T, class U> constexpr bool has_mixin = _has_mixin<T, U>::value;

struct Type;
struct Context;
struct Object;
struct Member;

using PMember = counted<Member>;

using PType = counted<Type>;

struct Object: countable {
	PType type;

	Object(PType _type): type(_type) {}

	void* GetMember(PMember member);
	void* GetMember(std::string_view name);
	byte* body() {
		return (byte*)(this + 1);
	}

	static void* operator new(size_t size) noexcept {
		return ::operator new(size);
	}
	static void* operator new(size_t size, PType type) noexcept;
	static void operator delete(void* ptr, PType) noexcept {
		::operator delete(ptr);
	}
	static void operator delete(void* ptr) noexcept {
		::operator delete(ptr);
	}
};

struct Name {
	owner<char[]> str;
	size_t str_len;

	operator std::string_view() const {
		return std::string_view(str, str_len);
	}
	Name(std::string_view sv) {
		operator=(sv);
	}

	Name& operator=(std::string_view sv) {
		str.reset();
		str_len = sv.size();
		if (!sv.empty()) {
			str = new char[str_len];
			std::copy(sv.begin(), sv.end(), (char*)str);
			//str[str_len] = '\0';
		}
		return *this;
	}
};

struct Member: Object {
	Name name;
	size_t size_of;

	explicit Member(
		std::string_view _name,
		size_t _size_of = 0
	) : Object(nullptr), // TODO add Method class
		name(_name),
		size_of(_size_of) {}

	Member(
		std::string_view _name,
		PType _type
	);
};

struct BoundMember: PMember {
	size_t offset_of;

	BoundMember(
		PMember member,
		size_t _offset_of
	) : PMember(member),
		offset_of(_offset_of) {}
};

using Members = std::vector<BoundMember>;

size_t CountOffset(const Members& members, counted<Member> stop = nullptr) {
	for (const BoundMember& member: members) {
		if (member == stop) {
			return member.offset_of;
		}
	}
	return ~static_cast<size_t>(0);
}

size_t CountOffset(const Members& members, std::string_view name) {
	for (const BoundMember& member : members) {
		if (member->name == name) {
			return member.offset_of;
		}
	}
	return ~static_cast<size_t>(0);
}

size_t CountSize(const Members& members) {
	size_t sum = 0;
	for (const BoundMember& member : members) {
		sum += member->size_of;
	}
	return sum;
}

struct Type: Member {
	Members members;

	Type(
		std::string_view _name,
		Members _members = Members()
	) : Member(_name, CountSize(members)),
		members(std::move(_members)) {}

	inline size_t GetOffset(PMember m) const {
		return CountOffset(members, m);
	}
	inline size_t GetOffset(std::string_view name) const {
		return CountOffset(members, name);
	}
	inline bool HasMember(PMember m) const {
		return GetOffset(m) < size_of;
	}
	inline bool HasMember(std::string_view name) const {
		return GetOffset(name) < size_of;
	}

	PMember AddMember(PMember m) {
		return AddMember(m, size_of);
	}
	PMember AddMember(PMember m, size_t offset) {
		m = members.emplace_back(m, offset);
		size_of += m->size_of;
		return m;
	}
};

Member::Member(
	std::string_view _name,
	PType _type
) : Object(nullptr), // TODO add Method class
	name(_name),
	size_of(_type->size_of) {}


void* Object::GetMember(PMember member) {
	size_t offset = type->GetOffset(member);
	return body() + offset;
}
inline void* Object::GetMember(std::string_view name) {
	size_t offset = type->GetOffset(name);
	return body() + offset;
}
void* Object::operator new(size_t size, PType type) noexcept {
	return ::operator new(size + type->size_of);
}

struct Context {
	//virtual Object* Alloc(size_t size) = 0;
	//virtual void Free(Object* head) = 0;
};

struct CppContext: Context {
	using traits = types_list<
		int,
		double,
		float
	>;
	static constexpr const char* names[] {
		"Int",
		"Double",
		"Float"
	};

	struct TypesEqual {
		bool operator()(const PType& a, const PType& b) const {
			return std::string_view(a->name) == b->name;
		}
		bool operator()(std::string_view a, const PType& type) const {
			return a == type->name;
		}
		bool operator()(const PType& type, std::string_view a) const {
			return a == type->name;
		}
	};
	struct TypesHasher {
		using transparent_key_equal = TypesEqual; // C++20

		size_t operator()(const PType& type) const noexcept {
			return std::hash<std::string_view>()(std::string_view(type->name));
		}
	};
	std::unordered_set<PType, TypesHasher, TypesEqual> types;
	//std::unordered_map<void*, Member> functions;

	template<class U>
	inline PType _CppTypeImpl(const char* type_name) {
		std::string_view sv(type_name);
		if constexpr (traits::index<U> < traits::size) {
			sv = names[traits::index<U>];
		}
		PType type = PType::make(sv);
		auto iter = types.find(type);
		if (iter == types.end())
			return *types.insert(type).first;
		return *iter;
	}
#define CppType(X) _CppTypeImpl<X>(#X)

	template<class T, class U>
	inline PMember _CppMemberImpl(T(U::*offset_of), const char* name, const char* type_name) {
		PType type = _CppTypeImpl<U>(type_name);
		for (PMember m: type->members) {
			if (m->name == std::string_view(name))
				return m;
		}
		size_t offset = (size_t)(&(((U*)0)->*offset_of));
		return type->AddMember(PMember::make(name, sizeof(T)), offset);
	}
#define CppMember(U, X) _CppMemberImpl(&U::X, #X, #U)
};


#endif // !CFAST_OBJECT_HPP
