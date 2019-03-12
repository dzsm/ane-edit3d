#include "molecule.h"
#include <Eigen/Eigenvalues>
#include "neighbourlist.h"


#include <regex>
#include <iterator>     // std::istream_iterator

namespace Molecule {

int Molecule::size() const {
    return m_atoms.size();
}

bool Molecule::isEmpty() const {
    return m_atoms.size() == 0;
}

std::vector<int>& Molecule::atoms() {
    return m_atoms;
}
const std::vector<int>& Molecule::atoms() const {
    return m_atoms;
}

std::vector<Eigen::Vector3d>& Molecule::positions() {
    return m_positions;
}
const std::vector<Eigen::Vector3d>& Molecule::positions() const {
    return m_positions;
}

std::vector<unsigned char>& Molecule::group() {
    return m_group;
}
const std::vector<unsigned char>& Molecule::group() const {
    return m_group;
}

std::vector<char>& Molecule::orders() {
    return m_orders;
}
const std::vector<char>& Molecule::orders() const {
    return m_orders;
}

std::vector<std::tuple<int,int>>& Molecule::links() {
    return m_links;
}
const std::vector<std::tuple<int,int>>& Molecule::links() const {
    return m_links;
}

std::vector<std::tuple<int,int>>& Molecule::distances() {
    return m_distances;
}
const std::vector<std::tuple<int,int>>& Molecule::distances() const {
    return m_distances;
}

const std::vector<int> & Molecule::picks() const {
    return m_picks;
}
std::vector<int> & Molecule::picks() {
    return m_picks;
}

void Molecule::addAtom(int atomicNumber, const Eigen::Vector3d & position, unsigned char groupBits) {
    if(atomicNumber>=Element::count||atomicNumber<0) return;

    m_atoms.push_back(atomicNumber);
    m_positions.push_back(position);
    m_group.push_back(groupBits);
}
void Molecule::addBond(int atomIndex1,int atomIndex2,char order) {
    if(!(isValidIndex(atomIndex1)&&isValidIndex(atomIndex2))) return;

    if(atomIndex1>atomIndex2) std::swap(atomIndex1,atomIndex2);
    m_links.push_back(std::make_tuple(atomIndex1,atomIndex2));
    m_orders.push_back(order);
}

void Molecule::toggleDistance(int atomIndex1,int atomIndex2) {
    if(atomIndex1>atomIndex2) std::swap(atomIndex1,atomIndex2);
    for(int i = 0; i < static_cast<int>(m_distances.size());i++) if (m_distances[i]==std::make_tuple(atomIndex1,atomIndex2)) {
        m_distances.erase(m_distances.cbegin()+i);
        return;
    }
    if(isValidIndex(atomIndex1)&&isValidIndex(atomIndex2))
    m_distances.push_back(std::make_tuple(atomIndex1,atomIndex2));
}

void Molecule::clearDistances() {
    m_distances.clear();
}

bool Molecule::isValidIndex(int atomIndex) const {
    return atomIndex<static_cast<int>(m_atoms.size()) && atomIndex >= 0;
}

void Molecule::togglePicks(int atomIndex) {
    for(int i = 0; i < static_cast<int>(m_picks.size());i++) if (m_picks[i]==atomIndex) {
        m_picks.erase(m_picks.cbegin()+i);
        return;
    }

    if(isValidIndex(atomIndex)) {
        m_picks.push_back(atomIndex);
        if(m_picks.size()>4) m_picks.erase(m_picks.cbegin());
    }
}

void Molecule::clearPicks() {
    m_picks.clear();
}


void Molecule::resolveBondDuplicates() {

    std::map<std::tuple<int,int>,char> o;
    for(int i = 0; i < static_cast<int>(m_links.size());i++) o[m_links[i]] = m_orders[i];
    m_links.clear();
    m_orders.clear();
    for(std::map<std::tuple<int,int>,char>::const_iterator it = o.cbegin(); it != o.cend(); ++it) {
        m_links.push_back(it->first);
        m_orders.push_back(it->second);
    }
}
void Molecule::clear() {
    m_atoms.clear();
    m_positions.clear();
    m_group.clear();

    m_links.clear();
    m_orders.clear();

    m_distances.clear();
    m_picks.clear();

    m_hasCell = false;
    m_a1.setZero();
    m_a2.setZero();
    m_a3.setZero();
}
void Molecule::clearBonds() {
    m_links.clear();
    m_orders.clear();
}

void Molecule::setCell(Eigen::Vector3d a1,Eigen::Vector3d a2,Eigen::Vector3d a3) {
    m_hasCell = true;
    m_a1 = a1;
    m_a2 = a2;
    m_a3 = a3;
}

void Molecule::removeCell() {
    m_hasCell = false;
    m_a1.setZero();
    m_a2.setZero();
    m_a3.setZero();
}

std::string & Molecule::title() {
    return m_title;
}
const std::string & Molecule::title() const {
    return m_title;
}

Eigen::Vector3d & Molecule::a1() {
    return m_a1;
}
const Eigen::Vector3d & Molecule::a1() const {
    return m_a1;
}

Eigen::Vector3d & Molecule::a2() {
    return m_a2;
}
const Eigen::Vector3d & Molecule::a2() const {
    return m_a2;
}

Eigen::Vector3d & Molecule::a3() {
    return m_a3;
}
const Eigen::Vector3d & Molecule::a3() const {
    return m_a3;
}

bool Molecule::hasCell() const {
    return m_hasCell;
}

Eigen::Vector3d Molecule::getCenter(unsigned char mask) const {
    Eigen::Vector3d center;
    center.setZero();
    int count = 0;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask))
    {
        center += m_positions[i];
        count++;
    }
    if(count>0) center /= double(count);
    return center;
}
Eigen::Vector3d Molecule::getBoundingBox(unsigned char mask,Eigen::Vector3d &cmi,Eigen::Vector3d &cma) const {
    cmi.setZero();
    cma.setZero();
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        cmi = m_positions[i];
        cma = m_positions[i];
        break;
    }

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        cmi.x() = m_positions[i].x() < cmi.x() ? m_positions[i].x() : cmi.x();
        cmi.y() = m_positions[i].y() < cmi.y() ? m_positions[i].y() : cmi.y();
        cmi.z() = m_positions[i].z() < cmi.z() ? m_positions[i].z() : cmi.z();
        cma.x() = m_positions[i].x() > cma.x() ? m_positions[i].x() : cma.x();
        cma.y() = m_positions[i].y() > cma.y() ? m_positions[i].y() : cma.y();
        cma.z() = m_positions[i].z() > cma.z() ? m_positions[i].z() : cma.z();
    }

    if((cma-cmi).norm()<0.01) { cma << 5,5,5; }
    return cma-cmi;
}

