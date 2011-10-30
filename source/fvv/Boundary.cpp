#include "Boundary.hpp"

Boundary::Boundary() {
  hasLastPoint = false;
}

Boundary::~Boundary() {

}

void Boundary::begin() {
  hasLastPoint = false;
  outerEdges.clear();
  innerEdges.clear();
}

void Boundary::end() {
  if (hasLastPoint) {
    outerEdges.push_back(Edge(lastPoint, outerEdges[0].a));
  }
}

void Boundary::insert(Point p) {
  if (!hasLastPoint) {
    lastPoint = p;
    hasLastPoint = true;
  } else {
    outerEdges.push_back(Edge(lastPoint, p));
    lastPoint = p;
  }
}

void Boundary::calculate() {
  int blendingBorder = 25.0;
  innerEdges.clear();

  vector<Edge> newOuterEdges;
  for (int i = 0; i < outerEdges.size(); i++) {
    int prev = (i + outerEdges.size() - 1) % outerEdges.size();
    int cur = i;
    int next = (i + 1) % outerEdges.size();

    Edge prevEdge = outerEdges[prev].offset(blendingBorder / 2.0);
    Edge curEdge = outerEdges[cur].offset(blendingBorder / 2.0);
    Edge nextEdge = outerEdges[next].offset(blendingBorder / 2.0);

    Point a = curEdge.intersection(prevEdge);
    Point b = curEdge.intersection(nextEdge);

    innerEdges.push_back(Edge(a, b));

    prevEdge = outerEdges[prev].offset(-blendingBorder / 2.0);
    curEdge = outerEdges[cur].offset(-blendingBorder / 2.0);
    nextEdge = outerEdges[next].offset(-blendingBorder / 2.0);

    a = curEdge.intersection(prevEdge);
    b = curEdge.intersection(nextEdge);

    newOuterEdges.push_back(Edge(a, b));
  }

  outerEdges = newOuterEdges;
}

void Boundary::render(bool invert) {
  // Calculate the geometry of the border
  calculate();

  //glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  // Render the border
  glBegin(GL_QUADS);
  for (int i = 0; i < outerEdges.size(); i++) {
    Edge out = outerEdges[i];
    Edge in = innerEdges[i];

    if (!invert) {
      glColor4f(0.0, 0.0, 0.0, 0.0);
    } else {
      glColor4f(1.0, 1.0, 1.0, 1.0);
    }
    glVertex3f(out.a.x, out.a.y, 0.0);
    glVertex3f(out.b.x, out.b.y, 0.0);
    
    if (!invert) {
      glColor4f(1.0, 1.0, 1.0, 1.0);
    } else {
      glColor4f(0.0, 0.0, 0.0, 0.0);
    }
    glVertex3f(in.b.x, in.b.y, 0.0);
    glVertex3f(in.a.x, in.a.y, 0.0);
  }
  glEnd();
  
  // Fill the border
  if (!invert) {
    glColor3f(1.0, 1.0, 1.0);
  } else {
    glColor3f(0.0, 0.0, 0.0);
  }

  GLUtesselator *tess = gluNewTess();

  gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (*) ()) &glBegin);
  gluTessCallback(tess, GLU_TESS_END, (GLvoid (*) ()) &glEnd);
  gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*) ()) &glVertex3dv); 
  
  gluTessProperty(tess, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);   
  gluTessNormal(tess, 0, 0, 1);

  gluTessBeginPolygon(tess, NULL);
  gluTessBeginContour(tess);

  GLdouble *vertices = new GLdouble[innerEdges.size() * 3];
  for (int i = 0; i < innerEdges.size(); i++) {
    vertices[(i * 3) + 0] = innerEdges[i].a.x;
    vertices[(i * 3) + 1] = innerEdges[i].a.y;
    vertices[(i * 3) + 2] = 0.0;

    gluTessVertex(tess, vertices + (i * 3), vertices + (i * 3));
  }

  gluTessEndContour(tess);
  gluTessEndPolygon(tess);

  delete [] vertices;
  gluDeleteTess(tess);
}

