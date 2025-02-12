#ifndef FILE_OCCGEOM
#define FILE_OCCGEOM

/* *************************************************************************/
/* File:   occgeom.hpp                                                     */
/* Author: Robert Gaisbauer                                                */
/* Date:   26. May  03                                                     */
/* *************************************************************************/

#ifdef OCCGEOMETRY

#include <meshing.hpp>
#include "occ_utils.hpp"
#include "occmeshsurf.hpp"

#include <Quantity_ColorRGBA.hxx>
#include <STEPCAFControl_Reader.hxx>
#include <StepBasic_MeasureValueMember.hxx>
#include <StepRepr_CompoundRepresentationItem.hxx>
#include <StepRepr_IntegerRepresentationItem.hxx>
#include <StepRepr_ValueRepresentationItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TDocStd_Document.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Transfer_FinderProcess.hxx>

#if OCC_VERSION_MAJOR>=7 && OCC_VERSION_MINOR>=4
#define OCC_HAVE_HISTORY
#endif

namespace netgen
{

  // extern DLL_HEADER MeshingParameters mparam;

#define PROJECTION_TOLERANCE 1e-10

#define ENTITYISVISIBLE 1
#define ENTITYISHIGHLIGHTED 2
#define ENTITYISDRAWABLE 4

#define OCCGEOMETRYVISUALIZATIONNOCHANGE   0
#define OCCGEOMETRYVISUALIZATIONFULLCHANGE 1  // Compute transformation matrices and redraw
#define OCCGEOMETRYVISUALIZATIONHALFCHANGE 2  // Redraw


  class EntityVisualizationCode
  {
    int code;

  public:

    EntityVisualizationCode()
    {  code = ENTITYISVISIBLE + !ENTITYISHIGHLIGHTED + ENTITYISDRAWABLE;}

    int IsVisible ()
    {  return code & ENTITYISVISIBLE;}

    int IsHighlighted ()
    {  return code & ENTITYISHIGHLIGHTED;}

    int IsDrawable ()
    {  return code & ENTITYISDRAWABLE;}

    void Show ()
    {  code |= ENTITYISVISIBLE;}

    void Hide ()
    {  code &= ~ENTITYISVISIBLE;}

    void Highlight ()
    {  code |= ENTITYISHIGHLIGHTED;}

    void Lowlight ()
    {  code &= ~ENTITYISHIGHLIGHTED;}

    void SetDrawable ()
    {  code |= ENTITYISDRAWABLE;}

    void SetNotDrawable ()
    {  code &= ~ENTITYISDRAWABLE;}
  };



  class Line
  {
  public:
    Point<3> p0, p1;
    double Dist (Line l);
    double Length () { return (p1-p0).Length(); }
  };
  


  inline double Det3 (double a00, double a01, double a02,
                      double a10, double a11, double a12,
                      double a20, double a21, double a22)
  {
    return a00*a11*a22 + a01*a12*a20 + a10*a21*a02 - a20*a11*a02 - a10*a01*a22 - a21*a12*a00;
  }
  
  class DLL_HEADER OCCParameters
  {
  public:

    /// Factor for meshing close edges, moved to meshingparameters
    // double resthcloseedgefac = 2.;

    /// Enable / Disable detection of close edges
    // int resthcloseedgeenable = true;

    /// Minimum edge length to be used for dividing edges to mesh points
    double resthminedgelen = 0.001;

    /// Enable / Disable use of the minimum edge length (by default use 1e-4)
    int resthminedgelenenable = true;

    /*!
      Dump all the OpenCascade specific meshing parameters 
      to console
    */
    void Print (ostream & ost) const;
  };


  class DLL_HEADER OCCGeometry : public NetgenGeometry
  {
    Point<3> center;
    OCCParameters occparam;
  public:
    static std::map<T_Shape, ShapeProperties> global_shape_properties;
    static std::map<T_Shape, std::vector<OCCIdentification>> identifications;

    TopoDS_Shape shape;
    TopTools_IndexedMapOfShape fmap, emap, vmap, somap, shmap, wmap; // legacy maps
    NgArray<bool> fsingular, esingular, vsingular;
    Box<3> boundingbox;

    std::map<T_Shape, int> edge_map, vertex_map, face_map, solid_map;

    mutable int changed;
    mutable NgArray<int> facemeshstatus;

    // Philippose - 15/01/2009
    // Maximum mesh size for a given face
    // (Used to explicitly define mesh size limits on individual faces)
    NgArray<double> face_maxh;
     
    // Philippose - 14/01/2010
    // Boolean array to detect whether a face has been explicitly modified 
    // by the user or not
    NgArray<bool> face_maxh_modified;
     
    // Philippose - 15/01/2009
    // Indicates which faces have been selected by the user in geometry mode
    // (Currently handles only selection of one face at a time, but an array would
    //  help to extend this to multiple faces)
    NgArray<bool> face_sel_status;
     
