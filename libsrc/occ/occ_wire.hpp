#ifndef FILE_OCC_WIRE_INCLUDED
#define FILE_OCC_WIRE_INCLUDED

#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

#include "meshing.hpp"
#include "occ_utils.hpp"

namespace netgen
{
    class OCCWire : public GeometryShape
    {
        TopoDS_Wire wire;
        T_Shape twire;

        public:
        OCCWire( ) = default;
        OCCWire( const TopoDS_Shape & s )
            : wire(TopoDS::Wire(s)),
              twire(s.TShape())
        { }
        ~OCCWire() {}
        size_t GetHash() const override
        {
	  return ShapeHash(wire);
        }
        T_Shape TShape() { return twire; }
        const TopoDS_Shape & Shape() const { return wire; }
    };
}

#endif // FILE_OCC_WIRE_INCLUDED