void Molecule::reserve(int numberOfAtoms) {
    m_atoms.reserve(numberOfAtoms);
    m_positions.reserve(numberOfAtoms);
    m_group.reserve(numberOfAtoms);
}

void Molecule::translate(unsigned char mask, const Eigen::Vector3d & t) {
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask))  {
        m_positions[i] += t;
    }

}
void Molecule::repeatCell(int N1,int N2,int N3) {

    if(m_hasCell==false) return;
    int nn = m_positions.size();

    if(N1<=0 || N2<=0 || N3<=0) return;

    m_positions.reserve(nn*N1*N2*N3);
    m_atoms.reserve(nn*N1*N2*N3);
    m_group.reserve(nn*N1*N2*N3);
    for(int n1 = 0; n1 < N1; n1++)
        for(int n2 = 0; n2 < N2; n2++)
            for(int n3 = 0; n3 < N3; n3++) if (n1+n2+n3>0) {
                Eigen::Vector3d t = n1*m_a1 + n2*m_a2 + n3*m_a3;
                for(int i = 0; i < nn; i++) {
                    m_positions.push_back(m_positions[i]+t);
                    m_atoms.push_back(m_atoms[i]);
                    m_group.push_back(0);
                }

            }

    m_a1 = N1*m_a1;
    m_a2 = N2*m_a2;
    m_a3 = N3*m_a3;
}
void Molecule::rotateTo(unsigned char mask, Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d o) {
    v1.normalize();
    v2.normalize();

    Eigen::Vector3d axis = v1.cross(v2);

    double cosangle = v1.dot(v2);
    double sinangle = v1.cross(v2).norm();

    //sina = |Va x Vb| / ( |Va| * |Vb| )
    //cosa = (Va . Vb) / ( |Va| * |Vb| )

    double angle = std::atan2( sinangle, cosangle );

    /*double sign = axis.dot( v1.cross(v2) );
    if(sign<0.0) angle=-angle;


    double cosangle = v1.dot(v2);
    double angle = std::acos(v1.dot(v2)); //std::cerr<< "a:" << angle << std::endl;
    if (cosangle<0) angle = 3.1415-angle;
    */

    //Eigen::Vector3d axis = v1.cross(v2); //std::cerr<< axis << std::endl;

    if(axis.norm() < 1.0e-8) return;
    axis.normalize();
    Eigen::Affine3d t;
    t = Eigen::Translation3d(o)*Eigen::AngleAxisd(angle,axis)*Eigen::Translation3d(-o);
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        m_positions[i] = t * m_positions[i];
    }

}
void Molecule::rotateAround(unsigned char mask, Eigen::Vector3d axis ,double angle, Eigen::Vector3d o) {
    axis.normalize();
    Eigen::Affine3d t;
    t = Eigen::Translation3d(o)*Eigen::AngleAxisd(angle,axis)*Eigen::Translation3d(-o);
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        m_positions[i] = t * m_positions[i];
    }

}
void Molecule::alignTo(unsigned char mask, Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d v3,Eigen::Vector3d dir){
    dir.normalize();
    Eigen::Vector3d wdir = ((v1-v2).cross(v1-v3)).normalized();
    Eigen::Vector3d o = v1;
    double angle = std::acos(dir.dot(wdir)); //std::cerr<< "a:" << angle << std::endl;
    Eigen::Vector3d axis = dir.cross(wdir); //std::cerr<< axis << std::endl;
    if(axis.norm() < 1.0e-8) return;
    axis.normalize();
    Eigen::Affine3d t;
    t = Eigen::Translation3d(o)*Eigen::AngleAxisd(-angle,axis)*Eigen::Translation3d(-o);
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        m_positions[i] = t * m_positions[i];
    }

}
void Molecule::alignAlong(unsigned char mask, Eigen::Vector3d v1 ,Eigen::Vector3d v2,Eigen::Vector3d dir){
    dir.normalize();
    Eigen::Vector3d wdir = (v2-v1).normalized();  //std::cerr << wdir << std::endl;
    Eigen::Vector3d o = v1;
    double angle = std::acos(dir.dot(wdir)); //std::cerr<< "a:" << angle << std::endl;
    Eigen::Vector3d axis = dir.cross(wdir); //std::cerr<< axis << std::endl;
    if(axis.norm() < 1.0e-8) return ;
    axis.normalize();
    Eigen::Affine3d t;
    t = Eigen::Translation3d(o)*Eigen::AngleAxisd(-angle,axis)*Eigen::Translation3d(-o);
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        m_positions[i] = t * m_positions[i];
    }

}
void Molecule::autoAlign(unsigned char mask, char xyz[3]) {

    // inertia tensor
    Eigen::Matrix3d M;
    M.setZero();
    Eigen::Vector3d cent;
    cent.setZero();
    int nn = 0;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        nn++;
        cent += m_positions[i];
    }
    if(nn>0) cent /= double(nn);

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        double x = m_positions[i].x()-cent.x();
        double y = m_positions[i].y()-cent.y();
        double z = m_positions[i].z()-cent.z();

        M(0,0)+=z*z+y*y;
        M(1,1)+=x*x+z*z;
        M(2,2)+=x*x+y*y;

        M(0,1)+=-x*y;
        M(1,2)+=-y*z;
        M(0,2)+=-x*z;


    }
    M(1,0)=M(0,1); M(2,1)=M(1,2); M(2,0)=M(0,2);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> ei(M);

    //		Eigen::Matrix3d V = ei.eigenvectors();
    //		Eigen::Vector3d ax1(V.col(0)(0);
    Eigen::Vector3d ax1 = ei.eigenvectors().col(0);
    Eigen::Vector3d ax2 = ei.eigenvectors().col(1);
    Eigen::Vector3d ax3 = ei.eigenvectors().col(2);

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        Eigen::Vector3d here =  m_positions[i]-cent;

        if(xyz[0]=='x')  m_positions[i].x() = ax1.dot(here);
        else  if(xyz[0]=='y')  m_positions[i].y() = ax1.dot(here);
        else  if(xyz[0]=='z')  m_positions[i].z() = ax1.dot(here);

        if(xyz[1]=='x')  m_positions[i].x() = ax2.dot(here);
        else  if(xyz[1]=='y')  m_positions[i].y() = ax2.dot(here);
        else  if(xyz[1]=='z')  m_positions[i].z() = ax2.dot(here);

        if(xyz[2]=='x')  m_positions[i].x() = ax3.dot(here);
        else  if(xyz[2]=='y')  m_positions[i].y() = ax3.dot(here);
        else  if(xyz[2]=='z')  m_positions[i].z() = ax3.dot(here);

        m_positions[i]+=cent;
    }
}

