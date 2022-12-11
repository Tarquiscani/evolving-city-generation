#ifndef GM_DATA_ARRAY_HH
#define GM_DATA_ARRAY_HH


#include <cstdint>
#include <fstream>
#include <limits>
#include <memory>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <type_traits>
#include <vector>

#include "io/std/vector_io.hh"
#include "settings/debug/debug_settings.hh"

#include "debug/logger/logger.h"


namespace tgm
{



using DataArrayId = uint64_t;

template <typename T, bool Resizable>
class DataArray;


// Forward declarations to allow begin(), end() ... to be declared.
template <typename T, bool Resizable, bool IsConst>
class DataArrayIterImpl;

template <typename T, bool Resizable>
using DataArrayIterator = DataArrayIterImpl<T, Resizable, false>;

template <typename T, bool Resizable>
using DataArrayConstIterator = DataArrayIterImpl<T, Resizable, true>;


namespace detail
{
	template<typename T>
	class DataArrayEl
	{
		public:
			DataArrayEl() : m_id{ 0 }, value{} { }

			template <typename ...Ts>
			DataArrayEl(DataArrayId const id, Ts&&... args) : m_id(id), value(std::forward<Ts>(args)...) { }

			////
			//	64 bits: AVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
			//		A (leftmost bit): Indicate whether this is a valid or an invalid/inactive/freed element.
			//		V (central 32 bits): Version of this element. Each time this slot is freed and recycled the version increase by 1. 
			//							 The starting value is 1, such that no element can have an id = 0.
			//		S (rightmost 31 bits): Slot id. It's the index of this element in the vector.
			//		
			//		The id is badly represented by hexadecimal values, since some different information are superimposed: XVVV VVVV YSSS SSSS.
			//			X contains informations for both A flag and V flag.
			//			Y contains informations for both V flag and S flag.
			////
			auto id() const noexcept -> DataArrayId { return m_id; }


			template <typename std::size_t N>
			auto get() -> std::conditional_t<N==0, DataArrayId const&, T &>
			{
				static_assert(N == 0 || N == 1, "No member matches that N");

				if      constexpr (N == 0) { return (m_id); }
				else if constexpr (N == 1) { return (value); }
			}
			

			template <typename std::size_t N>
			auto get() const -> std::conditional_t<N==0, DataArrayId const&, T const&>
			{
				static_assert(N == 0 || N == 1, "No member matches that N");

				if      constexpr (N == 0) { return (m_id); }
				else if constexpr (N == 1) { return (value); }
			}

			auto debug_get_id() { return m_id; }

		private:
			DataArrayId m_id; // leftmost bit :Active/Inactive; centrale 32bits: Version; rightmost 31bits: Slot
			
		public:
			T value;

		friend class DataArray<T, true>;
		friend class DataArray<T, false>;
		
		template<typename T_>
		friend auto operator<<(std::ofstream & ofs, DataArrayEl<T_> const& da_el) -> std::ofstream &;

		template<typename T_>
		friend auto operator>>(std::ifstream & ifs, DataArrayEl<T_> & da_el) -> std::ifstream &;

		template<typename T_>
		friend auto operator<<(Logger & lgr, DataArrayEl<T_> const& da_el) -> Logger &;
	};
}



} //namespace tgm


//Providing structured binding to DataArrayEl
namespace std 
{
    template<typename T>
    struct tuple_size<tgm::detail::DataArrayEl<T>> : std::integral_constant<std::size_t, 2> 
	{};
	
    template<typename T> struct tuple_element<0, tgm::detail::DataArrayEl<T>> { using type = tgm::DataArrayId const; };

    template<typename T> struct tuple_element<1, tgm::detail::DataArrayEl<T> > { using type = T; };
    template<typename T> struct tuple_element<1, tgm::detail::DataArrayEl<T> const> { using type = T const; };

	//template<typename T, std::size_t N>
 //   struct tuple_element<N, tgm::detail::DataArrayEl<T>> 
	//{
 //       using type = decltype(std::declval<tgm::detail::DataArrayEl<T>>().get<N>()); // Make the compiler compute the right type from DataArrayEl::get()
 //   };
} //namespace std



