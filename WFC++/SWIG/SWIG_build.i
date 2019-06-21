%module wfc
%{
    #include "../HelperClasses.h"
	#include "../Tiled3D/State.h"
	#include "../Tiled3D/TilePermutator.h"
%}

%naturalvar;

//Import the various integer type-defs.
%import <stdint.i>

//Import things like the WFC_API macro.
%import "../Platform.h"

//Ignore ranged-for-loop stuff.
%ignore *::begin();
%ignore *::begin() const;
%ignore *::end();
%ignore *::end() const;
%ignore *::const_iterator;
%ignore *::iterator;

//Vector2i and Vector3i won't export their operators without additional work,
//    but they shouldn't be used that often in C# anyway.
// https://github.com/swig/swig/issues/886
//If in the Unity game engine, provide an automatic converter to the Unity type.
%typemap(cscode) WFC::Vector2i %{
	#if UNITY_2018_1_OR_NEWER
	UnityEngine.Vector2Int ToUnityStruct() { return new UnityEngine.Vector2Int(x, y); }
	#endif

	public override int GetHashCode() { return GetHashcode(); }
%}
%typemap(cscode) WFC::Vector3i %{
	#if UNITY_2018_1_OR_NEWER
	UnityEngine.Vector3Int ToUnityStruct() { return new UnityEngine.Vector3Int(x, y, z); }
	#endif

	public override int GetHashCode() { return GetHashcode(); }
%}
%include "../Vector2i.h"
%include "../Vector3i.h"

//Bring in the other helper classes.
%include "../Nullable.hpp"
%include "../EnumFlags.h"
%include "../Set.hpp"
%include "../List.hpp"
%include "../Dictionary.hpp"
%include "../Array2D.hpp"
%include "../Array3D.hpp"


//Bring in Tiled3D.
%include "../Tiled3D/Transform3D.h"
%include "../Tiled3D/Tile.hpp"
%include "../Tiled3D/InputData.h"
%include "../Tiled3D/TilePermutator.h"
%include "../Tiled3D/State.h"

//Generate specific types from templates.
%typemap (cscode) WFC::Nullable<bool> %{
	public bool? ToStruct(bool disposeMe = false) {
		bool? result = null;
		if (HasValue)
			result = Value;

		if (disposeMe)
			Dispose();

		return result;
	}
%}
%typemap (cscode) WFC::Nullable<WFC::Tiled3D::TileID> %{
	public uint? ToStruct(bool disposeMe = false) {
		uint? result = null;
		if (HasValue)
			result = Value;

		if (disposeMe)
			Dispose();

		return result;
	}
%}
%template (MaybeBool) WFC::Nullable<bool>;
%template (MaybeTile) WFC::Nullable<WFC::Tiled3D::TileID>;
%template(TileList) WFC::List<WFC::Tiled3D::Tile>;
%template(TileIDList) WFC::List<WFC::Tiled3D::TileID>;
%template(Transform3DList) WFC::List<WFC::Tiled3D::Transform3D>;
%template(TileSymmetryList) WFC::List<WFC::List<WFC::Tiled3D::Transform3D>>;
%template(TileIDSet) WFC::Set<WFC::Tiled3D::TileID, std::hash<WFC::Tiled3D::TileID>>;
%template(OutputArray3D) WFC::Array3D<WFC::Tiled3D::State::OutputTile>;
%template (Vec3iList) WFC::List<WFC::Vector3i>;