void Molecule::sort(unsigned char mask, Eigen::Vector3d dir) {
    dir.normalize();

    std::vector<int> orderindex,index;
    orderindex.reserve(m_positions.size());
    index.reserve(m_positions.size());

    std::vector<int> ordering;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) ordering.push_back(i);

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        orderindex.push_back(i);
    }

    auto a = m_atoms;
    auto p = m_positions;
    auto g = m_group;

    // sort indexes based on comparing values in v
    std::stable_sort(orderindex.begin(), orderindex.end(),[&p,&dir](size_t i1, size_t i2) {return dir.dot(p[i1]-p[i2]) < 0.0;});



    int count = 0;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        ordering[i] = orderindex[count++];
    }

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        m_atoms[i] = a[ordering[i]];
        m_positions[i] = p[ordering[i]];
        m_group[i] = g[ordering[i]];
    }


    for(int i = 0; i < static_cast<int>(m_links.size()); i++) {
        std::get<0>(m_links[i]) = ordering[std::get<0>(m_links[i])];
        std::get<1>(m_links[i]) = ordering[std::get<1>(m_links[i])];

    }

    for(int i = 0; i < static_cast<int>(m_picks.size()); i++) {
        m_picks[i] = ordering[m_picks[i]];
    }

    for(int i = 0; i < static_cast<int>(m_distances.size()); i++) {
        std::get<0>(m_distances[i]) = ordering[std::get<0>(m_distances[i])];
        std::get<1>(m_distances[i]) = ordering[std::get<1>(m_distances[i])];
    }


}
void Molecule::gather(unsigned char mask) {

    /*std::vector<int> ordering;
    for(int i = 0; i < m_positions.size(); i++) ordering.push_back(i);
    std::vector<int> groupflag;
    for(int i = 0; i < m_positions.size(); i++) groupflag.push_back(mask.check(m_group[i])?0:1);

    // sort indexes based on comparing values in v
    std::stable_sort(ordering.begin(), ordering.end(),[&groupflag](size_t i1, size_t i2) {return groupflag[i1] < groupflag[i2];});
    */

    std::vector<int> ordering(m_group.size());
    int count = 0;
    for(int i = 0; i < static_cast<int>(m_group.size()); i++) if(!isGroup(i,mask)) ordering[i] = count++;
    for(int i = 0; i < static_cast<int>(m_group.size()); i++) if(isGroup(i,mask)) ordering[i] = count++;

    auto a = m_atoms;
    auto p = m_positions;
    auto g = m_group;

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) {
        m_atoms[ordering[i]] = a[i];
        m_positions[ordering[i]] = p[i];
        m_group[ordering[i]] = g[i];
    }


    for(int i = 0; i < static_cast<int>(m_links.size()); i++) {
        std::get<0>(m_links[i]) = ordering[std::get<0>(m_links[i])];
        std::get<1>(m_links[i]) = ordering[std::get<1>(m_links[i])];

    }

    for(int i = 0; i < static_cast<int>(m_picks.size()); i++) {
        m_picks[i] = ordering[m_picks[i]];
    }


    for(int i = 0; i < static_cast<int>(m_distances.size()); i++) {
        std::get<0>(m_distances[i]) = ordering[std::get<0>(m_distances[i])];
        std::get<1>(m_distances[i]) = ordering[std::get<1>(m_distances[i])];
    }

}