namespace tgm
{



////
//	@T: must be copy-constructible and copy-assignable.
//	@Resizable: Indicate whether the DataArray is allowed to automatically increase the allocated memory when the initial
//				size is reached.
////
template<typename T, bool Resizable = false>
class DataArray
{
	public:
		using size_type = uint32_t;
		using DataArrayEl = detail::DataArrayEl<T>;
		
		////
		//	@max_size: The maximum number of elements that this DataArray can hold. If the DataArray is Resizable,
		//			   then it represents only the initial allocated memory, but the maximum size is automatically 
		//			   increased if needed.
		////
		DataArray(size_type const max_size)
		{
			static_assert(std::is_copy_constructible_v<T>, "DataArray requires T to be copy-constructible.");
			static_assert(std::is_copy_assignable_v<T>, "DataArray requires T to be copy-assignable.");

			set_maxSize(max_size);
		}

		template <typename ...Ts>
		auto create(Ts&&... args) -> DataArrayEl &
		{
			DataArrayEl* el;

			if (m_free.empty())
			{
				if constexpr (Resizable)
				{
					if (m_vec.size() == m_max_size)
					{
						try { set_maxSize(m_max_size * 2); }
						catch (std::exception const& e) { 
							std::ostringstream oss; 
							oss << "This DataArray cannot be resized further. " << e.what(); 
							throw std::runtime_error(oss.str());
						}
					}
				}
				else
				{
					if (m_vec.size() >= m_max_size)
						throw std::runtime_error("The DataArray was full.");
				}

				++m_max_used;
				DataArrayId new_id = m_max_used - 1; //conversion from size_type to DataArrayId
				new_id |= UINT64_C(0x0000000080000000); //version is set to 1
				m_vec.emplace_back(new_id, std::forward<Ts>(args)...);

				el = &m_vec.back();
			}
			else
			{
				auto free_slot = m_free.back();
				m_free.pop_back();

				el = &m_vec[free_slot];

				// nullify leftmost bit (that indicate a free state)  ---  id version was already changed in DataArray::destroy
				el->m_id = el->m_id & UINT64_C(0x7FFFFFFFFFFFFFFF);
				el->value = T{ std::forward<Ts>(args)... };
			}

			++m_count;
			return *el;
		}
		
		////
		//	Note: The user has the responsibility to use only valid ids. Otherwise there is an undefined behavior.
		////
		auto get(DataArrayId const id) -> T &
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			return m_vec[slot(id)].value;
		}

