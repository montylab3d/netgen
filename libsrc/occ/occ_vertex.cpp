#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>

#include "occ_vertex.hpp"

namespace netgen
{

    OCCVertex::OCCVertex( const TopoDS_Shape & s )
        : vertex(TopoDS::Vertex(s)),
          tvertex(s.TShape())
    {
        p = occ2ng(vertex);
    }

    Point<3> OCCVertex::GetPoint() const
    {
        return p;
    }

    size_t OCCVertex::GetHash() const
    {
        return ShapeHash(vertex);
    }
}