void Molecule::removeBonds(unsigned char mask) {
    for (int i = 0; i < static_cast<int>(m_links.size()); ++i)
        if(isGroup(std::get<0>(m_links[i]),mask) || isGroup(std::get<1>(m_links[i]),mask) ) {
            std::swap(m_links[i], m_links.back());  m_links.pop_back();
            std::swap(m_orders[i],m_orders.back()); m_orders.pop_back();
            i--;
        }
}

void Molecule::removeAtoms(unsigned char mask) {

    //std::cout << mask;
    for (int i = 0; i < static_cast<int>(m_links.size()); ++i)
        if(isGroup(std::get<0>(m_links[i]),mask) || isGroup(std::get<1>(m_links[i]),mask) ) {
            std::swap(m_links[i], m_links.back());  m_links.pop_back();
            std::swap(m_orders[i],m_orders.back()); m_orders.pop_back();
            i--;
        }

    for (int i = 0; i < static_cast<int>(m_distances.size()); ++i)
        if(isGroup(std::get<0>(m_distances[i]),mask) || isGroup(std::get<1>(m_distances[i]),mask) ) {
            std::swap(m_distances[i], m_distances.back());  m_distances.pop_back();
            i--;
        }

    std::vector<int> tmp_picks;
    for (int i = 0; i < static_cast<int>(m_picks.size()); ++i) if (!isGroup(m_picks[i],mask)) tmp_picks.push_back(m_picks[i]);
    m_picks = tmp_picks;


    std::vector<int> ordering(m_group.size());
    int count = 0;
    for(int i = 0; i < static_cast<int>(m_group.size()); i++) if(!isGroup(i,mask)) ordering[i] = count++;
    int countSize = count;
    for(int i = 0; i < static_cast<int>(m_group.size()); i++) if(isGroup(i,mask)) ordering[i] = count++;

    std::vector<int> a = m_atoms;
    std::vector<Eigen::Vector3d> p = m_positions;
    std::vector<unsigned char> g = m_group;

    for(int i = 0; i < static_cast<int>(m_group.size()); i++) {
        m_atoms[ordering[i]] = a[i];
        m_positions[ordering[i]] = p[i];
        m_group[ordering[i]] = g[i];
    }

    m_atoms.resize(countSize);
    m_positions.resize(countSize);
    m_group.resize(countSize);

    for(int i = 0; i < static_cast<int>(m_links.size()); i++) {
        std::get<0>(m_links[i]) = ordering[std::get<0>(m_links[i])];
        std::get<1>(m_links[i]) = ordering[std::get<1>(m_links[i])];
    }

    for(int i = 0; i < static_cast<int>(m_picks.size()); i++) {
        m_picks[i] = ordering[m_picks[i]];
    }

    for(int i = 0; i < static_cast<int>(m_distances.size()); i++) {
        std::get<0>(m_distances[i]) = ordering[std::get<0>(m_distances[i])];
        std::get<1>(m_distances[i]) = ordering[std::get<1>(m_distances[i])];
    }
}

void Molecule::removeBond(int bondIndex) {
    if(bondIndex<0 || bondIndex>= m_links.size()) return;

    int lastBondIndex = m_links.size() - 1;
    if (bondIndex != lastBondIndex) {
        m_links[bondIndex] = m_links.back();
        m_orders[bondIndex] = m_orders.back();
    }
    m_links.pop_back();
    m_orders.pop_back();
}
void Molecule::removeAtom(int atomIndex) {
    for (int i = 0; i < static_cast<int>(m_links.size()); ++i)
        if (std::get<0>(m_links[i]) == atomIndex || std::get<1>(m_links[i]) == atomIndex) {
            std::swap(m_links[i], m_links.back());  m_links.pop_back(); // instead of swap only set!
            std::swap(m_orders[i],m_orders.back()); m_orders.pop_back();
        }

    for (int i = 0; i < static_cast<int>(m_distances.size()); ++i)
        if (std::get<0>(m_distances[i]) == atomIndex || std::get<1>(m_distances[i]) == atomIndex) {
            std::swap(m_distances[i], m_distances.back());  m_distances.pop_back();
        }

    std::vector<int> tmp_picks;
    for (int i = 0; i < static_cast<int>(m_picks.size()); ++i) if (m_picks[i] != atomIndex) tmp_picks.push_back(m_picks[i]);
    m_picks = tmp_picks;

    if(!isValidIndex(atomIndex)) return;

    int lastAtomIndex = static_cast<int>(m_atoms.size()) - 1;
    if(atomIndex != lastAtomIndex) {
        m_atoms[atomIndex] = m_atoms.back();
        m_positions[atomIndex] = m_positions.back();
        m_group[atomIndex] = m_group.back();

        for (int i = 0; i < static_cast<int>(m_links.size()); ++i) {
            if (std::get<0>(m_links[i]) == lastAtomIndex) std::get<0>(m_links[i]) = atomIndex;
            if (std::get<1>(m_links[i]) == lastAtomIndex) std::get<1>(m_links[i]) = atomIndex;
        }

        for (int i = 0; i < static_cast<int>(m_distances.size()); ++i) {
            if (std::get<0>(m_distances[i]) == lastAtomIndex) std::get<0>(m_distances[i]) = atomIndex;
            if (std::get<1>(m_distances[i]) == lastAtomIndex) std::get<1>(m_distances[i]) = atomIndex;
        }

        for(int i = 0; i < static_cast<int>(m_picks.size()); i++) if (m_picks[i] == lastAtomIndex) {
            m_picks[i] = atomIndex;
        }

    }

    m_atoms.pop_back();
    m_positions.pop_back();
    m_group.pop_back();
}

