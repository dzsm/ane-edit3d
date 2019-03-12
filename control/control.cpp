#include "control.h"
#include <fstream>

Control::Control(QObject *parent) : QObject(parent) {

}

void Control::broadcastChange() {
    emit changed();
}

void Control::wait() {
    // if (!((++loadcount)%10)) { emit changed(); { QTime dieTime = QTime::currentTime().addMSecs( 1 ); while( QTime::currentTime() < dieTime ) { QCoreApplication::processEvents( QEventLoop::AllEvents, 100 ); } } }
    // QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
}

void Control::errorMsg(std::string msg) {
    emit error(tr(msg.c_str()));
}
void Control::showMsg(std::string msg) {
    emit message(tr(msg.c_str()));
}

std::string extension(std::string filePath) {
    std::string ext;
    size_t i = filePath.rfind('.', filePath.length());
    if (i != std::string::npos)  ext = filePath.substr(i+1, filePath.length() - i);
    return ext;
}

QVariant Control::script_center(unsigned char bit){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) QVariant(QVariantList({0.0,0.0,0.0}));

    MoleculeData & mol = sequence.currentFrame();

    double x,y,z;
    Eigen::Vector3d v = mol.getCenter(bit);
    x = v.x();
    y = v.y();
    z = v.z();
    return QVariant(QVariantList({x,y,z}));
}
QVariant Control::script_boundingbox(unsigned char bit) {
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return QVariant(QVariantList({0.0,0.0,0.0,0.0,0.0,0.0}));

    MoleculeData & mol = sequence.currentFrame();

    double mx,my,mz,Mx,My,Mz;
    Eigen::Vector3d m,M;
    Eigen::Vector3d v = mol.getBoundingBox(bit,m,M);
    mx = m.x();
    my = m.y();
    mz = m.z();
    Mx = M.x();
    My = M.y();
    Mz = M.z();

    return QVariant(QVariantList({mx,my,mz,Mx,My,Mz}));
}
QVariant Control::script_cell() {
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return QVariant(QVariantList({0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0}));

    MoleculeData & mol = sequence.currentFrame();

    double x1,y1,z1,x2,y2,z2,x3,y3,z3;

    x1 = mol.a1().x();
    y1 = mol.a1().y();
    z1 = mol.a1().z();

    x2 = mol.a2().x();
    y2 = mol.a2().y();
    z2 = mol.a2().z();

    x3 = mol.a3().x();
    y3 = mol.a3().y();
    z3 = mol.a3().z();
    return QVariant(QVariantList({x1,y1,z1,x2,y2,z2,x3,y3,z3}));
}
QVariant Control::script_coord(int id){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return QVariant(QVariantList({0.0,0.0,0.0}));

    MoleculeData & mol = sequence.currentFrame();

    double x,y,z;

    x = 0;
    y = 0;
    z = 0;

    if(!(id<mol.positions().size() && id>=0)) return QVariant(QVariantList({0.0,0.0,0.0}));

    x = mol.positions()[id].x();
    y = mol.positions()[id].y();
    z = mol.positions()[id].z();

    return QVariant(QVariantList({x,y,z}));
}
QVariant Control::script_picks(){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return QVariant(QVariantList({}));

    MoleculeData & mol = sequence.currentFrame();

    QVariantList vs;
    for(int i = 0; i < mol.picks().size();i++) vs.push_back(mol.picks()[i]);
    return QVariant(vs);

}
QVariant Control::script_atom(int id){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return 0;

    MoleculeData & mol = sequence.currentFrame();

    if(!(id<mol.picks().size() && id>=0)) return 0;

    return mol.atoms()[mol.picks()[id]];

}
QVariant Control::script_count(unsigned char bit){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return 0;

    MoleculeData & mol = sequence.currentFrame();

    return mol.getCount(bit);
}