		////
		//	Note: The user has the responsibility to use only valid ids. Otherwise there is an undefined behavior.
		////
		auto get(DataArrayId const id) const -> T const&
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			return m_vec[slot(id)].value;
		}
		
		////
		//	Note: The user has the responsibility to use only valid ids. Otherwise there is an undefined behavior. Expired ids are valid.
		////
		auto weak_get(DataArrayId const id) -> T*
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			auto& el = m_vec[slot(id)];

			if (el.m_id == id)  // Compare free status, version and slot. To be precise one should also verify that !is_free(el.id), 
			{				  // but if the version match that would be a rare case.
				return &el.value;
			}
			else
			{
				return nullptr;
			}
		}
		
		////
		//	Note: The user has the responsibility to use only valid ids. Otherwise there is an undefined behavior. Expired ids are valid.
		////
		auto weak_get(DataArrayId const id) const -> T const*
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			auto& el = m_vec[slot(id)];

			if (el.m_id == id)  // Compare free status, version and slot. To be precise one should also verify that !is_free(el.id), 
			{				    // but if the version match that would be a rare case.
				return &el.value;
			}
			else
			{
				return nullptr;
			}
		}
		
		auto get_or_throw(DataArrayId const id) -> T &
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			auto & el = m_vec[slot(id)];

			if (el.m_id == id)  // Compare free status, version and slot. To be precise one should also verify that !is_free(el.id), 
			{				    // but if the version match that would be a rare case.
				return el.value;
			}
			else
			{
				throw std::runtime_error("Expired id");
			}
		}
		
		auto get_or_throw(DataArrayId const id) const -> T const&
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			auto & el = m_vec[slot(id)];

			if (el.m_id == id)  // Compare free status, version and slot. To be precise one should also verify that !is_free(el.id), 
			{				    // but if the version match that would be a rare case.
				return el.value;
			}
			else
			{
				throw std::runtime_error("Expired id");
			}
		}
		
		////
		//	@return: True if the id version matched the current element and the element has been actually destroyed. False if the element hasn't been destroyed.
		//	Note: The user has the responsibility to use only valid ids. Otherwise there is an undefined behavior. Expired ids are valid.
		////
		#pragma warning(disable: 4702)
		bool destroy(DataArrayId const id)
		{
			#if DYNAMIC_ASSERTS
				assert_idValidity(id);
			#endif

			//std::cout << std::hex << "destroy: argument id: 0x" << std::setw(16) << std::setfill('0') << id << std::endl;
			//std::cout << std::hex << "destroy: slot(id):    0x" << std::setw(16) << std::setfill('0') << slot(id) << std::endl;
			auto& el = m_vec[slot(id)];
			//std::cout << std::hex << "destroy: el.m_id        0x" << std::setw(16) << std::setfill('0') << el.id << std::endl;

			if (el.m_id != id) // Compare free status, version and slot
			{
				#if DYNAMIC_ASSERTS
					throw std::runtime_error("Trying to destroy an already destroyed element (or, rarely, the id is completely invalid and wrong).");
				#endif
				
				return false;
			}
			#pragma warning(default: 4702)

			el.m_id = setFree_and_increaseVersion(el.m_id);

			m_free.push_back(slot(id));

			--m_count;

			return true;
		}

		auto begin() noexcept -> DataArrayIterator<T, Resizable>;
		auto end() noexcept -> DataArrayIterator<T, Resizable>;
		auto begin() const noexcept -> DataArrayConstIterator<T, Resizable>;
		auto end() const noexcept -> DataArrayConstIterator<T, Resizable>;
		auto cbegin() const noexcept -> DataArrayConstIterator<T, Resizable>;
		auto cend() const noexcept -> DataArrayConstIterator<T, Resizable>;
		
		void assert_idValidity(DataArrayId const id) const
		{
			if (slot(id) >= m_max_used)
				throw std::runtime_error("The id is out-of-range.");
			
			uint32_t vsn = version(id);
			if (vsn < 1)
				throw std::runtime_error("The version is out-of-range.");
		}

		auto count() const noexcept -> size_type { return m_count; }
		bool empty() const noexcept { return m_count == 0; }
		auto max_size() const noexcept -> size_type { return m_max_size; }

		auto debug_internalVec() -> std::vector<DataArrayEl>& { return m_vec; }
		auto debug_vecSize() { return m_vec.size(); }
		void debug_setVersion(DataArrayId & id, size_type new_version)
		{
			auto& el = m_vec[slot(id)];

			DataArrayId vn = new_version;

			el.m_id = (id & UINT64_C(0x8000000000000000)) | (vn << 31) | (id & UINT64_C(0x000000007FFFFFFF));

			id = el.m_id;
		}
		static auto debug_isFree(DataArrayId id) -> bool { return is_free(id); }
		static auto debug_version(DataArrayId id) -> size_type { return version(id); }
		static auto debug_slot(DataArrayId id) -> size_type { return slot(id); }


	private:
		std::vector<DataArrayEl> m_vec;
		std::vector<size_type> m_free;

		size_type m_max_size = 0;	//max possible number of elements 
		size_type m_max_used = 0;	//max number of memory blocks ever used
		size_type m_count = 0;		//active elements



		void set_maxSize(size_type const max_size)
		{
			if (max_size > 0x7FFFFFFF || max_size > m_vec.max_size() ) // the second condition is a very rare case on modern 64-bit machines
				throw std::runtime_error("Such a big number of elements is not allowed on this platform.");
			
			m_max_size = max_size;

			try
			{
				m_vec.reserve(max_size);
				m_free.reserve(max_size); //TODO: PERFORMANCE: m_free is hardly as big as m_vec, so a smaller initial value can be assigned
			}
			catch (std::exception const& e)
			{
				std::ostringstream oss;	oss << "Not enough space in RAM for this vertices_count (~" << (max_size * sizeof(T) / (1024*1024)) << " MB required). " << e.what();
				throw std::runtime_error(oss.str());
			}
		}



		static auto is_free(DataArrayId id) -> bool      { return                         id & UINT64_C(0x8000000000000000);         }
		static auto version(DataArrayId id) -> size_type { return static_cast<size_type>((id & UINT64_C(0x7FFFFFFF80000000)) >> 31); }
		static auto slot(DataArrayId id) -> size_type	 { return static_cast<size_type>( id & UINT64_C(0x000000007FFFFFFF)       ); }

		static auto setFree_and_increaseVersion(DataArrayId id) -> DataArrayId
		{
			//std::cout << std::hex << "initial id: 0x" << std::setw(16) << std::setfill('0') << id << std::endl;

			DataArrayId vn = version(id); //conversion from size_type to DataArrayId

			//std::cout << "version   : 0x" << std::setw(16) << std::setfill('0') << vn << std::endl;
			
			#if DYNAMIC_ASSERTS
				if (vn == UINT64_C(0xFFFFFFFF))
					throw std::runtime_error("The maximum version has been reached.");

				if (is_free(id))
					throw std::runtime_error("Unexpected state: the id was already set to free.");
			#endif
			
			++vn;

			//std::cout << "++version : 0x" << std::setw(16) << std::setfill('0') << vn << std::endl;

			//DataArrayId new_id = id = (vn << 32) | (id & UINT64_C(0x00000000FFFFFFFF));
			//std::cout << "new_id    : 0x" << std::setw(16) << std::setfill('0')  << new_id << std::dec << std::endl;

			return id = UINT64_C(0x8000000000000000) | (vn << 31) | (id & UINT64_C(0x000000007FFFFFFF));
		}


	friend DataArrayIterator<T, Resizable>;
	friend DataArrayConstIterator<T, Resizable>;
	
    template <typename T_, bool Resizable_>
	friend auto operator<<(std::ofstream & ofs, DataArray<T_, Resizable_> const& da) -> std::ofstream &;

    template <typename T_, bool Resizable_>
	friend auto operator>>(std::ifstream & ifs, DataArray<T_, Resizable_> & da) -> std::ifstream &;

    template <typename T_, bool Resizable_>
    friend auto operator<<(Logger & lgr, DataArray<T_, Resizable_> const& da) -> Logger &;
};