void Molecule::append(const Molecule & mol,unsigned char maskWrite, unsigned char flip) {

    int n = size();

    const auto & a = mol.atoms();
    const auto & p = mol.positions();
    const auto & g = mol.group();

    for(int i = 0; i < static_cast<int>(a.size()); i++) {
        m_atoms.push_back(a[i]);
        m_positions.push_back(p[i]);
        unsigned char ch = g[i];
        ch &=~maskWrite;
        ch ^=flip;
        m_group.push_back(ch);
    }

    const auto & l = mol.links();
    const auto & o = mol.orders();

    for(int i = 0; i < static_cast<int>(l.size()); i++) {
        m_links.push_back(std::tuple<int,int>(std::get<0>(l[i])+n,std::get<1>(l[i])+n));
        m_orders.push_back(o[i]);
    }

    const auto & pi = mol.picks();
    const auto & d = mol.distances();

    for(int i = 0; i < static_cast<int>(pi.size()); i++) {
        m_picks.push_back(pi[i]);
    }
    m_picks.resize(std::min<size_t>(4,m_picks.size()));

    for(int i = 0; i < static_cast<int>(d.size()); i++) {
        m_distances.push_back(std::tuple<int,int>(std::get<0>(d[i])+n,std::get<1>(d[i])+n));
    }
}

void Molecule::extract(unsigned char mask, Molecule & mol) {

    mol.clear();

    auto & a = mol.atoms();
    auto & p = mol.positions();
    auto & g = mol.group();

    std::vector<int> ordering(m_group.size());
    for(int i = 0; i < static_cast<int>(m_group.size()); i++) if(isGroup(i,mask)) {
        ordering[i] = static_cast<int>(a.size());
        a.push_back(m_atoms[i]);
        p.push_back(m_positions[i]);
        g.push_back(m_group[i]);
    } else ordering[i] = 0;

    auto & l = mol.links();
    auto & o = mol.orders();

    for(int i = 0; i < static_cast<int>(m_links.size()); i++)
        if (isGroup(std::get<0>(m_links[i]),mask) && isGroup(std::get<1>(m_links[i]),mask)   ) {
        l.push_back(std::tuple<int,int>(ordering[std::get<0>(m_links[i])],ordering[std::get<1>(m_links[i])]));
        o.push_back(m_orders[i]);
    }

    auto & pi = mol.picks();
    auto & d = mol.distances();

    for(int i = 0; i < static_cast<int>(m_picks.size()); i++) if (isGroup(m_picks[i],mask))  {
        pi.push_back(ordering[m_picks[i]]);
    }
    m_picks.resize(std::min<size_t>(4,m_picks.size()));

    for(int i = 0; i < static_cast<int>(m_distances.size()); i++)
        if (isGroup(std::get<0>(m_distances[i]),mask) && isGroup(std::get<1>(m_distances[i]),mask)   ) {
                d.push_back(std::tuple<int,int>(ordering[std::get<0>(m_distances[i])],ordering[std::get<1>(m_distances[i])]));
    }
}

//0001
//0010

bool Molecule::isGroup(int id, unsigned char mask) const {
    return ((m_group[id]&mask) == mask);
}

void Molecule::setGroup(int id, unsigned char mask, unsigned char flip) {
    m_group[id]&=~mask;
    m_group[id]^=flip;
}

void Molecule::find(unsigned char mask, std::string sym, unsigned char maskWrite, unsigned char flip) {

    for(int i = 0; i < static_cast<int>(m_atoms.size()); i++) if(isGroup(i,mask)) if(Element::symbol[m_atoms[i]]==sym) {
        setGroup(i,maskWrite,flip);
    }
}
void Molecule::nearby(unsigned char mask, Eigen::Vector3d c, double maxd, double mind, unsigned char maskWrite, unsigned char flip) {

    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        double d = (m_positions[i]-c).norm();
        if(d<=maxd && mind <= d) {
            setGroup(i,maskWrite,flip);
        }
    }
}

void Molecule::neighbours(int id, unsigned char maskWrite, unsigned char flip) {

    for(int k = 0; k < m_links.size(); k++) {
        if (std::get<0>(m_links[k])==id) setGroup(std::get<1>(m_links[k]),maskWrite,flip);
        else if(std::get<1>(m_links[k])==id ) setGroup(std::get<0>(m_links[k]),maskWrite,flip);
    }

}

void Molecule::neighbourhood(unsigned char mask, unsigned char maskWrite, unsigned char flip) {

    std::vector<int> ids;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        ids.push_back(i);
    }
    for(int k = 0; k < static_cast<int>(ids.size()); k++) {
        neighbours(ids[k],maskWrite,flip);
    }

}

int Molecule::getCount(unsigned char mask) const {

    int count = 0;
    for(int i = 0; i < static_cast<int>(m_positions.size()); i++) if(isGroup(i,mask)) {
        count++;
    }
    return count;

}

void Molecule::autoLink(unsigned char mask, double factor,double maxd,double mind) {

  std::vector<std::vector<size_t> > neighbourList;
  std::vector<bool> exclusions;
  exclusions.resize(m_positions.size(),false);
  for(int i = 0; i < static_cast<int>(exclusions.size()); i++) if(!isGroup(i,mask)) {
      exclusions[i] = true;
  }
  generateNeighbourList(neighbourList,m_positions,exclusions,maxd,mind);

  assert(neighbourList.size() == m_atoms.size() && neighbourList.size() == m_positions.size());

  for(int i = 0; i < neighbourList.size(); i++) {
      std::vector<size_t> & list = neighbourList[i];
      double ri = Element::covalentRadius[m_atoms[i]];
      for(int ilist = 0; ilist<list.size();ilist++) {
          int j = list[ilist];
          if(i<j) {
              double rj = Element::covalentRadius[m_atoms[j]];
              if ( (m_positions[i]-m_positions[j]).norm() < factor*(rj+ri) )
                addBond(i,j,'1');
            }
        }
    }

  resolveBondDuplicates();
}