void Control::script_group(unsigned char maskWrite, unsigned char flip, const std::vector<int> & atomIds){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    auto & group = mol.group();
    for(int i = 0; i < static_cast<int>(atomIds.size()); i++)
        if(atomIds[i]<mol.size()&&0<=atomIds[i]) mol.setGroup(atomIds[i],maskWrite,flip); else return;

    broadcastChange();
    return;

}
void Control::script_group(unsigned char maskWrite, unsigned char flip, unsigned char maskRead) {
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    auto & group = mol.group();
    for(int i = 0; i < static_cast<int>(group.size()); i++)
       if(mol.isGroup(i,maskRead)) mol.setGroup(i,maskWrite,flip);

    broadcastChange();
    return ;
}
void Control::script_neighbourByGroup(unsigned char maskRead, unsigned char maskWrite, unsigned char flip) {
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.neighbourhood(maskRead,maskWrite,flip);

    broadcastChange();
    return;
}
void Control::script_neighbourByPick(int id, unsigned char maskWrite, unsigned char flip) {
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    if(!(id<mol.picks().size() && id>=0)) return;

    mol.neighbours(mol.picks()[id],maskWrite,flip);
    broadcastChange();
    return;
}
void Control::script_pick(int atomId){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.togglePicks(atomId);

    broadcastChange();
    return;
}
void Control::script_pick(unsigned char mask,int ith){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.pick(mask, ith);

    broadcastChange();
    return;
}
void Control::script_pick(){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.clearPicks();

    broadcastChange();
    return;
}
void Control::script_distance(int atomId1,int atomId2){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.toggleDistance(atomId1,atomId2);

    broadcastChange();
    return;
}
void Control::script_distance(){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.clearDistances();

    broadcastChange();
    return;
}
void Control::script_translate(unsigned char mask,double x,double y,double z){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.translate(mask,Eigen::Vector3d(x,y,z));

    broadcastChange();
    return;
}
void Control::script_rotate(unsigned char mask,double ax,double ay,double az,double angleRad, double ox,double oy,double oz){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.rotateAround(mask,Eigen::Vector3d(ax,ay,az),angleRad,Eigen::Vector3d(ox,oy,oz));

    broadcastChange();
    return;
}
void Control::script_rotateto(unsigned char mask, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.rotateTo(mask,Eigen::Vector3d(x1,y1,z1),Eigen::Vector3d(x2,y2,z2),Eigen::Vector3d(x3,y3,z3));

    broadcastChange();
    return;
}
void Control::script_alignalong(unsigned char mask, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.alignAlong(mask,Eigen::Vector3d(x1,y1,z1),Eigen::Vector3d(x2,y2,z2),Eigen::Vector3d(x3,y3,z3));

    broadcastChange();
    return;
}
void Control::script_autoalign(unsigned char mask, QString qxyz){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    std::string xyz = qxyz.toStdString();

    MoleculeData & mol = sequence.currentFrame();

    if(xyz.size()!=3) return;

    char c[3] = {xyz[0],xyz[1],xyz[2]};
    mol.autoAlign(mask,c);

    broadcastChange();
    return;
}
void Control::script_erase(unsigned char mask){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.removeAtoms(mask);

    broadcastChange();
    return;
}
void Control::script_copy(unsigned char maskRead, unsigned char maskWrite, unsigned char flip){
    std::lock_guard<std::mutex> lock(mutex);

    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    Molecule::Molecule submol;
    mol.extract(maskRead,submol);
    mol.append(submol,maskWrite, flip);

    broadcastChange();
    return;
}
void Control::script_repeatcell(int n1,int n2, int n3){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();
    mol.repeatCell(n1,n2,n3);

    broadcastChange();
    return;
}
void Control::script_sort(unsigned char mask,double x,double y,double z){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.sort(mask,Eigen::Vector3d(x,y,z));

    broadcastChange();
    return;
}
void Control::script_gather(unsigned char mask){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.gather(mask);

    broadcastChange();
    return;
}
void Control::script_add(int atomicNumber,double x,double y,double z,unsigned char mask){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.addAtom(atomicNumber,Eigen::Vector3d(x,y,z),mask);

    broadcastChange();
    return;
}
void Control::script_cell(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.setCell(Eigen::Vector3d(x1,y1,z1),Eigen::Vector3d(x2,y2,z2),Eigen::Vector3d(x3,y3,z3));

    broadcastChange();
    return;
}