//////////////////////////
//						//
//		ITERATORS		//
//						//
//////////////////////////


// Forward declaration to allow friendship of full-specialization
template<typename T, bool Resizable, bool IsConst>
bool operator==(DataArrayIterImpl<T, Resizable, IsConst> const& lhs, DataArrayIterImpl<T, Resizable, IsConst> const& rhs);  


////
//  Template that generates both DataArrayConstIterator and DataArrayIterator.
//	@Resizable: Property of the underlying DataArray. It means that it automatically resize its size.
////
template<typename T, bool Resizable, bool IsConst>
class DataArrayIterImpl
{
	private:
        using DataArrayPointer = typename std::conditional_t< IsConst, DataArray<T, Resizable> const*, DataArray<T, Resizable> * >;
        using DataArrayElPointer = typename std::conditional_t< IsConst, detail::DataArrayEl<T> const*, detail::DataArrayEl<T> * >;
		using DataArrayElReference = typename std::conditional_t< IsConst, detail::DataArrayEl<T> const&, detail::DataArrayEl<T> & >;

    public:
        //Iterator traits
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::conditional_t< IsConst, detail::DataArrayEl<T> const, detail::DataArrayEl<T> >;
        using pointer = DataArrayElPointer;
        using reference = DataArrayElReference;
        using iterator_category = std::forward_iterator_tag;

		////
		//	@data_array: A pointer to a constant DataArray if this is a const iterator or a pointer to a plain DataArray otherwise.
		////
		DataArrayIterImpl(DataArrayPointer data_array, DataArrayId starting_slot)
			: m_data_array(data_array)
		{
			#if DYNAMIC_ASSERTS
				if(!data_array)
					throw std::runtime_error("Invalid argument: data_array is null.");

				if(starting_slot > data_array->m_max_used)
					throw std::runtime_error("Invalid argument: starting slot identify an out-of-used-range slot.");
			#endif


			m_current_el = data_array->m_vec.data() + starting_slot;
			go_to_firstValidElement();
		}

