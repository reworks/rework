///
/// Meta.hpp
/// quasar
///
/// Apache 2.0 LICENSE.
/// Refer to LICENSE.txt for more details.
///

#ifndef QUASAR_META_HPP_
#define QUASAR_META_HPP_

///
/// Core namespace.
///
namespace qs
{
	///
	/// \brief Specifies a static buffer at compile time.
	///
	/// Cannot be instansiated.
	///
	struct BufferTypeDynamic final
	{
		BufferTypeDynamic() = delete;
	};

	///
	/// \brief Specifies a dynamic buffer at compile time.
	///
	/// Cannot be instansiated.
	///
	struct BufferTypeStatic final
	{
		BufferTypeStatic() = delete;
	};

	///
	/// \brief Specifies "position" vertex attribute type at compile time.
	///
	/// Cannot be instansiated.
	///
	struct VATypePosition final
	{
		VATypePosition() = delete;
	};

	///
	/// \brief Specifies "colour" vertex attribute type at compile time.
	///
	/// Cannot be instansiated.
	///
	struct VATypeColour final
	{
		VATypeColour() = delete;
	};

	///
	/// \brief Specifies "texel" vertex attribute type at compile time.
	///
	/// Cannot be instansiated.
	///
	struct VATypeTexel final
	{
		VATypeTexel() = delete;
	};
}

#endif