void Molecule::pick(unsigned char mask, int ith) {

    int count = 1;
    int n = static_cast<int>(m_atoms.size());

    if (ith>0) {
        for(int i = 0; i < n; i++) if(isGroup(i,mask)) {
            if (ith == count)  { togglePicks(i); return; }
            count++;
        }
    } else {
        for(int i = n-1; i >= 0; i--) if(isGroup(i,mask)) {
            if (-ith == count)  { togglePicks(i); return; }
            count++;
        }
    }


}

}

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace BlockReader {

class GeneralBlock
{
private:
    int m_length;
public:
    int getLength() const
    {
        return m_length;
    }
    static const char * BlockName()
    {
        return "";
    }

    virtual const char * getBlockName() const
    {
        return GeneralBlock::BlockName();
    }
    virtual void readData(std::istream & is) ;

    bool checkBlock(std::string & line)
    {
        std::string blockName;
        int         blockLength;
        bool        result = false;

        std::istringstream ss(line);

        ss >> blockName;
        ss >> blockLength;

        if (!ss.fail()) result = true;

        if(result && (blockName == getBlockName() || getBlockName() == GeneralBlock::BlockName()) && (blockLength >= 0) )
        {
            m_length = blockLength;
            return true;
        }

        return false;
    }
    static bool checkBlock(const std::string & line,const std::string & blockName,int & length)
    {
        std::string blockNameTest;
        int         blockLength;
        bool        result = false;

        std::istringstream ss(line);

        ss >> blockNameTest;
        ss >> blockLength;

        if (!ss.fail()) result = true;

        if(result && (blockName == blockNameTest || blockName == GeneralBlock::BlockName()) && (blockLength >= 0) )
        {
            length = blockLength;
            return true;
        }

        return false;
    }
    static bool skipBlock(std::string & line,std::istream &is)
    {
        std::string blockName;
        int         blockLength;
        bool        result = false;

        std::istringstream ss(line);

        ss >> blockName;
        ss >> blockLength;

        if (!ss.fail()) result = true;

        if(result && (blockName.size()>0) && (blockLength >= 0) )
        {
            for(int l = 0; l < blockLength; l++)
            {
                std::string dummy;
                std::getline(is,dummy);
                std::cout << dummy << std::endl;
            }
            return true;
        }

        return false;
    }
    static bool checkEmpty(std::string & line)
    {
        return line.find_first_not_of(" \t\n\v\f\r") == std::string::npos;
    }
};
class AnyBlock : public GeneralBlock
{
private:
    std::vector<std::string> m_lines;

public:
    void readData(std::istream & is)
    {
        for(int l = 0; l < getLength(); l++)
        {
            std::string line;
            std::getline(is,line);
            m_lines.push_back(line);

            //std::cout << line << std::endl;
        }
    }

    const std::vector<std::string> & getLines() const
    {
        return m_lines;
    }
};
class Block : public GeneralBlock
{
private:
    std::string              m_blockName;
    std::vector<std::string> m_lines;
public:
    const char * getBlockName() const
    {
        return m_blockName.c_str();
    }
    Block(const std::string & blockName) : m_blockName(blockName) {}
    void readData(std::istream & is)
    {
        for(int l = 0; l < getLength(); l++)
        {
            std::string line;
            std::getline(is,line);
            //std::cout << line << std::endl;
            m_lines.push_back(line);
        }
    }

    const std::vector<std::string> & getLines() const
    {
        return m_lines;
    }
};

}
namespace Molecule {

bool loadEDI(Molecule & mol,std::istream & is)
{
    mol.clear();
    std::string line;
    bool structureBlockIn = false;
    bool cellBlockIn = false;
    bool atomBlockIn = false;
    bool bondBlockIn = false;
    while (!std::getline(is,line).eof())
    {
        int length = 0;
        if(BlockReader::GeneralBlock::checkBlock(line,"STRUCTURE",length))
        {

            for(int l = 0; l < length; l++)
            {
                if(std::getline(is,line).fail()) break;
                std::istringstream ss(line);
                std::string name;
                ss >> name;
                mol.title() = name;
            }
            structureBlockIn = true;

        }
        else if(BlockReader::GeneralBlock::checkBlock(line,"CELL",length))
        {

            for(int l = 0; l < length; l++)
            {
                if(std::getline(is,line).fail()) break;
                std::istringstream ss(line);

                Eigen::Vector3d a1,a2,a3;
                ss >> a1.x() >> a1.y() >> a1.z();
                ss >> a2.x() >> a2.y() >> a2.z();
                ss >> a3.x() >> a3.y() >> a3.z();
                if(!ss) return false;
                mol.setCell(a1,a2,a3);

            }
            cellBlockIn = true;

        }
        else if(BlockReader::GeneralBlock::checkBlock(line,"ATOM",length))
        {

            for(int l = 0; l < length; l++)
            {
                if(std::getline(is,line).fail()) break;
                std::istringstream ss(line);
                std::string label;
                double x,y,z;
                int g;
                if(!(ss >> label >> x >> y >> z >> g)) return false;
                mol.addAtom(Element::atomicNumber(label),{x,y,z},g);

            }
            atomBlockIn = true;

        }
        else if(BlockReader::GeneralBlock::checkBlock(line,"BOND",length))
        {

            for(int l = 0; l < length; l++)
            {
                if(std::getline(is,line).fail()) break;
                std::istringstream ss(line);
                int i,j;
                char o;
                if(!(ss >> i >> j >> o)) return false;
                mol.addBond(i-1,j-1,o);
            }
            bondBlockIn = true;

        }
        else if(BlockReader::GeneralBlock::skipBlock(line,is)) ;
        else if(BlockReader::GeneralBlock::checkEmpty(line)) break;
        else break;
    }
    //return structureBlockIn && cellBlockIn && atomBlockIn && bondBlockIn;
    //return atomBlockIn || (atomBlockIn && bondBlockIn) || (atomBlockIn && cellBlockIn) ||;

    mol.resolveBondDuplicates();

    return atomBlockIn || ( atomBlockIn && structureBlockIn && (bondBlockIn || cellBlockIn) );
}
bool writeEDI(Molecule & mol,std::ostream & os) {
    std::string line;
    os << "STRUCTURE 1" << std::endl;
    os << mol.title() << std::endl;

    if(mol.hasCell()) {
        os << "CELL 1" << std::endl;
        os << mol.a1().x() << ' ';
        os << mol.a1().y() << ' ';
        os << mol.a1().z() << ' ';
        os << mol.a2().x() << ' ';
        os << mol.a2().y() << ' ';
        os << mol.a2().z() << ' ';
        os << mol.a3().x() << ' ';
        os << mol.a3().y() << ' ';
        os << mol.a3().z() << ' ' << std::endl;

    } else os << "CELL 0" << std::endl;

    const std::vector<int> & atoms = mol.atoms();
    const std::vector<Eigen::Vector3d> & positions = mol.positions();

    os << "ATOM " << mol.size() << std::endl;

    const std::vector<unsigned char> & group = mol.group();

    for(int i = 0; i < static_cast<int>(mol.size()); i++) {

        os << std::left << std::setw(5) << Element::symbol[atoms[i]] << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].x() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].y() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].z() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << int(group[i]) << ' ';
        os << std::endl;

    }

    const std::vector<std::tuple<int,int>> & links = mol.links();
    const std::vector<char> & orders = mol.orders();

    os << "BOND " << mol.links().size() << std::endl;

    for(int i = 0; i < static_cast<int>(links.size()); i++) {

        os << std::right << std::setw(6) << std::setprecision(6) << std::fixed << std::get<0>(links[i])+1 << ' ';
        os << std::right << std::setw(6) << std::setprecision(6) << std::fixed << std::get<1>(links[i])+1 << ' ';
        os << std::right << std::setw(6) << std::setprecision(6) << std::fixed << orders[i] << ' ';
        os << std::endl;

    }

    os << std::endl;

    return true;
}

