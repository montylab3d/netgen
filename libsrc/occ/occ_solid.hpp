#ifndef FILE_OCC_SOLID_INCLUDED
#define FILE_OCC_SOLID_INCLUDED

#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

#include "meshing.hpp"

namespace netgen
{
    class OCCSolid : public GeometrySolid 
    {
        T_Shape tsolid;
        TopoDS_Solid solid;

        public:
        OCCSolid(TopoDS_Shape & dshape)
            : tsolid(dshape.TShape()),
              solid(TopoDS::Solid(dshape))
        { }

        TopoDS_Shape & Shape() { return solid; }
        T_Shape TShape() { return tsolid; }
        size_t GetHash() const override
        {
	    return solid.HashCode(INT_MAX);
	}
    };
}

#endif // FILE_OCC_SOLID_INCLUDED
