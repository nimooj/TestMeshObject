
print("start\n");
# v, vn, v_idx/vn_idx
f = open("avatar_woman.obj", "r")


verts = []
norms = []
texts = []

vidx = []
nidx = []
tidx = []

while True:
	l = f.readline()
	if not l : break

	ls = []
	ls = l.split()

	if (ls[0] == 'v') :
		verts.append([float(ls[1]), float(ls[2]), float(ls[3])])

	if (ls[0] == 'vn') :
		norms.append([float(ls[1]), float(ls[2]), float(ls[3])])

	if (ls[0] == 'f') :
		first = ls[1].split('/')
		vidx.append(int(first[0]))
		nidx.append(int(first[2]))

		second = ls[2].split('/')
		vidx.append(int(second[0]))
		nidx.append(int(second[2]))

		third = ls[3].split('/')
		vidx.append(int(third[0]))
		nidx.append(int(third[2]))


f.close()

print("Done0")

# vt vt_idx
p = open("avatar_woman_1_oPP.OBJ", "r")

while True:
	l = p.readline()
	if not l : break
	ls = l.split()


	if (ls[0] == 'vt') :
		texts.append([float(ls[1]), float(ls[2])])


	if (ls[0] == 'f') :
		first = ls[1].split('/')
		tidx.append(int(first[1]))

		second = ls[2].split('/')
		tidx.append(int(second[1]))

		third = ls[3].split('/')
		tidx.append(int(third[1]))


p.close()


print("Done1")


n = open("avatar_woman_new.obj", "w")

for i in range(0, len(verts)):
	n.write("v %f %f %f\n" % (verts[i][0], verts[i][1], verts[i][2]))

for i in range(0, len(norms)):
	n.write("vn %f %f %f\n" % (norms[i][0], norms[i][1], norms[i][2]))

for i in range(0, len(texts)):
	n.write("vt %f %f\n" % (texts[i][0], texts[i][1]))

for i in range(0, len(vidx), 3) :
	n.write("f %d/%d/%d %d/%d/%d %d/%d/%d\n" % (vidx[i], tidx[i], nidx[i], vidx[i + 1], tidx[i + 1], nidx[i + 1], vidx[i + 2], tidx[i + 2], nidx[i + 2]))	


n.close()

print("Done!\n")
