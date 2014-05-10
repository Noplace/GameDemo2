struct Ray {
  float3 o;
  float3 d;
};



void IntersectSlab(float o, float d, float l, float h, inout bool hit, inout float tnear, inout float tfar)
{
	if (d==0.0) {
		// ray parallel to planes
		if ((o < l) || (o > h)) {
			// ray is not between slabs
			hit = false;
//			return;
		}
	}
	
	float t1 = (l - o) / d;
	float t2 = (h - o) / d;
	if (t1 > t2) {
		// swap so that t1 is nearest
		float temp = t1;
		t1 = t2;
		t2 = temp;
	}

	if (t1 > tnear) {
		tnear = t1;
	}
	if (t2 < tfar) {
		tfar = t2;
	}

	if (tnear > tfar) {
		// box missed
		hit = false;
	}
	if (tfar < 0) {
		// box behind ray
		hit = false;
	}
}

// Intersect ray with box
// note - could probably vectorize this better
bool IntersectBox(Ray r, float3 Bl, float3 Bh, out float tnear, out float tfar)
{
	tnear = -1e20;
	tfar = 1e20;
	bool hit = true;
	IntersectSlab(r.o.x, r.d.x, Bl.x, Bh.x, hit, tnear, tfar);
	IntersectSlab(r.o.y, r.d.y, Bl.y, Bh.y, hit, tnear, tfar);
	IntersectSlab(r.o.z, r.d.z, Bl.z, Bh.z, hit, tnear, tfar);
	return hit;
}

