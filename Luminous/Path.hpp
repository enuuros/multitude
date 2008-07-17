#ifndef FP_PATH_HPP
#define FP_PATH_HPP

#include <Valuable/HasValues.hpp>

#include <Nimble/Matrix3.hpp>
#include <Nimble/Vector2.hpp>

#include <Luminous/GLSLProgramObject.hpp>
#include <Luminous/TCBSpline.hpp>

#include <vector>

namespace Luminous {

  class Path : public Valuable::HasValues
  {
    public:
      Path();

      void addPoint(Nimble::Vector2f p) { m_points.push_back(p); }

      void clear() { m_points.clear(); }

      void render();

      void renderSpline();
      void renderLineStrip(const Nimble::Matrix3f & m) const;

      void renderDebug() const;

      void prepare();

      void simplify(float clusterTolerance, float dpTolerance);

      void simplifyAngular(float degrees);

      void transform(const Nimble::Matrix3f & m);

      size_t size() const { return m_points.size(); }
      Nimble::Vector2f point(size_t i) const { return m_points[i]; }

      virtual const char * const type() const { return "Path"; }
      virtual xercesc::DOMElement * serializeXML(xercesc::DOMDocument * doc);
      virtual bool deserializeXML(xercesc::DOMElement * element);

      Nimble::Vector2f center() const;

      static bool intersect(const Path & p1, const Nimble::Matrix3f & m1, const Path & p2, const Nimble::Matrix3f & m2);

      bool isDegenerate() const;

      TCBSpline2 * spline();

      void debugDump() const;

    protected:
      typedef std::vector<Nimble::Vector2f> container;

      container m_points;
  };

}

#endif
