#ifndef MOLECULE_H
#define MOLECULE_H

#include <tuple>
#include <vector>
#include <map>

#include <Eigen/Geometry>
#include "element.h"

namespace Molecule {

  // Quick avogadrolib compatible class, later should replace with avogadrolib when sort out compiling..
  class Molecule {
  protected:
    std::string                           m_title;

    std::vector<int>                      m_atoms;
    std::vector<Eigen::Vector3d>          m_positions;
    std::vector<unsigned char>            m_group;

    std::vector<std::tuple<int,int>>      m_links;
    std::vector<char>                     m_orders;

    std::vector<std::tuple<int,int>>      m_distances;
    std::vector<int>                      m_picks;

    bool                                  m_hasCell;
    Eigen::Vector3d                  m_a1,m_a2,m_a3;

  public:

    int size() const;

    bool isEmpty() const;

    std::vector<int>& atoms();
    const std::vector<int>& atoms() const ;

    std::vector<Eigen::Vector3d>& positions();
    const std::vector<Eigen::Vector3d>& positions() const;

    std::vector<unsigned char>& group();
    const std::vector<unsigned char>& group() const;

    std::vector<char>& orders();
    const std::vector<char>& orders() const ;

    std::vector<std::tuple<int,int>>& links();
    const std::vector<std::tuple<int,int>>& links() const;

    std::vector<std::tuple<int,int>>& distances();
    const std::vector<std::tuple<int,int>>& distances() const;

    std::vector<int>& picks();
    const std::vector<int>& picks() const;

    bool isValidIndex(int atomIndex) const;

    void addAtom(int atomicNumber, const Eigen::Vector3d & position, unsigned char groupBits);
    void addBond(int atomIndex1,int atomIndex2,char order);

    void toggleDistance(int atomIndex1,int atomIndex2);
    void togglePicks(int atomIndex);

    void resolveBondDuplicates();
    void reserve(int numberOfAtoms);

    void removeBond(int bondIndex);
    void removeAtom(int atomIndex);

    void clear();

    void clearBonds();
    void clearDistances();
    void clearPicks();

    std::string & title();
    const std::string & title() const ;

    Eigen::Vector3d & a1();
    const Eigen::Vector3d & a1() const;

    Eigen::Vector3d & a2();
    const Eigen::Vector3d & a2() const;

    Eigen::Vector3d & a3() ;
    const Eigen::Vector3d & a3() const;

    void setCell(Eigen::Vector3d a1,Eigen::Vector3d a2,Eigen::Vector3d a3);
    void removeCell();
    bool hasCell() const;

    Eigen::Vector3d getCenter(unsigned char mask) const;
    Eigen::Vector3d getBoundingBox(unsigned char mask,Eigen::Vector3d &cmi,Eigen::Vector3d &cma) const;

    void translate(unsigned char mask,const Eigen::Vector3d & t);
    void repeatCell(int N1,int N2,int N3);

    void rotateTo(unsigned char mask,Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d o );
    void rotateAround(unsigned char mask,Eigen::Vector3d axis ,double angle, Eigen::Vector3d o);

    void alignTo(unsigned char mask,Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d v3,Eigen::Vector3d dir);
    void alignAlong(unsigned char mask,Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d dir );
    void autoAlign(unsigned char mask,char xyz[3]);
    //void autoLink(unsigned char mask,double factor,double maxd,double mind);

    void sort(unsigned char mask,Eigen::Vector3d dir);
    void gather(unsigned char mask);

    void removeAtoms(unsigned char mask);
    void removeBonds(unsigned char mask);

    void append(const Molecule & mol,unsigned char mask, unsigned char flip);
    void extract(unsigned char mask, Molecule & mol);

    void setGroup(int id, unsigned char mask, unsigned char flip);

    bool isGroup(int id, unsigned char mask) const;

    void find(unsigned char mask, std::string sym, unsigned char maskWrite, unsigned char flip);
    void nearby(unsigned char mask, Eigen::Vector3d c, double maxd, double mind, unsigned char maskWrite, unsigned char flip);
    void autoLink(unsigned char mask, double factor,double maxd,double mind);
    void pick(unsigned char mask, int ith) ;
    void neighbours(int id, unsigned char maskWrite, unsigned char flip);
    void neighbourhood(unsigned char mask, unsigned char maskWrite, unsigned char flip);
    int getCount(unsigned char mask) const;


};

  bool loadEDI(Molecule & mol,std::istream & is);
  bool writeEDI(Molecule & mol,std::ostream & os);

  bool loadxyz(Molecule & mol,std::istream & is);
  bool writexyz(Molecule & mol,std::ostream & os);

  bool loadaxyz(Molecule & mol,std::istream & is);
  bool writeaxyz(Molecule & mol,std::ostream & os);

  bool loadxyzpy(Molecule & mol,std::istream & is);
  bool writexyzpy(Molecule & mol,std::ostream & os);


}

#endif // MOLECULE_H
