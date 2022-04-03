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
        OCCSolid(const TopoDS_Shape & shape)
            : tsolid(shape.TShape()),
              solid(TopoDS::Solid(shape))
        { }

        TopoDS_Shape & Shape() { return solid; }
        T_Shape TShape() { return tsolid; }
        size_t GetHash() const override { return ShapeHash(solid); }
    };
}

#endif // FILE_OCC_SOLID_INCLUDED
