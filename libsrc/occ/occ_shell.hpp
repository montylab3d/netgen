#ifndef FILE_OCC_SHELL_INCLUDED
#define FILE_OCC_SHELL_INCLUDED

#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>

#include "meshing.hpp"
#include "occ_utils.hpp"

namespace netgen
{
    class OCCShell : public GeometryShape
    {
        TopoDS_Shell shell;
        T_Shape tshell;

        public:
        OCCShell( ) = default;
        OCCShell( const TopoDS_Shape & s )
            : shell(TopoDS::Shell(s)),
              tshell(s.TShape())
        { }
        ~OCCShell() {}
        size_t GetHash() const override
        {
            return ShapeHash(shell);
        }
        T_Shape TShape() { return tshell; }
        const TopoDS_Shape & Shape() const { return shell; }
    };
}

#endif // FILE_OCC_SHELL_INCLUDED
