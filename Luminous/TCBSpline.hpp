#ifndef TCB_SPLINE_HPP
#define TCB_SPLINE_HPP

#include <Nimble/Vector2.hpp>

#include <vector>

class TCBSpline2 
{
  public:
    TCBSpline2() {}
    TCBSpline2(int segments,
        const std::vector<float> & time, 
        const std::vector<Nimble::Vector2f> & points, 
        const std::vector<float> & tension,
        const std::vector<float> & continuity, 
        const std::vector<float> & bias);

    virtual ~TCBSpline2();

    Nimble::Vector2f value(float t) const;
    Nimble::Vector2f firstDerivative(float t) const;

    void render() const;
    void renderQuads(float step, float thickness) const;

  protected:
    void computePoly(int i0, int i1, int i2, int i3);

    void getKeyInfo(float t, int & key, float & dt) const;
   
    size_t m_segments; 
    std::vector<float> m_time;

    std::vector<Nimble::Vector2f> m_points;
    std::vector<float> m_tension;
    std::vector<float> m_continuity;
    std::vector<float> m_bias;

    std::vector<Nimble::Vector2f> m_A;
    std::vector<Nimble::Vector2f> m_B;
    std::vector<Nimble::Vector2f> m_C;
    std::vector<Nimble::Vector2f> m_D;
};

#endif