bool loadxyz(Molecule & mol,std::istream & is) {

    mol.clear();


    std::string line;
    int n;
    if(std::getline(is,line))
    {
        std::istringstream iss(line);
        if(!(iss >> n)) return false;
    }
    else return false;

    if(std::getline(is,line)) { }

    mol.reserve(n);

    for(int i = 0; i < n; i++)
        if(std::getline(is,line))
        {
            std::istringstream iss(line);
            std::string label;
            double x,y,z;
            if(!(iss >> label >> x >> y >> z)) return false;

            mol.addAtom(Element::atomicNumber(label),{x,y,z},0);
        }
        else return false;

    return true;
}
bool writexyz(Molecule & mol,std::ostream & os) {

    os << std::left << std::setw(5) << mol.size() << ' ';
    os << std::endl << std::endl;

    const std::vector<int> & atoms = mol.atoms();
    const std::vector<Eigen::Vector3d> & positions = mol.positions();

    for(int i = 0; i < mol.size(); i++) {
        os << std::left << std::setw(5) << Element::symbol[atoms[i]] << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].x() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].y() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].z() << ' ';
        os << std::endl;
    }

    return true;
}


bool loadaxyz(Molecule & mol,std::istream & is) {

    mol.clear();


    std::string line;
    int n;
    if(std::getline(is,line))
    {
        std::istringstream iss(line);
        if(!(iss >> n)) return false;
    }
    else return false;

    if(std::getline(is,line)) {

        std::istringstream iss(line);
        Eigen::Vector3d a1,a2,a3;
        iss >> a1.x() >> a1.y() >> a1.z();
        iss >> a2.x() >> a2.y() >> a2.z();
        iss >> a3.x() >> a3.y() >> a3.z();
        if(!iss) return false;
        mol.setCell(a1,a2,a3);
    }

    mol.reserve(n);

    for(int i = 0; i < n; i++)
        if(std::getline(is,line))
        {
            std::istringstream iss(line);
            std::string label;
            double x,y,z;
            if(!(iss >> label >> x >> y >> z)) return false;

            mol.addAtom(Element::atomicNumber(label),{x,y,z},0);
        }
        else return false;

    return true;
}
bool writeaxyz(Molecule & mol,std::ostream & os) {

    os << std::left << std::setw(5) << mol.size() << ' ';
    os << std::endl;

    if(mol.hasCell()) {
        os << mol.a1().x() << ' ';
        os << mol.a1().y() << ' ';
        os << mol.a1().z() << ' ';
        os << mol.a2().x() << ' ';
        os << mol.a2().y() << ' ';
        os << mol.a2().z() << ' ';
        os << mol.a3().x() << ' ';
        os << mol.a3().y() << ' ';
        os << mol.a3().z() << ' ' << std::endl;
    } else os << "0.0 0.0 0.0   0.0 0.0 0.0   0.0 0.0 0.0" << std::endl;

    const std::vector<int> & atoms = mol.atoms();
    const std::vector<Eigen::Vector3d> & positions = mol.positions();

    for(int i = 0; i < mol.size(); i++) {
        os << std::left << std::setw(5) << Element::symbol[atoms[i]] << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].x() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].y() << ' ';
        os << std::right << std::setw(12) << std::setprecision(6) << std::fixed << positions[i].z() << ' ';
        os << std::endl;
    }

    return true;
}