    NgArray<EntityVisualizationCode> fvispar, evispar, vvispar;
     
    double tolerance;
    bool fixsmalledges;
    bool fixspotstripfaces;
    bool sewfaces;
    bool makesolids;
    bool splitpartitions;

    OCCGeometry()
    {
      somap.Clear();
      shmap.Clear();
      fmap.Clear();
      wmap.Clear();
      emap.Clear();
      vmap.Clear();
    }

    OCCGeometry(const TopoDS_Shape& _shape, int aoccdim = 3, bool copy = false);

    Mesh::GEOM_TYPE GetGeomType() const override
    { return Mesh::GEOM_OCC; }

    void SetOCCParameters(const OCCParameters& par)
    { occparam = par; }

    void Analyse(Mesh& mesh,
                 const MeshingParameters& mparam) const override;
    bool MeshFace(Mesh& mesh, const MeshingParameters& mparam,
                     int nr, FlatArray<int, PointIndex> glob2loc) const override;
    // void OptimizeSurface(Mesh& mesh, const MeshingParameters& mparam) const override {}
 
    void Save (string filename) const override;
    void SaveToMeshFile (ostream & /* ost */) const override;
     
    void DoArchive(Archive& ar) override;

    void BuildFMap();

    auto GetShape() const { return shape; }
    Box<3> GetBoundingBox() const
    { return boundingbox; }

    int NrSolids() const
    { return somap.Extent(); }

    // Philippose - 17/01/2009
    // Total number of faces in the geometry
    int NrFaces() const
    { return fmap.Extent(); }

    void SetCenter()
    { center = boundingbox.Center(); }

    Point<3> Center() const
    { return center; }

    OCCSurface GetSurface (int surfi)
    {
      cout << "OCCGeometry::GetSurface using PLANESPACE" << endl;
      return OCCSurface (TopoDS::Face(fmap(surfi)), PLANESPACE);
    }

    void CalcBoundingBox ();
    void BuildVisualizationMesh (double deflection);
    
    void RecursiveTopologyTree (const TopoDS_Shape & sh,
                                stringstream & str,
                                TopAbs_ShapeEnum l,
                                bool free,
                                const char * lname);

    void GetTopologyTree (stringstream & str);

    void PrintNrShapes ();

    void CheckIrregularEntities (stringstream & str);

    void SewFaces();

    void MakeSolid();

    void HealGeometry();
    void GlueGeometry();

    // Philippose - 15/01/2009
    // Sets the maximum mesh size for a given face
    // (Note: Local mesh size limited by the global max mesh size)
    void SetFaceMaxH(int facenr, double faceh, const MeshingParameters & mparam)
    {
      if((facenr> 0) && (facenr <= fmap.Extent()))
        {
          face_maxh[facenr-1] = min(mparam.maxh,faceh);
            
          // Philippose - 14/01/2010
          // If the face maxh is greater than or equal to the 
          // current global maximum, then identify the face as 
          // not explicitly controlled by the user any more
          if(faceh >= mparam.maxh)
            {
              face_maxh_modified[facenr-1] = 0;
            }
          else
            {
              face_maxh_modified[facenr-1] = 1;
            }
        }
    }

    void SetFaceMaxH(size_t facenr, double faceh)
    {
      if(facenr >= fmap.Extent())
        throw RangeException("OCCGeometry faces", facenr, 0, fmap.Extent());
      face_maxh[facenr] = faceh;
      face_maxh_modified[facenr] = true;
    }

    // Philippose - 15/01/2009
    // Returns the local mesh size of a given face
    double GetFaceMaxH(int facenr)
    {
      if((facenr> 0) && (facenr <= fmap.Extent()))
        {
          return face_maxh[facenr-1];
        }
      else
        {
          return 0.0;
        }
    }
      
    // Philippose - 14/01/2010
    // Returns the flag whether the given face 
    // has a mesh size controlled by the user or not
    bool GetFaceMaxhModified(int facenr)
    {
      return face_maxh_modified[facenr-1];
    }
      
    // Philippose - 17/01/2009
    // Returns the index of the currently selected face
    int SelectedFace()
    {
      for(int i = 1; i <= fmap.Extent(); i++)
        {
          if(face_sel_status[i-1])
            {
              return i;
            }
        }

      return 0;
    }

    // Philippose - 17/01/2009
    // Sets the currently selected face
    void SetSelectedFace(int facenr)
    {
      face_sel_status = 0;

      if((facenr >= 1) && (facenr <= fmap.Extent()))
        {
          face_sel_status[facenr-1] = 1;
        }
    }

    void LowLightAll()
    {
      for (int i = 1; i <= fmap.Extent(); i++)
        fvispar[i-1].Lowlight();
      for (int i = 1; i <= emap.Extent(); i++)
        evispar[i-1].Lowlight();
      for (int i = 1; i <= vmap.Extent(); i++)
        vvispar[i-1].Lowlight();
    }

