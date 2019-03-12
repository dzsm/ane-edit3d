/******************************************************************************
 *
 * This source is part of ANEditor application.
 * Some part of it based on the Avogadro project.
 *
*******************************************************************************/
/******************************************************************************

  This source file is part of the Avogadro project.

  Copyright 2012 Kitware, Inc.

  This source code is released under the New BSD License, (the "License").

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

******************************************************************************/


#include "geometries.h"



SmoothSphereGeometry::SmoothSphereGeometry() : bufferIndex(QOpenGLBuffer::IndexBuffer),
  bufferColor(QOpenGLBuffer::VertexBuffer),
  bufferTexCoord(QOpenGLBuffer::VertexBuffer),
  bufferVertex(QOpenGLBuffer::VertexBuffer),
  bufferNormal(QOpenGLBuffer::VertexBuffer),
  bufferCenter(QOpenGLBuffer::VertexBuffer),data(0),isRenderRequired(false)
{

  data = &selfData;
  dataChanged = false;

}
SmoothSphereGeometry::~SmoothSphereGeometry() {
}



void SmoothSphereGeometry::initialize(QString vglslSource,QString fglslSource) {

  program.addShaderFromSourceFile(QOpenGLShader::Vertex,vglslSource);
  program.addShaderFromSourceFile(QOpenGLShader::Fragment,fglslSource);
  program.link();
  program.bind();

  vao.create();
  vao.bind();

  bufferIndex.create();
  bufferIndex.bind();
  bufferIndex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  //bufferIndex.allocate(dataIndex.constData(),dataIndex.size()*sizeof(GLuint));
  bufferIndex.allocate(0*sizeof(GLuint));

  bufferColor.create();
  bufferColor.bind();
  bufferColor.setUsagePattern(QOpenGLBuffer::StaticDraw);
  //bufferColor.allocate(dataColor.constData(),dataColor.size()*sizeof(QVector4D));
  bufferColor.allocate(0*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.create();
  bufferVertex.bind();
  bufferVertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  //bufferVertex.allocate(dataVertex.constData(),dataVertex.size()*sizeof(QVector3D));
  bufferVertex.allocate(0*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  bufferTexCoord.create();
  bufferTexCoord.bind();
  bufferTexCoord.setUsagePattern(QOpenGLBuffer::StaticDraw);
  //bufferTexCoord.allocate(dataTexCoord.constData(),dataTexCoord.size()*sizeof(QVector2D));
  bufferTexCoord.allocate(0*sizeof(QVector2D));
  program.enableAttributeArray("texCoordinate");
  program.setAttributeBuffer("texCoordinate",GL_FLOAT,0,2);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("color");
  program.disableAttributeArray("texCoordinate");

  bufferIndex.release();
  bufferVertex.release();
  bufferTexCoord.release();
  bufferColor.release();

  program.release();

}

void SmoothSphereGeometry::uploadData() {

  if (dataChanged == false) return;

  dataChanged = false;

  const QVector<QVector4D>         &dataColor = data->dataColor;
  const QVector<QVector3D>         &dataVertex = data->dataVertex;
  const QVector<QVector2D>         &dataTexCoord = data->dataTexCoord;
  const QVector<GLuint>            &dataIndex = data->dataIndex;

  program.bind();
  vao.bind();

  bufferIndex.bind();
  bufferIndex.allocate(dataIndex.constData(),dataIndex.size()*sizeof(GLuint));

  bufferColor.bind();
  bufferColor.allocate(dataColor.constData(),dataColor.size()*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.bind();
  bufferVertex.allocate(dataVertex.constData(),dataVertex.size()*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  bufferTexCoord.bind();
  bufferTexCoord.allocate(dataTexCoord.constData(),dataTexCoord.size()*sizeof(QVector2D));
  program.enableAttributeArray("texCoordinate");
  program.setAttributeBuffer("texCoordinate",GL_FLOAT,0,2);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("color");
  program.disableAttributeArray("texCoordinate");

  bufferIndex.release();
  bufferVertex.release();
  bufferTexCoord.release();
  bufferColor.release();

  program.release();

}

void SmoothSphereGeometry::updateData(SphereData * sphereData) {
  data = sphereData;
  dataChanged = true;
}
void SmoothSphereGeometry::updateData() {
  dataChanged = true;
}

void SmoothSphereGeometry::render(Camera & camera) {

  if(isRenderRequired == false) return;

  uploadData();

  program.bind();

  camera.setProjection();

  program.setUniformValue("modelView",camera.vMatrix);
  program.setUniformValue("projection",camera.pMatrix);

  vao.bind();
  glDrawElements(GL_TRIANGLES,data->dataIndex.size(),GL_UNSIGNED_INT,0);
  vao.release();
  program.release();
}

std::multimap<float, int>
SmoothSphereGeometry::hits(const QVector3D &rayOrigin,
                           const QVector3D &rayEnd,
                           const QVector3D &rayDirection) const
{
  std::multimap<float, int> result;

  // Check for intersection.
  for(int i = 0; i < data->positions.size();i++) {
      float r = data->radius[i];

      QVector3D distance = data->positions[i] - rayOrigin;
      float B = QVector3D::dotProduct(distance,rayDirection);
      float C = QVector3D::dotProduct(distance,distance) - (r*r);
      float D = B * B - C;

      // Test for intersection
      if (D < 0)
        continue;

      // Test for clipping
      if (B < 0 || QVector3D::dotProduct((data->positions[i] - rayEnd),rayDirection) > 0)
        continue;


      float rootD = static_cast<float>(sqrt(D));
      float depth = std::min(std::abs(B + rootD), std::abs(B - rootD));
      result.insert(std::pair<float, int>(depth, i));

    }
  return result;
}








std::multimap<float, int> SmoothSphereGeometry::hits(QPoint point,Camera & camera) const
{
  int x = point.x();
  int y = point.y();
  // Our ray:
  const QVector3D origin(camera.unProject(QVector3D(static_cast<float>(x),
                                                    static_cast<float>(y),
                                                    0.f)));
  const QVector3D end(camera.unProject(QVector3D(static_cast<float>(x),
                                                 static_cast<float>(y), 1.f)));
  const QVector3D direction((end - origin).normalized());

  return hits( origin, end, direction);
}


SmoothCylinderGeometry::SmoothCylinderGeometry() : bufferIndex(QOpenGLBuffer::IndexBuffer),
  bufferColor(QOpenGLBuffer::VertexBuffer),
  bufferTexCoord(QOpenGLBuffer::VertexBuffer),
  bufferVertex(QOpenGLBuffer::VertexBuffer),
  bufferNormal(QOpenGLBuffer::VertexBuffer),
  bufferCenter(QOpenGLBuffer::VertexBuffer),data(0),isRenderRequired(false)
{
  data = &selfData;
  dataChanged = false;
}

SmoothCylinderGeometry::~SmoothCylinderGeometry() {
}



void SmoothCylinderGeometry::initialize() {

  QOpenGLShader sphere_vs(QOpenGLShader::Vertex);
  sphere_vs.compileSourceFile(":/shaders/cylinders_vs.glsl");
  QOpenGLShader sphere_fs(QOpenGLShader::Fragment);
  sphere_fs.compileSourceFile(":/shaders/cylinders_fs.glsl");

  program.addShader(&sphere_fs);
  program.addShader(&sphere_vs);
  program.link();
  program.bind();

  vao.create();
  vao.bind();

  bufferIndex.create();
  bufferIndex.bind();
  bufferIndex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferIndex.allocate(0*sizeof(GLuint));
  //bufferIndex.allocate(dataIndex.constData(),dataIndex.size()*sizeof(GLuint));

  bufferColor.create();
  bufferColor.bind();
  bufferColor.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferColor.allocate(0*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.create();
  bufferVertex.bind();
  bufferVertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferVertex.allocate(0*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  bufferNormal.create();
  bufferNormal.bind();
  bufferNormal.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferNormal.allocate(0*sizeof(QVector3D));
  program.enableAttributeArray("normal");
  program.setAttributeBuffer("normal",GL_FLOAT,0,3);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("normal");
  program.disableAttributeArray("color");

  bufferIndex.release();
  bufferVertex.release();
  bufferColor.release();
  bufferNormal.release();

  program.release();

}



void SmoothCylinderGeometry::uploadData() {

  if (dataChanged == false) return;

  dataChanged = false;

  const QVector<QVector4D>         &dataColor = data->dataColor;
  const QVector<QVector3D>         &dataVertex = data->dataVertex;
  const QVector<QVector3D>         &dataNormal = data->dataNormal;
  const QVector<GLuint>            &dataIndex = data->dataIndex;

  program.bind();
  vao.bind();

  bufferIndex.bind();
  bufferIndex.allocate(dataIndex.constData(),dataIndex.size()*sizeof(GLuint));

  bufferColor.bind();
  bufferColor.allocate(dataColor.constData(),dataColor.size()*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.bind();
  bufferVertex.allocate(dataVertex.constData(),dataVertex.size()*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  bufferNormal.bind();
  bufferNormal.allocate(dataNormal.constData(),dataNormal.size()*sizeof(QVector3D));
  program.enableAttributeArray("normal");
  program.setAttributeBuffer("normal",GL_FLOAT,0,3);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("normal");
  program.disableAttributeArray("color");

  bufferIndex.release();
  bufferVertex.release();
  bufferNormal.release();
  bufferColor.release();

  program.release();

}

void SmoothCylinderGeometry::updateData(CylinderData * CylinderData) {
  data = CylinderData;
  dataChanged = true;
}
void SmoothCylinderGeometry::updateData() {
  dataChanged = true;
}

void SmoothCylinderGeometry::render(Camera & camera) {

  if(isRenderRequired == false) return;

  uploadData();

  program.bind();

  camera.setProjection();

  //program.setUniformValue("modelView",camera.vMatrix);
  //program.setUniformValue("projection",camera.pMatrix);

  program.setUniformValue("projectionXmodelView",camera.pMatrix*camera.vMatrix);
  program.setUniformValue("normalMatrix",camera.vMatrix.normalMatrix());

  vao.bind();
  glDrawElements(GL_QUADS,data->dataIndex.size(),GL_UNSIGNED_INT,0);
  vao.release();

  program.release();
}

std::multimap<float, int>
SmoothCylinderGeometry::hits(const QVector3D &rayOrigin,
                             const QVector3D &rayEnd,
                             const QVector3D &rayDirection) const
{

  std::multimap<float, int> result;
  throw;
  return result;
}


std::multimap<float, int> SmoothCylinderGeometry::hits(QPoint point,Camera & camera) const
{
  int x = point.x();
  int y = point.y();
  // Our ray:
  const QVector3D origin(camera.unProject(QVector3D(static_cast<float>(x),
                                                    static_cast<float>(y),
                                                    0.f)));
  const QVector3D end(camera.unProject(QVector3D(static_cast<float>(x),
                                                 static_cast<float>(y), 1.f)));
  const QVector3D direction((end - origin).normalized());

  return hits( origin, end, direction);
}







LineGeometry::LineGeometry() :   bufferColor(QOpenGLBuffer::VertexBuffer),
  bufferIndex(QOpenGLBuffer::IndexBuffer),
  bufferVertex(QOpenGLBuffer::VertexBuffer),isRenderRequired(false)
{
  data = &selfData;
  dataChanged = true;
}

LineGeometry::~LineGeometry() {
}


void LineGeometry::initialize() {

  QOpenGLShader sphere_vs(QOpenGLShader::Vertex);
  sphere_vs.compileSourceFile(":/shaders/linestrip_vs.glsl");
  QOpenGLShader sphere_fs(QOpenGLShader::Fragment);
  sphere_fs.compileSourceFile(":/shaders/linestrip_fs.glsl");

  program.addShader(&sphere_fs);
  program.addShader(&sphere_vs);
  program.link();
  program.bind();

  vao.create();
  vao.bind();

  bufferIndex.create();
  bufferIndex.bind();
  bufferIndex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferIndex.allocate(0*sizeof(GLuint));

  bufferColor.create();
  bufferColor.bind();
  bufferColor.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferColor.allocate(0*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.create();
  bufferVertex.bind();
  bufferVertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferVertex.allocate(0*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("color");

  bufferIndex.release();
  bufferVertex.release();
  bufferColor.release();

  program.release();

}



void LineGeometry::uploadData() {

  if (dataChanged == false) return;

  dataChanged = false;

  const QVector<QVector4D>         &dataColor = data->dataColor;
  const QVector<QVector3D>         &dataVertex = data->dataVertex;
  const QVector<GLuint>            &dataIndex = data->dataIndex;

  program.bind();
  vao.bind();

  bufferIndex.bind();
  bufferIndex.allocate(dataIndex.constData(),dataIndex.size()*sizeof(GLuint));

  bufferColor.bind();
  bufferColor.allocate(dataColor.constData(),dataColor.size()*sizeof(QVector4D));
  program.enableAttributeArray("color");
  program.setAttributeBuffer("color",GL_FLOAT,0,4);

  bufferVertex.bind();
  bufferVertex.allocate(dataVertex.constData(),dataVertex.size()*sizeof(QVector3D));
  program.enableAttributeArray("vertex");
  program.setAttributeBuffer("vertex",GL_FLOAT,0,3);

  vao.release();

  program.disableAttributeArray("vertex");
  program.disableAttributeArray("color");

  bufferIndex.release();
  bufferVertex.release();
  bufferColor.release();

  program.release();

}

void LineGeometry::updateData(LineData * lineData) {
  data = lineData;
  dataChanged = true;
}
void LineGeometry::updateData() {
  dataChanged = true;
}

void LineGeometry::render(Camera & camera) {

  if(isRenderRequired == false) return;

  uploadData();

  program.bind();

  camera.setProjection();


  program.setUniformValue("projectionXmodelView",camera.pMatrix*camera.vMatrix);

  vao.bind();
  glDrawElements(GL_LINES,data->dataIndex.size(),GL_UNSIGNED_INT,0);
  vao.release();

  program.release();

}


CharTextureAtlas::CharTextureAtlas(const QFont charFontX,const QColor color,const QString charTable) :
  charTable(charTable),
  charFont(charFontX),
  charMetrics(charFont) {



    //charFont.setBold(true);
    charFont.setStyleStrategy(QFont::StyleStrategy::NoAntialias);

    //charFont.setStyleStrategy(QFont::PreferDevice);

    charFont.setStyleStrategy(QFont::OpenGLCompatible	);
    charFont.setStyleStrategy(QFont::ForceIntegerMetrics);

    //charFont.setStyleStrategy(QFont::ForceOutline);
    //charFont.setStyleStrategy(QFont::NoSubpixelAntialias);

  pixelsWide = charMetrics.width(charTable);
  pixelsHigh = charMetrics.height();

  charImage = QImage(pixelsWide,pixelsHigh,QImage::Format_ARGB32);
  charImage.fill(Qt::transparent);

  int count =  0;
  for(int i = 0; i < charTable.size(); i++)
    {
      int countNext = count+charMetrics.width(charTable[i]);
      charPositionMap[charTable[i]].setCoords(count,0,countNext,charMetrics.height());
      charNormalizedPositionMap[charTable[i]].setCoords(float(count)/pixelsWide,0.0f,float(countNext)/pixelsWide,1.0f);
      count = countNext;
    }


  QPainter painter;
  painter.begin(&charImage);
  painter.setPen(color);
  painter.setFont(charFont);


  //painter.setRenderHint(QPainter::TextAntialiasing, true);
  painter.drawText(QRect(0,0,pixelsWide,pixelsHigh), Qt::TextWrapAnywhere, charTable);
  painter.end();

  charImage = charImage.mirrored();
}



QImage CharTextureAtlas::getAtlasImage() {
  return charImage;
}

QImage CharTextureAtlas::getCharImage(QChar ch) {
  return charImage.copy(charPositionMap[ch]);
}

int CharTextureAtlas::addVerexTexCoords(QVector<QVector4D> &texturePoints,const QString & text, float scalew = 1.0f,float scaleh = 1.0f) {

  float x = 0.0;
  int count = 0;
  for(int i = 0; i < text.length();i++) {
      const QRectF & rect  = charNormalizedPositionMap[text[i]];
      const QRect  & recti = charPositionMap[text[i]];
      float w = recti.width();
      float h = recti.height();
      //texturePoints << QVector4D((x+w)*scalew,0.0f,rect.bottomRight().x(),rect.bottomRight().y());
      //texturePoints << QVector4D((x+w)*scalew,-h*scaleh,rect.topRight().x(),rect.topRight().y());
      //texturePoints << QVector4D(x*scalew,-h*scaleh,rect.topLeft().x(),rect.topLeft().y());
      //texturePoints << QVector4D(x*scalew,0.0f,rect.bottomLeft().x(),rect.bottomLeft().y());

      texturePoints << QVector4D((x+w),0.0f,rect.bottomRight().x(),rect.bottomRight().y());
      texturePoints << QVector4D((x+w),-h,rect.topRight().x(),rect.topRight().y());
      texturePoints << QVector4D(x,-h,rect.topLeft().x(),rect.topLeft().y());
      texturePoints << QVector4D(x,0.0f,rect.bottomLeft().x(),rect.bottomLeft().y());
      x+=w;
      count += 4;
    }
  return count;
}


TextData::TextData() : charTextureAtlas(QFont("Courier", 16), Qt::black," !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
{

}

void TextData::clear() {

  positions.clear();
  texts.clear();

  dataPosition.clear();
  dataVertexTexCoord.clear();
}


TextGeometry::TextGeometry() :  bufferPosition(QOpenGLBuffer::VertexBuffer),
  bufferVertexTexCoord(QOpenGLBuffer::VertexBuffer),texture(QOpenGLTexture::Target2D),isRenderRequired(false)
{
  data = &selfData;
  dataChanged = true;
}

TextGeometry::~TextGeometry() {
}


void TextGeometry::initialize() {

  QOpenGLShader sphere_vs(QOpenGLShader::Vertex);
  sphere_vs.compileSourceFile(":/shaders/vshader.glsl");
  QOpenGLShader sphere_fs(QOpenGLShader::Fragment);
  sphere_fs.compileSourceFile(":/shaders/fshader.glsl");

  program.addShader(&sphere_fs);
  program.addShader(&sphere_vs);
  program.link();
  program.bind();

  vao.create();
  vao.bind();

  bufferVertexTexCoord.create();
  bufferVertexTexCoord.bind();
  bufferVertexTexCoord.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferVertexTexCoord.allocate(data->dataVertexTexCoord.constData(),data->dataVertexTexCoord.size()*sizeof(QVector4D));
  program.enableAttributeArray("vertextex");
  program.setAttributeBuffer("vertextex",GL_FLOAT,0,4);

  bufferPosition.create();
  bufferPosition.bind();
  bufferPosition.setUsagePattern(QOpenGLBuffer::StaticDraw);
  bufferPosition.allocate(data->dataPosition.constData(),data->dataPosition.size()*sizeof(QVector3D));
  program.enableAttributeArray("position");
  program.setAttributeBuffer("position",GL_FLOAT,0,3);

  vao.release();

  program.disableAttributeArray("vertextex");
  program.disableAttributeArray("position");

  bufferVertexTexCoord.release();
  bufferPosition.release();

  texture.create();
  texture.setData(data->charTextureAtlas.getAtlasImage());
  texture.setMinificationFilter(QOpenGLTexture::NearestMipMapNearest);
  texture.setMagnificationFilter(QOpenGLTexture::Linear);
  texture.setWrapMode(QOpenGLTexture::Repeat);

  program.release();

}



void TextGeometry::uploadData() {

  if (dataChanged == false) return;

  dataChanged = false;

  program.bind();
  vao.bind();

  bufferVertexTexCoord.bind();
  bufferVertexTexCoord.allocate(data->dataVertexTexCoord.constData(),data->dataVertexTexCoord.size()*sizeof(QVector4D));
  program.enableAttributeArray("vertextex");
  program.setAttributeBuffer("vertextex",GL_FLOAT,0,4);

  bufferPosition.bind();
  bufferPosition.allocate(data->dataPosition.constData(),data->dataPosition.size()*sizeof(QVector3D));
  program.enableAttributeArray("position");
  program.setAttributeBuffer("position",GL_FLOAT,0,3);


  vao.release();

  program.disableAttributeArray("vertextex");
  program.disableAttributeArray("position");

  bufferVertexTexCoord.release();
  bufferPosition.release();

  program.release();

}

void TextGeometry::updateData(TextData * textData) {
  data = textData;
  dataChanged = true;
}
void TextGeometry::updateData() {
  dataChanged = true;
}

void TextGeometry::render(Camera & camera) {

  if(isRenderRequired == false) return;

  uploadData();

  camera.setProjection();

  program.bind();

  texture.bind();

  program.setUniformValue("texture",0);
  //program.setUniformValue("projectionXmodelView",camera.pMatrix*camera.vMatrix);
  program.setUniformValue("modelView",camera.vMatrix);
  program.setUniformValue("projection",camera.pMatrix);
  program.setUniformValue("viewport",QVector2D(camera.m_width,camera.m_height));


  vao.bind();
  glDrawArrays(GL_QUADS,0,data->dataVertexTexCoord.size());
  vao.release();

  texture.release();
  program.release();
}