void Control::script_autolink(unsigned char mask,double factor,double maxd,double mind){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.autoLink(mask,factor,maxd,mind);

    broadcastChange();
    return;
}
void Control::script_nearby(unsigned char mask,double x, double y, double z,double maxd,double mind, unsigned char maskWrite, unsigned char flip){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    mol.nearby(mask,Eigen::Vector3d(x,y,z),maxd,mind,maskWrite,flip);

    broadcastChange();
    return;
}
void Control::script_find(unsigned char mask,QString qsym, unsigned char maskWrite, unsigned char flip){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    std::string sym = qsym.toStdString();

    MoleculeData & mol = sequence.currentFrame();

    mol.find(mask,sym,maskWrite,flip);

    broadcastChange();
    return;
}
void Control::script_create(){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty())
        sequence.createRowWithColumn();
    else
        sequence.createRow();

    broadcastChange();
    return;
}
void Control::script_duplicate(){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;
    MoleculeData & mol = sequence.currentFrame();
    sequence.createRow() = mol;

    broadcastChange();
    return;
}
void Control::script_remove(){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;
    if (sequence.currentColumn().numRows() > 1 ) sequence.removeCurrentRow(); else return;
    broadcastChange();
    return;
}
void Control::script_next(){
    std::lock_guard<std::mutex> lock(mutex);
    sequence.moveForward();
    broadcastChange();
    return;
}
void Control::script_previous(){
    std::lock_guard<std::mutex> lock(mutex);
    sequence.moveBackward();
    broadcastChange();
    return;
}
void Control::script_push(unsigned char mask){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    MoleculeData submol;
    mol.extract(mask,submol);
    if (submol.size() > 0) stack.push_back(submol);

    broadcastChange();
    return;
}
void Control::script_pop(unsigned char maskWrite, unsigned char flip) {
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    MoleculeData & mol = sequence.currentFrame();

    if(stack.size()>0) {
        mol.append(stack.back(),maskWrite,flip);
        stack.pop_back();
    }
    broadcastChange();
    return;
}
void Control::script_write(unsigned char mask, QString qfilePath){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    std::string filePath = qfilePath.toStdString();

    MoleculeData & mol = sequence.currentFrame();
    MoleculeData submol;
    mol.extract(mask,submol);

    if (extension(filePath) == "edi") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;
        Molecule::writeEDI(submol,os);
        os.close();

    } else if (extension(filePath) == "xyz" || extension(filePath) == "ani") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;
        Molecule::writexyz(submol,os);
        os.close();
    } else if (extension(filePath) == "py") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;
        Molecule::writexyzpy(submol,os);
        os.close();
    } else return;

    broadcastChange();
    return;
}
void Control::script_read(unsigned char mask, unsigned char flip, QString qfilePath){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    std::string filePath = qfilePath.toStdString();

    MoleculeData & mol = sequence.currentFrame();
    MoleculeData submol;

    if (extension(filePath) == "edi") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(!Molecule::loadEDI(submol,is)) return;
        is.close();

    } else if (extension(filePath) == "xyz" || extension(filePath) == "ani") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(!Molecule::loadxyz(submol,is)) return;

        is.close();
    } else if (extension(filePath) == "py") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(!Molecule::loadxyzpy(submol,is)) return;

        is.close();
    } else return;

    mol.append(submol,mask,flip);

    broadcastChange();
    return;
}
void Control::script_filecreate(QString qfilename){
    std::lock_guard<std::mutex> lock(mutex);

    std::string filename = qfilename.toStdString();

    if (extension(filename) != "edi" &&
        extension(filename) != "xyz" &&
        extension(filename) != "ani" ) return;

    sequence.createRowWithColumn();
    sequence.currentColumn().data().fileInfo.setFile(filename.c_str());

    broadcastChange();
    return;
}
void Control::script_fileduplicate(QString qfilename){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    std::string filename = qfilename.toStdString();

    MoleculeData & mol = sequence.currentFrame();
    sequence.createRowWithColumn() = mol;
    sequence.currentColumn().data().fileInfo.setFile(filename.c_str());

    broadcastChange();
    return;
}
void Control::script_fileclose(){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    sequence.removeCurrentColumn();

    broadcastChange();
    return;
}
void Control::script_filesave(){

    std::string filePath;

    {
        std::lock_guard<std::mutex> lock(mutex);
        if(sequence.isEmpty()) return;
        filePath = sequence.currentColumn().data().fileInfo.absoluteFilePath().toStdString();
    }

    return script_filesave(filePath.c_str());
}
void Control::script_filesave(QString qfilePath){
    std::lock_guard<std::mutex> lock(mutex);
    if(sequence.isEmpty()) return;

    std::string filePath = qfilePath.toStdString();


    if (extension(filePath) == "edi") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;

        auto & column = sequence.currentColumn();
        for(int r = 0; r < column.numRows(); r++)
            Molecule::writeEDI(column.at(r),os);
        os.close();

    } else if (extension(filePath) == "xyz" || extension(filePath) == "ani") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;

        auto & column = sequence.currentColumn();
        for(int r = 0; r < column.numRows(); r++)
            Molecule::writexyz(column.at(r),os);
        os.close();
    } else if (extension(filePath) == "axyz" || extension(filePath) == "aani") {
        std::ofstream os(filePath.c_str());

        if(!os.is_open()) return;

        auto & column = sequence.currentColumn();
        for(int r = 0; r < column.numRows(); r++)
            Molecule::writeaxyz(column.at(r),os);
        os.close();
    } else return;

    sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
    sequence.currentColumn().data().isSaved = true;

    broadcastChange();
    return;
}
void Control::script_fileopen(QString qfilePath){
    std::lock_guard<std::mutex> lock(mutex);

    std::string filePath = qfilePath.toStdString();

    if (extension(filePath) == "edi") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(Molecule::loadEDI(sequence.createRowWithColumn(),is)) {
            sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
            broadcastChange();
            while (Molecule::loadEDI(sequence.createRow(),is)) { broadcastChange(); wait(); }
            sequence.removeCurrentRow();
        } else {
            sequence.removeCurrentRow();
            return;
        }
        is.close();

    } else if (extension(filePath) == "xyz" || extension(filePath) == "ani") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(Molecule::loadxyz(sequence.createRowWithColumn(),is)) {
            sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
            broadcastChange();
            int loadcount = 0;
            while (Molecule::loadxyz(sequence.createRow(),is)) {         broadcastChange(); wait();  }

            sequence.removeCurrentRow();
        } else {
            sequence.removeCurrentRow();
            return;
        }
        is.close();
    } else if (extension(filePath) == "axyz" || extension(filePath) == "aani") {
        std::ifstream is(filePath.c_str());
        if(!is.is_open()) return;

        if(Molecule::loadaxyz(sequence.createRowWithColumn(),is)) {
            sequence.currentColumn().data().fileInfo.setFile(filePath.c_str());
            broadcastChange();
            int loadcount = 0;
            while (Molecule::loadaxyz(sequence.createRow(),is)) {         broadcastChange(); wait();  }

            sequence.removeCurrentRow();
        } else {
            sequence.removeCurrentRow();
            return;
        }
        is.close();
    } return;

    if(sequence.isEmpty()) return;
    sequence.currentColumn().data().isSaved = true;

    broadcastChange();
    return;

}
void Control::script_settings(QString key,QString value) {
    std::lock_guard<std::mutex> lock(mutex);

    settings[key.toStdString()] = value.toStdString();

    broadcastChange();
    return;
}

void Control::script_execute(QString command) {
    std::lock_guard<std::mutex> lock(mutex);

    QProcess * process = new QProcess();

    QString shellCommand = command;
    process->execute(shellCommand);

    //QString file = QDir::homePath + QDir::separator + "file.exe";

    delete process;

    broadcastChange();
    return;
}
void Control::script_launch(QString command) {
    std::lock_guard<std::mutex> lock(mutex);

    QProcess * process = new QProcess();

    QString shellCommand = command;

    process->startDetached(shellCommand);

    delete process;

    broadcastChange();
    return;
}



