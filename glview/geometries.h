#ifndef GEOMETRIES_H
#define GEOMETRIES_H

#include<QMatrix4x4>
#include<QMatrix3x3>
#include<QVector3D>
#include<QOpenGLShader>
#include<QOpenGLShaderProgram>
#include<QOpenGLBuffer>
#include<QOpenGLVertexArrayObject>
#include<QOpenGLFramebufferObject>
#include<QOpenGLTexture>
#include<QFont>
#include<QFontMetrics>
#include<QColor>
#include<QPainter>
#include<QRect>

#include <Eigen/Eigen>
#include <glview/camera.h>

struct SphereData {
  QVector<QVector3D>        positions;
  QVector<float>            radius;
  QVector<QVector4D>        colors;
  QVector<int>              indecies;

  QVector<QVector4D>         dataColor;
  QVector<QVector3D>         dataVertex;
  QVector<QVector2D>         dataTexCoord;
  QVector<QVector3D>         dataNormal;

  QVector<GLuint>          dataIndex;

  void clear() {
    colors.clear();
    positions.clear();
    radius.clear();
    indecies.clear();

    dataColor.clear();
    dataTexCoord.clear();
    dataVertex.clear();
    dataNormal.clear();

    dataIndex.clear();
  }
};
struct SmoothSphereGeometry {

    QOpenGLShaderProgram program;

    QOpenGLVertexArrayObject   vao;
    QOpenGLBuffer              bufferIndex;

    QOpenGLBuffer              bufferColor;
    QOpenGLBuffer              bufferTexCoord;
    QOpenGLBuffer              bufferVertex;

    QOpenGLBuffer              bufferNormal;
    QOpenGLBuffer              bufferCenter;

    SphereData                 selfData;
    SphereData                *data;
    bool                       dataChanged;

    bool isRenderRequired;


    SmoothSphereGeometry() ;
    ~SmoothSphereGeometry() ;
    void initialize(QString vglslSource,QString fglslSource);
    void uploadData();
    void updateData(SphereData * sphereData) ;
    void updateData() ;

    void render(Camera & camera);
    std::multimap<float, int>    hits(const QVector3D &rayOrigin,
                         const QVector3D &rayEnd,
                         const QVector3D &rayDirection) const;


    std::multimap<float, int> hits(QPoint point,Camera & camera) const;
};

struct CylinderData {

  QVector<QVector3D>        positions1;
  QVector<QVector3D>        positions2;
  QVector<float>            radius;
  QVector<QVector4D>        colors1;
  QVector<QVector4D>        colors2;
  QVector<int>              indecies;

  QVector<QMatrix4x4>       modelTransformation;

  QVector<QVector4D>         dataColor;
  QVector<QVector3D>         dataVertex;
  QVector<QVector3D>         dataNormal;
  QVector<GLuint>            dataIndex;

  void clear() {

    positions1.clear();
    positions2.clear();
    radius.clear();
    colors1.clear();
    colors2.clear();
    indecies.clear();

    modelTransformation.clear();

    dataColor.clear();
    dataVertex.clear();
    dataNormal.clear();
    dataIndex.clear();

  }

  QVector<QVector3D>         singleDataVertex;
  QVector<QVector3D>         singleDataNormal;
  QVector<GLuint>            singleDataIndex;

  CylinderData() {
    int segments = 10;
    float segment_angle = 2.0*3.1415/segments;
    for(int s = 0; s < segments; s++) {
        singleDataVertex.push_back({std::cos(s*segment_angle),std::sin(s*segment_angle),0.0});
        singleDataVertex.push_back({std::cos(s*segment_angle),std::sin(s*segment_angle),1.0});

        singleDataNormal.push_back({std::cos(s*segment_angle),std::sin(s*segment_angle),0.0});
        singleDataNormal.push_back({std::cos(s*segment_angle),std::sin(s*segment_angle),0.0});
      }

    for(int s = 0; s < segments*2; s+=2) {
        singleDataIndex.push_back(s);
        singleDataIndex.push_back(s+1);
        singleDataIndex.push_back((s+3>=segments*2)?1:(s+3));
        singleDataIndex.push_back((s+2>=segments*2)?0:(s+2));
      }

  }



};
struct SmoothCylinderGeometry {

    QOpenGLShaderProgram program;

    QOpenGLVertexArrayObject   vao;
    QOpenGLBuffer              bufferIndex;

    QOpenGLBuffer              bufferColor;
    QOpenGLBuffer              bufferTexCoord;
    QOpenGLBuffer              bufferVertex;