		auto operator++() -> DataArrayIterImpl&
		{
			#if DYNAMIC_ASSERTS
				if (m_current_el == get_pastendEl())
					throw std::runtime_error("Out-of-range: incremented beyond the one-past-the-end position.");
			#endif


			++m_current_el;
			go_to_firstValidElement();

			return *this;
		}
		
		////
		//	@return: A const reference if this is a const iterator or a plain reference otherwise.
		////
		auto operator*() const -> DataArrayElReference
		{
			#if DYNAMIC_ASSERTS
				if (DataArray<T, Resizable>::is_free(m_current_el->id()))
					throw std::runtime_error("Invalid slot: dereferencing a freed slot.");
			#endif

			return *m_current_el;
		}
		
		////
		//	@return: A const reference if this is a const iterator or a plain reference otherwise.
		////
		auto operator->() const -> DataArrayElPointer
		{
			#if DYNAMIC_ASSERTS
				if (DataArray<T, Resizable>::is_free(m_current_el->id()))
					throw std::runtime_error("Invalid slot: dereferencing a freed slot.");
			#endif

			return m_current_el;
		}

	private:
		DataArrayPointer m_data_array = nullptr;
		DataArrayElPointer m_current_el = nullptr;

		////
		//	Browse the DataArray up to a valid element or to the past-the-end position.
		////
		void go_to_firstValidElement() 
		{ 
			auto pastend_el = get_pastendEl();
			while (m_current_el != pastend_el && DataArray<T, Resizable>::is_free(m_current_el->id()))
				++m_current_el; 
		}
		auto get_pastendEl() -> DataArrayElPointer { return m_data_array->m_vec.data() + m_data_array->m_max_used; }


	friend bool operator==<T, Resizable, IsConst>(DataArrayIterImpl<T, Resizable, IsConst> const& lhs, DataArrayIterImpl<T, Resizable, IsConst> const& rhs); //friend of the full-specialization of operator==
};

template <typename T, bool Resizable, bool IsConst>
bool operator==(DataArrayIterImpl<T, Resizable, IsConst> const& lhs, DataArrayIterImpl<T, Resizable, IsConst> const& rhs)
{
	//true if both they are the iterators of the same DataArray and they are currently pointing the same element.
	return lhs.m_data_array == rhs.m_data_array && lhs.m_current_el == rhs.m_current_el;
}

template <typename T, bool Resizable, bool IsConst>
bool operator!=(DataArrayIterImpl<T, Resizable, IsConst> const& lhs, DataArrayIterImpl<T, Resizable, IsConst> const& rhs)
{
	return !(lhs == rhs);
}




//////////////////////////////////////
//									//
//		IMPLEMENTATION OF SOME		//
//		 DataArray<T> METHODS		//
//		(those requiring a full-	//
//		definition of iterators)	//
//									//
//////////////////////////////////////


template <typename T, bool Resizable>
auto DataArray<T, Resizable>::begin() noexcept -> DataArrayIterator<T, Resizable> { return DataArrayIterator<T, Resizable>(this, 0); }

template <typename T, bool Resizable>
auto DataArray<T, Resizable>::end() noexcept -> DataArrayIterator<T, Resizable> { return DataArrayIterator<T, Resizable>(this, m_max_used); }

template <typename T, bool Resizable>
auto DataArray<T, Resizable>::begin() const noexcept -> DataArrayConstIterator<T, Resizable> { return DataArrayConstIterator<T, Resizable>(this, 0); }

template <typename T, bool Resizable>
auto DataArray<T, Resizable>::end() const noexcept -> DataArrayConstIterator<T, Resizable> { return DataArrayConstIterator<T, Resizable>(this, m_max_used); }

template <typename T, bool Resizable>
auto DataArray<T, Resizable>::cbegin() const noexcept -> DataArrayConstIterator<T, Resizable> { return DataArrayConstIterator<T, Resizable>(this, 0); }

