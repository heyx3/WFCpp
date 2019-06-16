%module wfc
%{
    #include "../HelperClasses.h"
	#include "../Tiled3D/Transform3D.h"
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
%}
%typemap(cscode) WFC::Vector3i %{
	#if UNITY_2018_1_OR_NEWER
	UnityEngine.Vector3Int ToUnityStruct() { return new UnityEngine.Vector3Int(x, y, z); }
	#endif
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

//Generate specific versions of the above templates.
//For now, just make some up for debugging.
%template(eFlagsList) WFC::List<unsigned short>;
%template(eFlags) WFC::EnumFlags<unsigned short, int, 55>;
%template(dictIntDouble) WFC::Dictionary<int, double, std::hash<int>>;
%template(setOfUInts) WFC::Set<unsigned int, std::hash<unsigned int>>;
%template(algoState) WFC::Nullable<bool>;