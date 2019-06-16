%module wfc
%{
    #include "../HelperClasses.h"
%}

//Import all the custom collections.
//Don't use their begin() / end() functions; that's just for C++ ranged-for loops.
%ignore *::begin();
%ignore *::begin() const;
%ignore *::end();
%ignore *::end() const;
%include "../Set.hpp"
%include "../List.hpp"
%include "../Dictionary.hpp"
%include "../Array2D.hpp"
%include "../Array3D.hpp"

//Bring in some other "helper" types.
%include "../EnumFlags.h"
%include "../Nullable.hpp"

//Generate specific versions of the above templates.
//For now, just make some up for debugging.
%template(eFlagsList) WFC::List<unsigned short>;
%template(eFlags) WFC::EnumFlags<unsigned short, int, 55>;
%template(dictIntDouble) WFC::Dictionary<int, double, std::hash<int>>;
%template(setOfUInts) WFC::Set<unsigned int, std::hash<unsigned int>>;
%template(algoState) WFC::Nullable<bool>;