template <typename T, bool Resizable>
auto DataArray<T, Resizable>::cend() const noexcept -> DataArrayConstIterator<T, Resizable> { return DataArrayConstIterator<T, Resizable>(this, m_max_used); }


namespace detail
{
	template<typename T_>
	auto operator<<(std::ofstream & ofs, DataArrayEl<T_> const& da_el) -> std::ofstream &
	{
		ofs << da_el.m_id << ' ';
		ofs << da_el.value;

		return ofs;
	}
	
	template<typename T_>
	auto operator>>(std::ifstream & ifs, DataArrayEl<T_> & da_el) -> std::ifstream &
	{
		ifs >> da_el.m_id;
		ifs >> da_el.value;

		return ifs;
	}
	
	template<typename T_>
	auto operator<<(Logger & lgr, tgm::detail::DataArrayEl<T_> const& da_el) -> Logger &
	{
		lgr << "DataArrayEl {"
			<< Logger::addt
			<< Logger::nltb				<< "free    : "	<<(DataArray<T_, false>::debug_isFree(da_el.m_id) ? "yes" : "no") << std::noboolalpha
			<< Logger::nltb				<< "version : "	<< DataArray<T_, false>::debug_version(da_el.m_id)
			<< Logger::nltb				<< "slot    : "	<< DataArray<T_, false>::debug_slot(da_el.m_id)
			<< Logger::nltb << std::hex	<< "id      : 0x"<< std::setw(16) << std::setfill('0') << da_el.m_id << std::dec
			<< Logger::nltb << "element: "	<< da_el.value
			<< Logger::remt
			<< Logger::nltb << "}";

		return lgr;
	}
}



////
//	Convert a DataArrayId in a human readable string.
////
inline auto human_did(DataArrayId const did) -> std::string
{
	std::ostringstream oss;
	oss << DataArray<int>::debug_slot(did) << " (v" << DataArray<int>::debug_version(did) << ')'; // The type "int" is used just to access the static function debug_slot that is independent from the tempate parameter

	return oss.str();
}

template <typename T_, bool Resizable_>
auto operator<<(std::ofstream & ofs, DataArray<T_, Resizable_> const& da) -> std::ofstream &
{
	ofs << Resizable_ << '\n';
	ofs << da.m_vec << '\n'; //it returns the base class, I can't append another operator<<
	ofs << da.m_free << '\n';
	ofs << da.m_max_size << ' ';
	ofs << da.m_max_used << ' ';
	ofs << da.m_count;

	return ofs;
}

template <typename T_, bool Resizable_>
auto operator>>(std::ifstream & ifs, DataArray<T_, Resizable_> & da) -> std::ifstream &
{
	auto resizable = false;
	ifs >> resizable;
	if (resizable != Resizable_) { throw std::runtime_error("The input DataArray has a different Resizable status."); };

	if (!(ifs >> da.m_vec)) { throw std::runtime_error("Error while reading DataArray m_vec vector."); };
	if (!(ifs >> da.m_free)) { throw std::runtime_error("Error while reading DataArray m_free vector."); };

	auto max_size = typename DataArray<T_, Resizable_>::size_type{ 0 };
	ifs >> max_size;
	da.set_maxSize(max_size);

	ifs >> da.m_max_used;
	ifs >> da.m_count;

	if (!ifs) { throw std::runtime_error("Error while reading DataArray members."); };
	return ifs;
}

template <typename T_, bool Resizable_>
auto operator<<(Logger & lgr, DataArray<T_, Resizable_> const& da) -> Logger &
{
	lgr << "DataArray {"
		<< Logger::addt
		<< Logger::nltb << "max_used: " << da.m_max_used
		<< Logger::nltb << "m_free.vertices_count(): " << da.m_free.size()
		<< Logger::nltb << "m_vec.vertices_count(): " << da.m_vec.size()
		<< Logger::nltb << "m_max_size: " << da.m_max_size;

	for (auto e : da.m_vec)
		lgr << "\n\n" << Logger::nltb << e << ",";

	lgr << Logger::remt
		<< Logger::nltb << "}";

	return lgr;
}



} //namespace tgm


#endif //GM_DATA_ARRAY_HH
