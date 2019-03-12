[getcell] getcell()
[getpicks] getpicks()
[getcoord] getcoord(id)
[getcenter] getcenter(rbit)
[getboundingbox] getboundingbox(rbit)
[getatom] getatom(id)
[getcount] getcount(rbit)
[add] add(atom,x,y,z,wbit)
[set] set(id,atom,x,y,z,wbit)
[pickith] pickith(rbit,ith)
[pick] pick(id)
[picknone] picknone()
[erase] erase(rbit)
[copy] copy(rbit,wbit)
repeatcell repeatcell(n1,n2,n3)
[sort] sort(rbit,x,y,z)
[gather] gather(rbit)
[framecreate] framecreate()
[frameduplicate] frameduplicate()
[frameremove] frameremove()
[framenext] framenext()
[frameprevious] frameprevious()
[write] write(filename,rbit)
[read] read(filename,wbit)
[filecreate] filecreate(filename)
[fileduplicate] fileduplicate(filename)
[fileclose] fileclose()
[filesave] filesave()
[filesave] filesave(filename)
[fileopen] fileopen(filename)
[find] find(rbit,sym,wbit)
[push] push(rbit)
[pop] pop(wbit)
[grouplist] grouplist(list,wbit)
[group] group(rbit,wbit)
[distance] distance(id1,id2)
[distancenone] distancenone(rbit)
[translate] translate(rbit,x,y,z)
[rotatet] rotate(rbit,ax,ay,az,angleRad,ox,oy,oz)
[rotateto] rotateto(rbit,x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3)
[alignalong] alignalong(rbit,x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3)
[autoalign] autoalign(rbit,xyz)
[cell] cell(x1,   y1,   z1,   x2,   y2,   z2,   x3,   y3,   z3)
[neighbourhood] neighbourhood(rbit,wbit)
[neighbours] neighbours(id,wbit)
[autolink] autolink(rbit,factor,maxd,mind)
[nearby] nearby(rbit,x,y,z,maxd,mind,wbit)
[avogadro-manipulate] write('temp.xyz',1); execute('avogadro temp.xyz'); erase(1); read('temp.xyz');