    QOpenGLBuffer              bufferNormal;
    QOpenGLBuffer              bufferCenter;

    CylinderData               selfData;
    CylinderData                *data;
    bool                       dataChanged;

    bool isRenderRequired;

    SmoothCylinderGeometry();

    ~SmoothCylinderGeometry();

    void initialize();

    void uploadData();

    void updateData(CylinderData * CylinderData);
    void updateData();
    void render(Camera & camera);

    std::multimap<float, int> hits(const QVector3D &rayOrigin,
                         const QVector3D &rayEnd,
                         const QVector3D &rayDirection) const;
    std::multimap<float, int> hits(QPoint point,Camera & camera) const;
};

struct LineData {

  QVector<QVector3D>        positions1;
  QVector<QVector3D>        positions2;
  QVector<float>            width;
  QVector<QVector4D>        colors;
  QVector<int>              indecies;

  QVector<QVector4D>         dataColor;
  QVector<QVector3D>         dataVertex;
  QVector<GLuint>            dataIndex;

  void clear() {

    positions1.clear();
    positions2.clear();
    width.clear();
    colors.clear();
    indecies.clear();

    dataColor.clear();
    dataVertex.clear();
    dataIndex.clear();

  }

};
struct LineGeometry {

    QOpenGLShaderProgram program;

    QOpenGLVertexArrayObject   vao;
    QOpenGLBuffer              bufferIndex;

    QOpenGLBuffer              bufferColor;
    QOpenGLBuffer              bufferVertex;

    LineData               selfData;
    LineData                *data;
    bool                       dataChanged;

    bool isRenderRequired;


    LineGeometry() ;
    ~LineGeometry();

    void initialize();
    void uploadData();
    void updateData(LineData * lineData);
    void updateData() ;
    void render(Camera & camera);

};

struct CharTextureAtlas {
  QString           charTable;
  QFont             charFont;
  QFontMetrics      charMetrics;

  QMap<QChar,QRect>  charPositionMap;
  QMap<QChar,QRectF> charNormalizedPositionMap;

  QImage            charImage;

  int pixelsWide;
  int pixelsHigh;

  CharTextureAtlas(const QFont charFont,
                   const QColor color,
                   const QString charTable);

  QImage getAtlasImage();

  QImage getCharImage(QChar ch);
  int addVerexTexCoords(QVector<QVector4D> &texturePoints,const QString & text, float scalew,float scaleh);

};
struct TextData {

  CharTextureAtlas charTextureAtlas;  // this is very slow and memory abuser, have to modify this!!!!!!!!!!!!!


  QVector<QVector3D>        positions;
  QVector<QString>          texts;

  QVector<QVector3D>         dataPosition;
  QVector<QVector4D>         dataVertexTexCoord;

  TextData();
  void clear();
};
struct TextGeometry {

   QOpenGLTexture texture;

   QOpenGLShaderProgram program;

   QOpenGLVertexArrayObject   vao;

   QOpenGLBuffer              bufferVertexTexCoord;
   QOpenGLBuffer              bufferPosition;


   TextData                   selfData;
   TextData                   *data;
   bool                       dataChanged;

   bool isRenderRequired;

   TextGeometry() ;
   ~TextGeometry();

   void initialize();
   void uploadData();
   void updateData(TextData * textData);
   void updateData() ;
   void render(Camera & camera);

};

//////////// Specific objects ///////////////
/*
 *
SmoothSphereGeometry   atoms;
SmoothSphereGeometry   halos;
SmoothSphereGeometry   pickhalos;
SmoothCylinderGeometry bonds;
LineGeometry           lines;
LineGeometry           axeslines;
LineGeometry           centeraxeslines;
LineGeometry           measurelines;
LineGeometry           unitcell;
TextGeometry           labels;

TextGeometry           measurelabels;
TextGeometry           unitcelllabels;



struct Atoms  : public SmoothSphereGeometry {

};
struct Halos  : public SmoothSphereGeometry {

};
struct Pickhalos  : public SmoothSphereGeometry {

};
struct Bonds  : public SmoothCylinderGeometry {

};
struct Lines  : public LineGeometry{};
struct Axeslines  : public LineGeometry{};
struct Centeraxeslines  : public LineGeometry{};
struct Measurelines : public LineGeometry{};
struct Unitcell : public LineGeometry{};
struct Labels : public TextGeometry{};

struct Measurelabels : public TextGeometry{};
struct Unitcelllabels : public TextGeometry{};


*/






#endif // GEOMETRIES_H