bool loadxyzpy(Molecule & mol,std::istream & is) { // TODO avoid lot of copies

    mol.clear();
    std::string line;

    std::vector<std::string> label;
    std::vector<double> x,y,z;
    std::vector<int> group;
    std::vector<int> link1;
    std::vector<int> link2;
    std::vector<char> order;
    std::vector<int> pick;

    while(std::getline(is,line))
    {
        std::istringstream iss{std::regex_replace(line, std::regex{R"(\[|\]|,|=|')"}, " ")};
        std::string varid;
        if (iss>>varid)
        if (varid=="symbol") {
            label = std::vector<std::string>({std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}});
        } else if (varid=="x") {
            x = std::vector<double>({std::istream_iterator<double>{iss}, std::istream_iterator<double>{}});
        } else if (varid=="y") {
            y = std::vector<double>({std::istream_iterator<double>{iss}, std::istream_iterator<double>{}});
        } else if (varid=="z") {
            z = std::vector<double>({std::istream_iterator<double>{iss}, std::istream_iterator<double>{}});
        } else if (varid=="group") {
            group = std::vector<int>({std::istream_iterator<int>{iss}, std::istream_iterator<int>{}});
        } else if (varid=="link1") {
            link1 = std::vector<int>({std::istream_iterator<int>{iss}, std::istream_iterator<int>{}});
        } else if (varid=="link2") {
            link2 = std::vector<int>({std::istream_iterator<int>{iss}, std::istream_iterator<int>{}});
        } else if (varid=="order") {
            order = std::vector<char>({std::istream_iterator<char>{iss}, std::istream_iterator<char>{}});
        } else if (varid=="pick") {
            pick = std::vector<int>({std::istream_iterator<int>{iss}, std::istream_iterator<int>{}});
        }
    }

    if (label.size()==x.size() && x.size() == y.size() && y.size() == z.size() && z.size() == group.size()) {

        for(int i = 0; i < label.size(); i++)
            mol.addAtom(Element::atomicNumber(label[i]),{x[i],y[i],z[i]},group[i]);
    } else return false;
    if (link1.size()==link2.size() && link2.size() == order.size() ) {

        for(int i = 0; i < link1.size(); i++)
            mol.addBond(link1[i],link2[i],order[i]);
        mol.resolveBondDuplicates();
    } else return false;

    for(int i = 0; i < pick.size(); i++)
        mol.togglePicks(pick[i]);

    return true;
}

bool writexyzpy(Molecule & mol,std::ostream & os) {

    const std::vector<int> & atoms = mol.atoms();
    const std::vector<Eigen::Vector3d> & positions = mol.positions();

    const std::vector<unsigned char> & group = mol.group();
    const std::vector<int> & pick = mol.picks();

    auto & links = mol.links();
    auto & orders = mol.orders();

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "atom=[ ";
        os << atoms[i];
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "group=[ ";
        os << int(group[i]);
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "symbol=[ ";
        os << "'" << Element::symbol[atoms[i]] << "'";
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "x=[ ";
        os << std::setprecision(6) << std::fixed << positions[i].x();
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "y=[ ";
        os << std::setprecision(6) << std::fixed << positions[i].y();
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < mol.size(); i++) {
        if(i==0) os << "z=[ ";
        os << std::setprecision(6) << std::fixed << positions[i].z();
        if(i==mol.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < pick.size(); i++) {
        if(i==0) os << "pick=[ ";
        os << pick[i];
        if(i==pick.size()-1) os << " ]" << std::endl; else os << " , ";
    }

    for(int i = 0; i < links.size(); i++) {
        if(i==0) os << "link1=[ ";
        os << std::get<0>(links[i]);
        if(i==links.size()-1) os << " ]" << std::endl; else os << " , ";
    }
    for(int i = 0; i < links.size(); i++) {
        if(i==0) os << "link2=[ ";
        os << std::get<1>(links[i]);
        if(i==links.size()-1) os << " ]" << std::endl; else os << " , ";
    }
    for(int i = 0; i < orders.size(); i++) {
        if(i==0) os << "order=[ ";
        os << orders[i];
        if(i==orders.size()-1) os << " ]" << std::endl; else os << " , ";
    }
    for(int i = 0; i < mol.a1().size(); i++) {
        if(i==0) os << "a1=[ ";
        os << mol.a1()[i];
        if(i==mol.a1().size()-1) os << " ]" << std::endl; else os << " , ";
    }
    for(int i = 0; i < mol.a2().size(); i++) {
        if(i==0) os << "a2=[ ";
        os << mol.a2()[i];
        if(i==mol.a2().size()-1) os << " ]" << std::endl; else os << " , ";
    }
    for(int i = 0; i < mol.a3().size(); i++) {
        if(i==0) os << "a3=[ ";
        os << mol.a3()[i];
        if(i==mol.a3().size()-1) os << " ]" << std::endl; else os << " , ";
    }
    os << std::endl << std::endl;

    return true;
}

}
