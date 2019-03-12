#ifndef MOLECULEDATA_H
#define MOLECULEDATA_H

#include <core/molecule.h>
#include <glview/geometries.h>

class MoleculeData : public Molecule::Molecule
{
public:
    MoleculeData();

    void syncOpenGLAtoms(SphereData & openGLAtoms);
    void syncOpenGLHalos(SphereData & openGLHalos);
    void syncOpenGLPicks(SphereData & openGLPicks);
    void syncOpenGLBonds(CylinderData & openGLBonds);
    void syncOpenGLWires(LineData & openGLLines);
    void syncOpenGLMeasureLines(LineData & openGLLines,TextData & openGLTexts);
    void syncOpenGLUnitCell(LineData & openGLLines,TextData & openGLTexts);
    void syncOpenGLCenterAxes(LineData & openGLLines);
    void syncOpenGLLabels(TextData & openGLTexts, int mode = 0);

signals:

public slots:
};

#endif // MOLECULEDATA_H
