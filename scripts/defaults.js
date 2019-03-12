function getcell()    { var c=control.script_cell(); return {a:[c[0],c[1],c[2]], b:[c[3],c[4],c[5]] , c:[c[6],c[7],c[8]]}; };
function getpicks()           { return control.script_picks(); };
function getcoord(id)         { return control.script_coord(id); };
function getcenter(rbit)      { return control.script_center(rbit); };
function getboundingbox(rbit) { return control.script_boundingbox(rbit); };
function getatom(id)          { return control.script_atom(id); };
function getcount(rbit)       { return control.script_count(rbit); };

function add(atom,x,y,z,wbit) { control.script_add(atom,x,y,z,wbit); };
function set(id,atom,x,y,z,wbit) { control.script_set(id,atom,x,y,z,wbit); };

function pickith(rbit,ith) { control.script_pick(rbit,ith); };
function pick(id) { control.script_pick(id); };
function picknone() { control.script_pick(); };

function erase(rbit) {control.script_erase(rbit);};
function copy(rbit,wbit) {control.script_copy(rbit, 255, wbit);};
function repeatcell(n1,n2,n3) {control.script_repeatcell(n1,n2,n3);};
function sort(rbit,x,y,z) {control.script_sort(rbit,x,y,z);};
function gather(rbit) {control.script_gather(rbit);};

function framecreate() {control.script_create(); };
function frameduplicate() {control.script_duplicate(); };
function frameremove() {control.script_remove(); };
//bool move();
function framenext()     {control.script_next();     };
function frameprevious() {control.script_previous(); };
//bool first();
//bool last();

function write(filename,rbit) {control.script_write(   rbit,  filename);       };
function read(filename,wbit)  {control.script_read(   255,   wbit,  filename); };

function filecreate(filename)    {control.script_filecreate( filename);        };
function fileduplicate(filename) {control.script_fileduplicate(  filename);    };
function fileclose()             {control.script_fileclose();          };
function filesave()              {control.script_filesave();           };
function filesave(filename)      {control.script_filesave(  filename); };
function fileopen(filename)      {control.script_fileopen(  filename); };

function find(rbit,sym,wbit) {control.script_find(   rbit,  sym,    255,    wbit); };

function push(rbit) {control.script_push(   rbit); };
function pop(wbit) {control.script_pop(   255,    wbit); };

function grouplist(list,wbit) {control.script_group(   255,    wbit, list); };
function group(rbit,wbit) {control.script_group(   255,    wbit,    rbit) ; };
function distance(id1,id2) {control.script_distance(id1,id2); };
function distancenone(rbit) {control.script_distance(); };

function translate(rbit,x,y,z) {control.script_translate(   rbit,  x,  y,  z); };
function rotate(rbit,ax,ay,az,angleRad,ox,oy,oz) {control.script_rotate(     rbit,  ax,  ay,  az,  angleRad,   ox,  oy,  oz); };

function rotateto(rbit,x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3) {control.script_rotateto(     rbit,   x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3); };

function alignalong(rbit,x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3) {control.script_alignalong(     rbit,   x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3); };
function autoalign(rbit,xyz) {control.script_autoalign(     rbit,   xyz); };

function cell(x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3) {control.script_cell(  x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3); };

function neighbourhood(rbit,wbit) {control.script_neighbourByGroup(   rbit,    255,    wbit); };
function neighbours(id,wbit) {control.script_neighbourByPick(id,    255,    wbit); };

function autolink(rbit,factor,maxd,mind) {control.script_autolink(     rbit,  factor,  maxd,  mind); };
function nearby(rbit,x,y,z,maxd,mind,wbit) {control.script_nearby(     rbit,   x,   y,   z,  maxd,  mind,    255,    wbit); };

function settings(key,value) { control.script_settings(key,value); };

function execute(command) { control.script_execute(command); };
function launch(command) { control.script_launch(command); };


G1 = 1;
G2 = 2;
G3 = 4;
G4 = 8;
G5 = 16;
G6 = 32;
G7 = 64;
G8 = 128;