    void GetUnmeshedFaceInfo (stringstream & str);
    void GetNotDrawableFaces (stringstream & str);
    bool ErrorInSurfaceMeshing ();

    //      void WriteOCC_STL(char * filename);
    static void IdentifyEdges(const TopoDS_Shape & me, const TopoDS_Shape & you, string name, Identifications::ID_TYPE type);
    static void IdentifyFaces(const TopoDS_Shape & solid,const TopoDS_Shape & me, const TopoDS_Shape & you, string name, Identifications::ID_TYPE type);

  private:
    //bool FastProject (int surfi, Point<3> & ap, double& u, double& v) const;
  };
   

  void PrintContents (OCCGeometry * geom);

  DLL_HEADER OCCGeometry * LoadOCC_IGES (const char * filename);
  DLL_HEADER OCCGeometry * LoadOCC_STEP (const char * filename);
  DLL_HEADER OCCGeometry * LoadOCC_BREP (const char * filename);

  // Philippose - 31.09.2009
  // External access to the mesh generation functions within the OCC
  // subsystem (Not sure if this is the best way to implement this....!!)
  DLL_HEADER extern void OCCSetLocalMeshSize(const OCCGeometry & geom, Mesh & mesh, const MeshingParameters & mparam,
                                             const OCCParameters& occparam);

  DLL_HEADER extern bool OCCMeshFace (const OCCGeometry & geom, Mesh & mesh, FlatArray<int, PointIndex> glob2loc,
                       const MeshingParameters & mparam, int nr, int projecttype, bool delete_on_failure);


  namespace step_utils
  {
      inline Handle(TCollection_HAsciiString) MakeName (string s)
      {
          return new TCollection_HAsciiString(s.c_str());
      };

      inline Handle(StepRepr_RepresentationItem) MakeInt (int n, string name = "")
      {
          Handle(StepRepr_IntegerRepresentationItem) int_obj = new StepRepr_IntegerRepresentationItem;
          int_obj->Init(MakeName(name), n);
          return int_obj;
      }

      inline int ReadInt (Handle(StepRepr_RepresentationItem) item)
      {
          return Handle(StepRepr_IntegerRepresentationItem)::DownCast(item)->Value();
      }

      inline Handle(StepRepr_RepresentationItem) MakeReal (double val, string name = "")
      {
            Handle(StepBasic_MeasureValueMember) value_member = new StepBasic_MeasureValueMember;
            value_member->SetReal(val);
            Handle(StepRepr_ValueRepresentationItem) value_repr = new StepRepr_ValueRepresentationItem;
            value_repr->Init(MakeName(name), value_member);
            return value_repr;
      }

      inline double ReadReal (Handle(StepRepr_RepresentationItem) item)
      {
          return Handle(StepRepr_ValueRepresentationItem)::DownCast(item)
              ->ValueComponentMember()->Real();
      }


      inline Handle(StepRepr_RepresentationItem) MakeCompound( FlatArray<Handle(StepRepr_RepresentationItem)> items, string name = "" )
      {
            Handle(StepRepr_HArray1OfRepresentationItem) array_repr = new StepRepr_HArray1OfRepresentationItem(1,items.Size());

            for(auto i : Range(items))
                array_repr->SetValue(i+1, items[i]);

            Handle(StepRepr_CompoundRepresentationItem) comp = new StepRepr_CompoundRepresentationItem;
            comp->Init( MakeName(name), array_repr );
            return comp;
      }

      void WriteIdentifications(const Handle(Interface_InterfaceModel) model, const TopoDS_Shape & shape, const Handle(Transfer_FinderProcess) finder);
      void ReadIdentifications(Handle(StepRepr_RepresentationItem) item, Handle(Transfer_TransientProcess) transProc);

      inline Quantity_ColorRGBA MakeColor(const Vec<4> & c)
      {
          return Quantity_ColorRGBA (c[0], c[1], c[2], c[3]);
      }

      inline Vec<4> ReadColor (const Quantity_ColorRGBA & c)
      {
          auto rgb = c.GetRGB();
          return {rgb.Red(), rgb.Green(), rgb.Blue(), c.Alpha()};
      }


      void LoadProperties(const TopoDS_Shape & shape,
                          const STEPCAFControl_Reader & reader,
                          const Handle(TDocStd_Document) step_doc);
      void WriteProperties(const Handle(Interface_InterfaceModel) model, const Handle(Transfer_FinderProcess) finder, const TopoDS_Shape & shape);

      void WriteSTEP(const TopoDS_Shape & shape, string filename);

      inline void WriteSTEP(const OCCGeometry & geo, string filename)
      {
          WriteSTEP(geo.GetShape(), filename);
      }

      // deep copy, also ensures consistent shape ordering (face numbers etc.)
      TopoDS_Shape WriteAndRead(const TopoDS_Shape shape);
  } // namespace step_utils
}

#endif

#endif
